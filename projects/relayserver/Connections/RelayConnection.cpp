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

using namespace std::chrono;
static const steady_clock::time_point ApplicationStartTime = steady_clock::now();

bool RelayConnection::Start()
{
    AsyncRead();

    /* SMSG_AUTH_CHALLENGE */
    Common::ByteBuffer authChallenge(37);
    authChallenge.Write<uint32_t>(1); // Unk
    authChallenge.Write<uint32_t>(_seed);

    BigNumber s1;
    s1.Rand(16 * 8);
    authChallenge.Append(s1.BN2BinArray(16).get(), 16);

    BigNumber s2;
    s2.Rand(16 * 8);
    authChallenge.Append(s2.BN2BinArray(16).get(), 16);

    SendPacket(authChallenge, Common::Opcode::SMSG_AUTH_CHALLENGE);
    return true;
}

void RelayConnection::HandleRead()
{
    Common::ByteBuffer& buffer = GetByteBuffer();

    while (buffer.GetActualSize() > 0)
    {
        if (_headerBuffer.GetSpaceLeft() > 0)
        {
            // need to receive the header
            std::size_t readHeaderSize = std::min(buffer.GetActualSize(), _headerBuffer.GetSpaceLeft());
            _headerBuffer.Write(buffer.GetReadPointer(), readHeaderSize);
            buffer.ReadBytes(readHeaderSize);

            if (_headerBuffer.GetSpaceLeft() > 0)
            {
                // Couldn't receive the whole header this time.
                assert(buffer.GetActualSize() == 0);
                break;
            }

            // We just nice new header
            //std::cout << "Just new header" << std::endl;
            if (!HandleHeaderRead())
            {
                Close(asio::error::shut_down);
                return;
            }
        }

        // We have full read header, now check the data payload
        if (_packetBuffer.GetSpaceLeft() > 0)
        {
            // need more data in the payload
            std::size_t readDataSize = std::min(buffer.GetActualSize(), _packetBuffer.GetSpaceLeft());
            _packetBuffer.Write(buffer.GetReadPointer(), readDataSize);
            buffer.ReadBytes(readDataSize);

            if (_packetBuffer.GetSpaceLeft() > 0)
            {
                // Couldn't receive the whole data this time.
                assert(buffer.GetActualSize() == 0);
                break;
            }
        }

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
            std::cout << "CMSG_PING Opcode: 0x" << std::uppercase << std::hex << opcode << std::dec << "(" << opcode << ")" << std::endl;

            Common::ByteBuffer pong(4);
            pong.Write<uint32_t>(0);
            SendPacket(pong, Common::Opcode::SMSG_PONG);
            break;
        }
        case Common::Opcode::CMSG_KEEP_ALIVE:
            std::cout << "CMSG_KEEP_ALIVE Opcode: 0x" << std::uppercase << std::hex << opcode << std::dec << "(" << opcode << ")" << std::endl;
            break;
        case Common::Opcode::CMSG_AUTH_SESSION:
            std::cout << "CMSG_AUTH_SESSION Opcode: 0x" << std::uppercase << std::hex << opcode << std::dec << "(" << opcode << ")" << std::endl;
            HandleAuthSession();
            break;
        default:
            std::cout << "Forwarding Opcode: 0x" << std::uppercase << std::hex << opcode << std::dec << "(" << opcode << ")" << std::endl;
            if (NovusConnection* characterServer = NovusConnectionHandler::GetConnection())
            {
                Common::ByteBuffer forwardedPacket;

                // Write the base novus structure for the packet
                forwardedPacket.Write<uint8_t>(NOVUS_FOWARDPACKET);
                forwardedPacket.Write<uint64_t>(accountGuid);
                forwardedPacket.Write<uint16_t>(opcode);

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
    uint32_t Build = 0;
    uint32_t LoginServerID = 0;
    std::string Account;
    uint32_t LoginServerType = 0;
    uint32_t LocalChallenge = 0;
    uint32_t RegionID = 0;
    uint32_t BattlegroupID = 0;
    uint32_t RealmID = 0;
    uint64_t DosResponse = 0;
    uint8_t Digest[SHA_DIGEST_LENGTH] = {};
    Common::ByteBuffer AddonInfo;

    _packetBuffer.Read(&Build, 4);
    _packetBuffer.Read(&LoginServerID, 4);
    _packetBuffer.Read(Account);
    _packetBuffer.Read(&LoginServerType, 4);
    _packetBuffer.Read(&LocalChallenge, 4);
    _packetBuffer.Read(&RegionID, 4);
    _packetBuffer.Read(&BattlegroupID, 4);
    _packetBuffer.Read(&RealmID, 4);
    _packetBuffer.Read(&DosResponse, 8);
    _packetBuffer.Read(&Digest, 20);
    AddonInfo.Append(_packetBuffer.data() + _packetBuffer._readPos, _packetBuffer.size() - _packetBuffer._readPos);

    // Make sure the account exist.
    PreparedStatement stmt("SELECT guid, sessionKey FROM accounts WHERE username={s};");
    stmt.Bind(Account);
    DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, stmt, [this, Account, LocalChallenge, Digest](amy::result_set& results, DatabaseConnector& connector)
    {
        if (results.affected_rows() != 1)
        {
            Close(asio::error::interrupted);
            return;
        }
        accountGuid = results[0][0].as<amy::sql_bigint_unsigned>();
        accountName = Account;

        // We need to try to use the session key that we have, if we don't the client won't be able to read the auth response error.
        sessionKey->Hex2BN(results[0][1].as<amy::sql_varchar>().c_str());
        _streamCrypto.SetupServer(sessionKey);

        SHA1Hasher sha;
        uint32_t t = 0;
        sha.UpdateHash(accountName);
        sha.UpdateHash((uint8_t*)&t, 4);
        sha.UpdateHash((uint8_t*)&LocalChallenge, 4);
        sha.UpdateHash((uint8_t*)&_seed, 4);
        sha.UpdateHashForBn(1, sessionKey);
        sha.Finish();

        if (memcmp(sha.GetData(), Digest, SHA_DIGEST_LENGTH) != 0)
        {
            std::cout << "Keys does not match" << std::endl;
            // BAD BAD WOLF, SCREEEETCH
            Close(asio::error::interrupted);
            return;
        }

        Common::ByteBuffer packet(1);
        packet.Write<uint8_t>(12);
        SendPacket(packet, Common::Opcode::SMSG_AUTH_RESPONSE);
        std::cout << "SMSG_AUTH_RESPONSE" << std::endl;

        Common::ByteBuffer clientCache(4);
        clientCache.Write<uint32_t>(0);
        SendPacket(clientCache, Common::Opcode::SMSG_CLIENTCACHE_VERSION);
        std::cout << "SMSG_CLIENTCACHE_VERSION" << std::endl;

        // Tutorial Flags : REQUIRED
        Common::ByteBuffer tutorialFlags(4 * 8);
        for (int i = 0; i < 8; i++)
            tutorialFlags.Write<uint32_t>(0xFF);

        SendPacket(tutorialFlags, Common::Opcode::SMSG_TUTORIAL_FLAGS);
        std::cout << "SMSG_TUTORIAL_FLAGS" << std::endl;

    });
}