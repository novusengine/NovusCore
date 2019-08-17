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

#include "RealmConnection.h"
#include <Networking/ByteBuffer.h>
#include <Cryptography/HMAC.h>
#include <Cryptography/BigNumber.h>
#include <Database/DatabaseConnector.h>
#include <Database/Cache/AuthDatabaseCache.h>

#include "../NovusEnums.h"
#include "../RealmHandler.h"
#include "../MessageHandler.h"

#include <zlib.h>
#include <map>

#pragma pack(push, 1)
struct sAuthChallenge
{
    u32 unk;
    u32 authSeed;

    u8 seed1[16];
    u8 seed2[16];

    void AddTo(ByteBuffer* buffer)
    {
        buffer->PutBytes(reinterpret_cast<u8*>(this), sizeof(sAuthChallenge));
    }

    void PutBytes(u8* dest, const u8* src, size_t size)
    {
        std::memcpy(dest, src, size);
    }
};
#pragma pack(pop)

bool RealmConnection::Start()
{
    std::shared_ptr<ByteBuffer> authPacket = ByteBuffer::Borrow<sizeof(sAuthChallenge)>();
    BigNumber seed1, seed2;
    sAuthChallenge challenge;
    seed1.Rand(16 * 8);
    seed2.Rand(16 * 8);

    challenge.unk = 1;
    challenge.authSeed = _seed;
    challenge.PutBytes(challenge.seed1, seed1.BN2BinArray(16).get(), 16);
    challenge.PutBytes(challenge.seed2, seed2.BN2BinArray(16).get(), 16);
    challenge.AddTo(authPacket.get());

    if (_resumeConnection)
    {
        std::shared_ptr<ByteBuffer> resumeComms = ByteBuffer::Borrow<0>();
        SendPacket(resumeComms.get(), Opcode::SMSG_RESUME_COMMS);
    }
    SendPacket(authPacket.get(), Opcode::SMSG_AUTH_CHALLENGE);

    AsyncRead();
    return true;
}

void RealmConnection::HandleRead()
{
    std::shared_ptr<ByteBuffer> buffer = GetReceiveBuffer();
    while (buffer->GetActiveSize())
    {
        // Check if we should read header
        if (!_headerBuffer.IsFull())
        {
            size_t headerSize = std::min(buffer->GetActiveSize(), _headerBuffer.GetRemainingSpace());
            _headerBuffer.PutBytes(buffer->GetReadPointer(), headerSize);

            // Wait for full header
            if (!_headerBuffer.IsFull())
                break;

            // Handle newly received header
            if (!HandleNewHeader())
            {
                Close(asio::error::shut_down);
                return;
            }

            buffer->ReadData += headerSize;
        }

        if (!_packetBuffer.IsFull() || _packetBuffer.Size == 0)
        {
            if (_packetBuffer.Size != 0)
            {
                u32 packetSize = std::min(buffer->GetActiveSize(), _packetBuffer.GetRemainingSpace());
                if (packetSize != 0)
                {
                    _packetBuffer.PutBytes(buffer->GetReadPointer(), packetSize);
                    buffer->ReadData += packetSize;
                }
            }

            // Wait for full packet
            if (!_packetBuffer.IsFull())
                break;

            if (!HandleNewPacket())
            {
                Close(asio::error::shut_down);
                return;
            }

            _headerBuffer.Reset();
        }
    }

    AsyncRead();
}

bool RealmConnection::HandleNewHeader()
{
    ClientPacketHeader* header = reinterpret_cast<ClientPacketHeader*>(_headerBuffer.GetReadPointer());
    _streamEncryption.Decrypt(_headerBuffer.GetReadPointer(), sizeof(ClientPacketHeader));

    // Reverse size bytes
    EndianConvertReverse(header->size);

    if ((header->size < 4 && header->size > 10240) || header->command >= Opcode::NUM_MSG_TYPES)
    {
        std::cout << "header->size < 4: " << (header->size < 4) << ", header->size > 10240: " << (header->size > 10240) << ", header->command >= Common::Opcode::NUM_MSG_TYPES: " << (header->command >= Opcode::NUM_MSG_TYPES) << std::endl;
        return false;
    }

    header->size -= sizeof(header->command);
    _packetBuffer.Size = header->size;
    _packetBuffer.Reset();
    return true;
}

bool RealmConnection::HandleNewPacket()
{
    ClientPacketHeader* header = reinterpret_cast<ClientPacketHeader*>(_headerBuffer.GetReadPointer());
    Opcode opcode = static_cast<Opcode>(header->command);

    MessageHandler* messageHandler = MessageHandler::Instance();
    return messageHandler->CallHandler(opcode, this);
}

void RealmConnection::SendPacket(ByteBuffer* packet, Opcode opcode)
{
    ServerPacketHeader header(packet->GetActiveSize() + 2, opcode);
    u8 headerSize = header.GetLength();
    i32 packetSize = packet->GetActiveSize() + headerSize;

    _streamEncryption.Encrypt(header.data, headerSize);
    _sendBuffer->Size = packetSize;

    if (!_sendBuffer->PutBytes(header.data, headerSize))
        return;

    if (!_sendBuffer->IsFull())
    {
        if (!_sendBuffer->PutBytes(packet->GetInternalData(), packet->WrittenData))
            return;
    }

    Send(_sendBuffer.get());
    _sendBuffer->Reset();
}

