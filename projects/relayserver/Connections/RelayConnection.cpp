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
#include <zlib.h>
#include <map>

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
    u32 unk;
    u32 authSeed;

    u8 seed1[16];
    u8 seed2[16];

    void AddTo(Common::ByteBuffer& buffer)
    {
        buffer.Append((u8*)this, sizeof(sAuthChallenge));
    }

    void Append(u8* dest, const u8* src, size_t size)
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
            pong.Write<u32>(0);
            SendPacket(pong, Common::Opcode::SMSG_PONG);
            break;
        }
        case Common::Opcode::CMSG_KEEP_ALIVE:
            break;
        case Common::Opcode::CMSG_AUTH_SESSION:
            HandleAuthSession();
            break;
        default:
            std::cout << "Forwarding Opcode: 0x" << std::uppercase << std::hex << opcode << std::dec << "(" << opcode << ")" << std::endl;

            if (opcode == Common::Opcode::CMSG_READY_FOR_ACCOUNT_DATA_TIMES || opcode == Common::Opcode::CMSG_UPDATE_ACCOUNT_DATA
                || opcode == Common::Opcode::CMSG_REALM_SPLIT || opcode == Common::Opcode::CMSG_CHAR_ENUM
                || opcode == Common::Opcode::CMSG_CHAR_CREATE || opcode == Common::Opcode::CMSG_CHAR_DELETE)
            {
                if (NovusConnection* characterServer = NovusConnectionHandler::GetCharacterConnection())
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
            }
            else
            {
                if (NovusConnection* worldServer = NovusConnectionHandler::GetWorldConnection(worldId))
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

                    worldServer->_crypto->Encrypt(forwardedPacket.GetReadPointer(), forwardedPacket.GetActualSize());
                    worldServer->Send(forwardedPacket);
                }
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

    if (AddonInfo._readPos + 4 <= AddonInfo.size())
    {
        u32 size;
        AddonInfo.Read(&size, 4);

        if (size > 0 && size < 0xFFFFF)
        {
            uLongf uSize = size;
            u32 pos = (u32)AddonInfo._readPos;

            Common::ByteBuffer addonInfo;
            addonInfo.Resize(size);

            if (uncompress(addonInfo.data(), &uSize, AddonInfo.data() + pos, AddonInfo.size() - pos) == Z_OK)
            {
                u32 addonsCount;
                addonInfo.Read(&addonsCount, 4);

                for (u32 i = 0; i < addonsCount; ++i)
                {
                    std::string addonName;
                    u8 enabled;
                    u32 crc, unk1;

                    if (addonInfo._readPos + 1 <= addonInfo.size())
                    {
                        addonInfo.Read(addonName);
                        addonInfo.Read(&enabled, 1);
                        addonInfo.Read(&crc, 4);
                        addonInfo.Read(&unk1, 4);
                    }
                }
            }
        }
    }

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
        u32 t = 0;
        sha.UpdateHash(sessionData.accountName);
        sha.UpdateHash((u8*)&t, 4);
        sha.UpdateHash((u8*)&sessionData.localChallenge, 4);
        sha.UpdateHash((u8*)&_seed, 4);
        sha.UpdateHashForBn(1, sessionKey);
        sha.Finish();

        if (memcmp(sha.GetData(), sessionData.digest, SHA_DIGEST_LENGTH) != 0)
        {
            Close(asio::error::interrupted);
            return;
        }

        /* SMSG_AUTH_RESPONSE */
        Common::ByteBuffer packet(1 + 4 + 1 + 4 + 1);
        packet.Write<u8>(AUTH_OK);
        packet.Write<u32>(0);
        packet.Write<u8>(0);
        packet.Write<u32>(0);
        packet.Write<u8>(2); // Expansion
        SendPacket(packet, Common::Opcode::SMSG_AUTH_RESPONSE);

        std::map<std::string, u32> addonMap;
        addonMap.insert(std::make_pair("Blizzard_AchievementUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_ArenaUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_AuctionUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_BarbershopUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_BattlefieldMinimap", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_BindingUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_Calendar", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_CombatLog", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_CombatText", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_DebugTools", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_GlyphUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_GMChatUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_GMSurveyUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_GuildBankUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_InspectUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_ItemSocketingUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_MacroUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_RaidUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_TalentUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_TimeManager", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_TokenUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_TradeSkillUI", 1276933997));
        addonMap.insert(std::make_pair("Blizzard_TrainerUI", 1276933997));

        u8 addonPublicKey[256] =
        {
            0xC3, 0x5B, 0x50, 0x84, 0xB9, 0x3E, 0x32, 0x42, 0x8C, 0xD0, 0xC7, 0x48, 0xFA, 0x0E, 0x5D, 0x54,
            0x5A, 0xA3, 0x0E, 0x14, 0xBA, 0x9E, 0x0D, 0xB9, 0x5D, 0x8B, 0xEE, 0xB6, 0x84, 0x93, 0x45, 0x75,
            0xFF, 0x31, 0xFE, 0x2F, 0x64, 0x3F, 0x3D, 0x6D, 0x07, 0xD9, 0x44, 0x9B, 0x40, 0x85, 0x59, 0x34,
            0x4E, 0x10, 0xE1, 0xE7, 0x43, 0x69, 0xEF, 0x7C, 0x16, 0xFC, 0xB4, 0xED, 0x1B, 0x95, 0x28, 0xA8,
            0x23, 0x76, 0x51, 0x31, 0x57, 0x30, 0x2B, 0x79, 0x08, 0x50, 0x10, 0x1C, 0x4A, 0x1A, 0x2C, 0xC8,
            0x8B, 0x8F, 0x05, 0x2D, 0x22, 0x3D, 0xDB, 0x5A, 0x24, 0x7A, 0x0F, 0x13, 0x50, 0x37, 0x8F, 0x5A,
            0xCC, 0x9E, 0x04, 0x44, 0x0E, 0x87, 0x01, 0xD4, 0xA3, 0x15, 0x94, 0x16, 0x34, 0xC6, 0xC2, 0xC3,
            0xFB, 0x49, 0xFE, 0xE1, 0xF9, 0xDA, 0x8C, 0x50, 0x3C, 0xBE, 0x2C, 0xBB, 0x57, 0xED, 0x46, 0xB9,
            0xAD, 0x8B, 0xC6, 0xDF, 0x0E, 0xD6, 0x0F, 0xBE, 0x80, 0xB3, 0x8B, 0x1E, 0x77, 0xCF, 0xAD, 0x22,
            0xCF, 0xB7, 0x4B, 0xCF, 0xFB, 0xF0, 0x6B, 0x11, 0x45, 0x2D, 0x7A, 0x81, 0x18, 0xF2, 0x92, 0x7E,
            0x98, 0x56, 0x5D, 0x5E, 0x69, 0x72, 0x0A, 0x0D, 0x03, 0x0A, 0x85, 0xA2, 0x85, 0x9C, 0xCB, 0xFB,
            0x56, 0x6E, 0x8F, 0x44, 0xBB, 0x8F, 0x02, 0x22, 0x68, 0x63, 0x97, 0xBC, 0x85, 0xBA, 0xA8, 0xF7,
            0xB5, 0x40, 0x68, 0x3C, 0x77, 0x86, 0x6F, 0x4B, 0xD7, 0x88, 0xCA, 0x8A, 0xD7, 0xCE, 0x36, 0xF0,
            0x45, 0x6E, 0xD5, 0x64, 0x79, 0x0F, 0x17, 0xFC, 0x64, 0xDD, 0x10, 0x6F, 0xF3, 0xF5, 0xE0, 0xA6,
            0xC3, 0xFB, 0x1B, 0x8C, 0x29, 0xEF, 0x8E, 0xE5, 0x34, 0xCB, 0xD1, 0x2A, 0xCE, 0x79, 0xC3, 0x9A,
            0x0D, 0x36, 0xEA, 0x01, 0xE0, 0xAA, 0x91, 0x20, 0x54, 0xF0, 0x72, 0xD8, 0x1E, 0xC7, 0x89, 0xD2
        };

        Common::ByteBuffer addonInfo(4);
        for (auto addon : addonMap)
        {
            addonInfo.Write<u8>(2); // State
            addonInfo.Write<u8>(1); // UsePublicKeyOrCRC

            // if (UsePublicKeyOrCRC)
            {
                u8 usepk = (addon.second != 1276933997);
                addonInfo.Write<u8>(usepk);

                if (usepk)
                {
                    std::cout << "Addon Mismatch (" << addon.first << "," << addon.second << ")" << std::endl;
                }

                addonInfo.Write<u32>(0); // What does this mean?
            }

            addonInfo.Write<u8>(0); // Uses URL
        }

        addonInfo.Write<u32>(0); // Size of banned addon list
        SendPacket(addonInfo, Common::Opcode::SMSG_ADDON_INFO);

        Common::ByteBuffer clientCache(4);
        clientCache.Write<u32>(0);
        SendPacket(clientCache, Common::Opcode::SMSG_CLIENTCACHE_VERSION);

        // Tutorial Flags : REQUIRED
        Common::ByteBuffer tutorialFlags(4 * 8);
        for (int i = 0; i < 8; i++)
            tutorialFlags.Write<u32>(0xFF);

        SendPacket(tutorialFlags, Common::Opcode::SMSG_TUTORIAL_FLAGS);
        worldId = 1;
    });
}