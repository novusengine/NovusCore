/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR a PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/

#include "RelayConnection.h"
#include "Networking\ByteBuffer.h"
#include <Cryptography\BigNumber.h>
#include <Database\DatabaseConnector.h>

#include "../ConnectionHandlers/NovusConnectionHandler.h"

enum AuthResponse
{
    AUTH_OK                                                = 12,
    AUTH_FAILED                                            = 13,
    AUTH_REJECT                                            = 14,
    AUTH_BAD_SERVER_PROOF                                  = 15,
    AUTH_UNAVAILABLE                                       = 16,
    AUTH_SYSTEM_ERROR                                      = 17,
    AUTH_BILLING_ERROR                                     = 18,
    AUTH_BILLING_EXPIRED                                   = 19,
    AUTH_VERSION_MISMATCH                                  = 20,
    AUTH_UNKNOWN_ACCOUNT                                   = 21,
    AUTH_INCORRECT_PASSWORD                                = 22,
    AUTH_SESSION_EXPIRED                                   = 23,
    AUTH_SERVER_SHUTTING_DOWN                              = 24,
    AUTH_ALREADY_LOGGING_IN                                = 25,
    AUTH_LOGIN_SERVER_NOT_FOUND                            = 26,
    AUTH_WAIT_QUEUE                                        = 27,
    AUTH_BANNED                                            = 28,
    AUTH_ALREADY_ONLINE                                    = 29,
    AUTH_NO_TIME                                           = 30,
    AUTH_DB_BUSY                                           = 31,
    AUTH_SUSPENDED                                         = 32,
    AUTH_PARENTAL_CONTROL                                  = 33,
    AUTH_LOCKED_ENFORCED                                   = 34
};

#pragma pack(push, 1)
struct sAuthChallenge
{
    uint32_t unk;
    uint32_t authSeed;

    uint8_t seed1[16];
    uint8_t seed2[16];

    void AddTo(Common::ByteBuffer& buffer)
    {
        buffer.Append((uint8_t*)this, sizeof(sAuthChallenge));
    }

    void Append(uint8_t* dest, const uint8_t* src, size_t size)
    {
        std::memcpy(dest, src, size);
    }
};
#pragma pack(pop)

bool RelayConnection::Start()
{
    AsyncRead();

    BigNumber seed1;
    seed1.Rand(16 * 8);

    BigNumber seed2;
    seed2.Rand(16 * 8);

    Common::ByteBuffer authPacket;
    sAuthChallenge challenge;
    challenge.unk = 1;
    challenge.authSeed = _seed;
    challenge.Append(challenge.seed1, seed1.BN2BinArray(32).get(), 16);
    challenge.Append(challenge.seed2, seed2.BN2BinArray(32).get(), 16);
    challenge.AddTo(authPacket);

    SendPacket(authPacket, Common::Opcode::SMSG_AUTH_CHALLENGE);
    return true;
}

void RelayConnection::HandleRead()
{
    Common::ByteBuffer& buffer = GetByteBuffer();

    while (buffer.GetActualSize() > 0)
    {
        // Check if we should read header
        if (_headerBuffer.GetSpaceLeft() > 0)
        {
            size_t headerSize = std::min(buffer.GetActualSize(), _headerBuffer.GetSpaceLeft());
            _headerBuffer.Write(buffer.GetReadPointer(), headerSize);
            buffer.ReadBytes(headerSize);

            if (_headerBuffer.GetSpaceLeft() > 0)
            {
                // Wait until we have the entire header
                assert(buffer.GetActualSize() == 0);
                break;
            }

            // Handle newly received header
            if (!HandleHeaderRead())
            {
                Close(asio::error::shut_down);
                return;
            }
        }

        // We have a header, now check the packet data
        if (_packetBuffer.GetSpaceLeft() > 0)
        {
            size_t packetSize = std::min(buffer.GetActualSize(), _packetBuffer.GetSpaceLeft());
            _packetBuffer.Write(buffer.GetReadPointer(), packetSize);
            buffer.ReadBytes(packetSize);

            if (_packetBuffer.GetSpaceLeft() > 0)
            {
                // Wait until we have all of the packet data
                assert(buffer.GetActualSize() == 0);
                break;
            }
        }

        // Handle newly received packet
        if (!HandlePacketRead())
        {
            Close(asio::error::shut_down);
            return;
        }
        _headerBuffer.ResetPos();
    }

    AsyncRead();
}

bool RelayConnection::HandleHeaderRead()
{
    // Make sure header buffer matches ClientPacketHeader size & Decrypt if required
    assert(_headerBuffer.GetActualSize() == sizeof(Common::ClientPacketHeader));
    _streamCrypto.Decrypt(_headerBuffer.GetReadPointer(), sizeof(Common::ClientPacketHeader));

    Common::ClientPacketHeader* header = reinterpret_cast<Common::ClientPacketHeader*>(_headerBuffer.GetReadPointer());

    // Reverse size bytes
    EndianConvertReverse(header->size);

    if (header->size < 4 && header->size > 10240 || header->command >= Common::Opcode::NUM_MSG_TYPES)
    {
        std::cout << "header->size < 4: " << (header->size < 4) << ", header->size > 10240: " << (header->size > 10240) << ", header->command >= Common::Opcode::NUM_MSG_TYPES: " << (header->command >= Common::Opcode::NUM_MSG_TYPES) << std::endl;
        return false;
    }

    header->size -= sizeof(header->command);
    _packetBuffer.Resize(header->size);
    _packetBuffer.ResetPos();
    return true;
}