void RealmConnection::HandleAuthSession()
{
    /* Read AuthSession Data */
    sessionData.Read(_packetBuffer);

    ByteBuffer AddonInfo(nullptr, _packetBuffer.Size - _packetBuffer.ReadData);
    AddonInfo.PutBytes(_packetBuffer.GetInternalData() + _packetBuffer.ReadData, AddonInfo.Size);

    if (AddonInfo.ReadData + 4 <= AddonInfo.Size)
    {
        u32 size = 0;
        AddonInfo.GetU32(size);

        if (size > 0 && size < 0xFFFFF)
        {
            uLongf uSize = size;
            u32 pos = static_cast<u32>(AddonInfo.ReadData);

            ByteBuffer addonInfo(nullptr, size);
            if (uncompress(addonInfo.GetInternalData(), &uSize, AddonInfo.GetInternalData() + pos, static_cast<uLong>(AddonInfo.Size - pos)) == Z_OK)
            {
                u32 addonsCount = 0;
                addonInfo.GetU32(addonsCount);

                for (u32 i = 0; i < addonsCount; ++i)
                {
                    std::string addonName;
                    u8 enabled;
                    u32 crc, unk1;

                    if (addonInfo.ReadData + 1 <= addonInfo.Size)
                    {
                        addonInfo.GetString(addonName);
                        addonInfo.GetU8(enabled);
                        addonInfo.GetU32(crc);
                        addonInfo.GetU32(unk1);
                    }
                }
            }
        }
    }

    std::shared_ptr<DatabaseConnector> connector = nullptr;
    if (!DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, connector))
    {
        Close(asio::error::interrupted);
        return;
    }

    amy::result_set results;
    PreparedStatement stmt("SELECT guid, sessionKey FROM accounts WHERE username={s};");
    stmt.Bind(sessionData.accountName);
    if (!connector->Query(stmt, results))
    {
        Close(asio::error::interrupted);
        return;
    }

    // Make sure the account exist.
    if (results.affected_rows() != 1)
    {
        Close(asio::error::interrupted);
        return;
    }

    // We need to try to use the session key that we have, if we don't the client won't be able to read the auth response error.
    sessionKey->Hex2BN(results[0][1].as<amy::sql_varchar>().c_str());

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

    _streamEncryption.Setup(sessionKey);
    account = results[0][0].GetU32();

    charCache.LoadCharactersForAccount(account);

    /* SMSG_AUTH_RESPONSE */
    std::shared_ptr<ByteBuffer> authResponse = ByteBuffer::Borrow<11>();
    authResponse->PutU8(AUTH_OK);
    authResponse->PutU32(0);
    authResponse->PutU8(0);
    authResponse->PutU32(0);
    authResponse->PutU8(2); // Expansion
    SendPacket(authResponse.get(), Opcode::SMSG_AUTH_RESPONSE);

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

    /*u8 addonPublicKey[256] =
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
    };*/

    std::shared_ptr<ByteBuffer> addonInfo = ByteBuffer::Borrow<1024>();
    for (auto addon : addonMap)
    {
        addonInfo->PutU8(2); // State
        addonInfo->PutU8(1); // UsePublicKeyOrCRC

        // if (UsePublicKeyOrCRC)
        {
            u8 usepk = (addon.second != 1276933997);
            addonInfo->PutU8(usepk);

            if (usepk)
            {
                std::cout << "Addon Mismatch (" << addon.first << "," << addon.second << ")" << std::endl;
            }

            addonInfo->PutU32(0); // What does this mean?
        }

        addonInfo->PutU8(0); // Uses URL
    }

    addonInfo->PutU32(0); // Size of banned addon list
    SendPacket(addonInfo.get(), Opcode::SMSG_ADDON_INFO);

    std::shared_ptr<ByteBuffer> clientCache = ByteBuffer::Borrow<4>();
    clientCache->PutU32(0);
    SendPacket(clientCache.get(), Opcode::SMSG_CLIENTCACHE_VERSION);

    // Tutorial Flags : REQUIRED
    std::shared_ptr<ByteBuffer> tutorialFlags = ByteBuffer::Borrow<32>();
    for (i32 i = 0; i < 8; i++)
        tutorialFlags->PutU32(0xFF);

    SendPacket(tutorialFlags.get(), Opcode::SMSG_TUTORIAL_FLAGS);

    if (_resumeConnection)
    {
        std::shared_ptr<ByteBuffer> logoutRequest = ByteBuffer::Borrow<0>();
        SendPacket(logoutRequest.get(), Opcode::SMSG_LOGOUT_COMPLETE);

        std::shared_ptr<DatabaseConnector> connector = nullptr;
        if (!DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector))
        {
            Close(asio::error::interrupted);
            return;
        }

        amy::result_set results;
        PreparedStatement stmt("SELECT guid FROM characters WHERE account={u} ORDER BY lastLogin DESC LIMIT 1;");
        stmt.Bind(account);
        if (!connector->Query(stmt, results))
        {
            Close(asio::error::interrupted);
            return;
        }

        characterGuid = results[0][0].GetU64();
    }
}