bool RelayConnection::HandlePacketRead()
{
    Common::ClientPacketHeader* header = reinterpret_cast<Common::ClientPacketHeader*>(_headerBuffer.GetReadPointer());
    Common::Opcode opcode = (Common::Opcode)header->command;

    switch (opcode)
    {
        case Common::Opcode::CMSG_PING:
        {
            Common::ByteBuffer pong(4);
            pong.Write<uint32_t>(0);
            SendPacket(pong, Common::Opcode::SMSG_PONG);
            break;
        }
        case Common::Opcode::CMSG_KEEP_ALIVE:
            break;
        case Common::Opcode::CMSG_AUTH_SESSION:
            HandleAuthSession();
            break;
        default:
            //std::cout << "Forwarding Opcode: 0x" << std::uppercase << std::hex << opcode << std::dec << "(" << opcode << ")" << std::endl;
            if (NovusConnection* characterServer = NovusConnectionHandler::GetConnection())
            {
                Common::ByteBuffer forwardedPacket;

                // Write the base novus structure for the packet
                NovusHeader packetHeader;
                packetHeader.command = NOVUS_FOWARDPACKET;
                packetHeader.account = account;
                packetHeader.opcode = opcode;
                packetHeader.size = _packetBuffer.GetActualSize();
                packetHeader.AddTo(forwardedPacket);

                // Add packet content (Append while make sure we only write if there is content)
                forwardedPacket.Append(_packetBuffer);

                characterServer->_crypto->Encrypt(forwardedPacket.GetReadPointer(), forwardedPacket.GetActualSize());
                characterServer->Send(forwardedPacket);
            }
            break;
    }

    return true;
}

void RelayConnection::SendPacket(Common::ByteBuffer& packet, Common::Opcode opcode)
{
    int packetSize = packet.GetActualSize() + 5;
    Common::ByteBuffer buffer(packetSize);
    buffer.Resize(packetSize);

    Common::ServerPacketHeader header(packet.size() + 2, opcode);
    _streamCrypto.Encrypt(header.headerArray, header.GetLength());

    buffer.Write(header.headerArray, header.GetLength());
    buffer.Write(packet.data(), packet.size());
    Send(buffer);
}

void RelayConnection::HandleAuthSession()
{
    /* Read AuthSession Data */
    sessionData.Read(_packetBuffer);

    Common::ByteBuffer AddonInfo;
    AddonInfo.Append(_packetBuffer.data() + _packetBuffer._readPos, _packetBuffer.size() - _packetBuffer._readPos);

    PreparedStatement stmt("SELECT guid, sessionKey FROM accounts WHERE username={s};");
    stmt.Bind(sessionData.accountName);
    DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, stmt, [this](amy::result_set& results, DatabaseConnector& connector)
    {
        // Make sure the account exist.
        if (results.affected_rows() != 1)
        {
            Close(asio::error::interrupted);
            return;
        }
        account = results[0][0].as<amy::sql_int_unsigned>();

        // We need to try to use the session key that we have, if we don't the client won't be able to read the auth response error.
        sessionKey->Hex2BN(results[0][1].as<amy::sql_varchar>().c_str());
        _streamCrypto.SetupServer(sessionKey);

        SHA1Hasher sha;
        uint32_t t = 0;
        sha.UpdateHash(sessionData.accountName);
        sha.UpdateHash((uint8_t*)&t, 4);
        sha.UpdateHash((uint8_t*)&sessionData.localChallenge, 4);
        sha.UpdateHash((uint8_t*)&_seed, 4);
        sha.UpdateHashForBn(1, sessionKey);
        sha.Finish();

        if (memcmp(sha.GetData(), sessionData.digest, SHA_DIGEST_LENGTH) != 0)
        {
            Close(asio::error::interrupted);
            return;
        }

        /* SMSG_AUTH_RESPONSE */
        Common::ByteBuffer packet(1 + 4 + 1 + 4 + 1);
        packet.Write<uint8_t>(AUTH_OK);
        packet.Write<uint32_t>(0);
        packet.Write<uint8_t>(0);
        packet.Write<uint32_t>(0);
        packet.Write<uint8_t>(2); // Expansion
        SendPacket(packet, Common::Opcode::SMSG_AUTH_RESPONSE);

        Common::ByteBuffer clientCache(4);
        clientCache.Write<uint32_t>(0);
        SendPacket(clientCache, Common::Opcode::SMSG_CLIENTCACHE_VERSION);

        // Tutorial Flags : REQUIRED
        Common::ByteBuffer tutorialFlags(4 * 8);
        for (int i = 0; i < 8; i++)
            tutorialFlags.Write<uint32_t>(0xFF);

        SendPacket(tutorialFlags, Common::Opcode::SMSG_TUTORIAL_FLAGS);

    });
}