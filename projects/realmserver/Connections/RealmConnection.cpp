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
#include "Networking/ByteBuffer.h"
#include <Cryptography/BigNumber.h>
#include <Cryptography/HMAC.h>
#include <Database/DatabaseConnector.h>

#include <zlib.h>
#include <map>

#include "../Utils/CharacterUtils.h"


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

enum EnterWorldResults
{
    ENTER_FAILED                                = 0x00,
    ENTER_FAILED_WORLDSERVER_DOWN               = 0x01,
    ENTER_FAILED_CHARACTER_WITH_NAME_EXISTS     = 0x02,
    ENTER_FAILED_NO_INSTANCE_SERVER_AVAILABLE   = 0x03,
    ENTER_FAILED_RACE_CLASS_UNAVAILABLE         = 0x04,
    ENTER_FAILED_CHARACTER_NOT_FOUND            = 0x05,
    ENTER_FAILED_CHARACTER_UPDATE_IN_PROGRESS   = 0x06,
    ENTER_FAILED_LOCKED_NO_GAMETIME             = 0x07,
    ENTER_FAILED_CANT_LOGIN_REMOTELY            = 0x08,
};

enum CharacterResponses
{
    CHAR_CREATE_IN_PROGRESS = 46,
    CHAR_CREATE_SUCCESS = 47,
    CHAR_CREATE_ERROR = 48,
    CHAR_CREATE_FAILED = 49,
    CHAR_CREATE_NAME_IN_USE = 50,
    CHAR_CREATE_DISABLED = 51,
    CHAR_CREATE_PVP_TEAMS_VIOLATION = 52,
    CHAR_CREATE_SERVER_LIMIT = 53,
    CHAR_CREATE_ACCOUNT_LIMIT = 54,
    CHAR_CREATE_SERVER_QUEUE = 55,
    CHAR_CREATE_ONLY_EXISTING = 56,
    CHAR_CREATE_EXPANSION = 57,
    CHAR_CREATE_EXPANSION_CLASS = 58,
    CHAR_CREATE_LEVEL_REQUIREMENT = 59,
    CHAR_CREATE_UNIQUE_CLASS_LIMIT = 60,
    CHAR_CREATE_CHARACTER_IN_GUILD = 61,
    CHAR_CREATE_RESTRICTED_RACECLASS = 62,
    CHAR_CREATE_CHARACTER_CHOOSE_RACE = 63,
    CHAR_CREATE_CHARACTER_ARENA_LEADER = 64,
    CHAR_CREATE_CHARACTER_DELETE_MAIL = 65,
    CHAR_CREATE_CHARACTER_SWAP_FACTION = 66,
    CHAR_CREATE_CHARACTER_RACE_ONLY = 67,
    CHAR_CREATE_CHARACTER_GOLD_LIMIT = 68,
    CHAR_CREATE_FORCE_LOGIN = 69,

    CHAR_DELETE_IN_PROGRESS = 70,
    CHAR_DELETE_SUCCESS = 71,
    CHAR_DELETE_FAILED = 72,
    CHAR_DELETE_FAILED_LOCKED_FOR_TRANSFER = 73,
    CHAR_DELETE_FAILED_GUILD_LEADER = 74,
    CHAR_DELETE_FAILED_ARENA_CAPTAIN = 75,
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
        buffer.Append(reinterpret_cast<u8*>(this), sizeof(sAuthChallenge));
    }

    void Append(u8* dest, const u8* src, size_t size)
    {
        std::memcpy(dest, src, size);
    }
};
#pragma pack(pop)

bool RealmConnection::Start()
{
    BigNumber seed1;
    seed1.Rand(16 * 8);

    BigNumber seed2;
    seed2.Rand(16 * 8);

    Common::ByteBuffer authPacket;
    sAuthChallenge challenge;
    challenge.unk = 1;
    challenge.authSeed = _seed;
    challenge.Append(challenge.seed1, seed1.BN2BinArray(16).get(), 16);
    challenge.Append(challenge.seed2, seed2.BN2BinArray(16).get(), 16);
    challenge.AddTo(authPacket);

    SendPacket(authPacket, Common::Opcode::SMSG_AUTH_CHALLENGE);

    AsyncRead();
    return true;
}

void RealmConnection::HandleRead()
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

bool RealmConnection::HandleHeaderRead()
{
    // Make sure header buffer matches ClientPacketHeader size & Decrypt if required
    assert(_headerBuffer.GetActualSize() == sizeof(Common::ClientPacketHeader));
    _streamCrypto.Decrypt(_headerBuffer.GetReadPointer(), sizeof(Common::ClientPacketHeader));

    Common::ClientPacketHeader* header = reinterpret_cast<Common::ClientPacketHeader*>(_headerBuffer.GetReadPointer());

    // Reverse size bytes
    EndianConvertReverse(header->size);

    if ((header->size < 4 && header->size > 10240) || header->command >= Common::Opcode::NUM_MSG_TYPES)
    {
        std::cout << "header->size < 4: " << (header->size < 4) << ", header->size > 10240: " << (header->size > 10240) << ", header->command >= Common::Opcode::NUM_MSG_TYPES: " << (header->command >= Common::Opcode::NUM_MSG_TYPES) << std::endl;
        return false;
    }

    header->size -= sizeof(header->command);
    _packetBuffer.Resize(header->size);
    _packetBuffer.ResetPos();
    return true;
}

bool RealmConnection::HandlePacketRead()
{
    Common::ClientPacketHeader* header = reinterpret_cast<Common::ClientPacketHeader*>(_headerBuffer.GetReadPointer());
    Common::Opcode opcode = static_cast<Common::Opcode>(header->command);

    printf("Received Opcode: %u\n", opcode);
    switch (opcode)
    {
        case Common::Opcode::CMSG_PLAYER_LOGIN:
        {
            Common::ByteBuffer redirectClient;
            i32 ip = 16777343;
            i16 port = 9000;

            // 127.0.0.1/1.0.0.127
            // 2130706433/16777343(https://www.browserling.com/tools/ip-to-dec)
            redirectClient.Write<i32>(ip);
            redirectClient.Write<i16>(port);
            redirectClient.Write<i32>(0); // unk
#pragma warning(push)
#pragma warning(disable: 4312)
            HMACH hmac(40, sessionKey->BN2BinArray(20).get());
            hmac.UpdateHash((u8*)&ip, 4);
            hmac.UpdateHash((u8*)&port, 2);
            hmac.Finish();
            redirectClient.Append(hmac.GetData(), 20);
#pragma warning(pop)
            SendPacket(redirectClient, Common::Opcode::SMSG_REDIRECT_CLIENT);

            u64 characterGuid = 0;
            _packetBuffer.Read<u64>(characterGuid);

            DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, [&, characterGuid](std::shared_ptr<DatabaseConnector> & connector)
            {
                PreparedStatement stmt("UPDATE characters SET online=1 WHERE guid={u};");
                stmt.Bind(characterGuid);
                connector->Execute(stmt);

                
                /*I'm am not 100% sure where this fits into the picture yet, but I'm sure it has a purpose*/
                Common::ByteBuffer suspendComms;
                suspendComms.Write<u32>(1);
                SendPacket(suspendComms, Common::Opcode::SMSG_SUSPEND_COMMS);
                
            });
            break;
        }
        case Common::Opcode::CMSG_CONNECT_TO_FAILED:
        {
            Common::ByteBuffer loginFailed(1);
            loginFailed.Write<u8>(ENTER_FAILED_WORLDSERVER_DOWN);
            SendPacket(loginFailed, Common::Opcode::SMSG_CHARACTER_LOGIN_FAILED);
            break;
        }
        case Common::Opcode::CMSG_SUSPEND_COMMS_ACK:
        {
            u32 response = 0;
            _packetBuffer.Read<u32>(response);
            break;
        }
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
        {
            if (account == 0)
            {
                HandleAuthSession();
            }
            break;
        }
        case Common::Opcode::CMSG_REDIRECT_CLIENT_PROOF:
        {
            if (account == 0)
            {
                HandleContinueAuthSession();
            }
            break;
        }
        case Common::Opcode::CMSG_READY_FOR_ACCOUNT_DATA_TIMES:
        {
            /* Packet Structure */
            // UInt32:  Server Time (time(nullptr))
            // UInt8:   Unknown Byte Value
            // UInt32:  Mask for the account data fields

            Common::ByteBuffer accountDataTimes;

            u32 mask = 0x15;
            accountDataTimes.Write<u32>(static_cast<u32>(time(nullptr)));
            accountDataTimes.Write<u8>(1); // bitmask blocks count
            accountDataTimes.Write<u32>(mask); // PER_CHARACTER_CACHE_MASK

            for (u32 i = 0; i < 8; ++i)
            {
                if (mask & (1 << i))
                    accountDataTimes.Write<u32>(0);
            }

            SendPacket(accountDataTimes, Common::Opcode::SMSG_ACCOUNT_DATA_TIMES);
            break;
        }
        case Common::Opcode::CMSG_UPDATE_ACCOUNT_DATA:
        {
            u32 type, timestamp, decompressedSize;
            _packetBuffer.Read(&type, 4);
            _packetBuffer.Read(&timestamp, 4);
            _packetBuffer.Read(&decompressedSize, 4);

            if (type > 8)
            {
                std::cout << "Bad Type." << std::endl;
                break;
            }

            Common::ByteBuffer updateAccountDataComplete(9 + 4 + 4);
            updateAccountDataComplete.Write<u32>(type);
            updateAccountDataComplete.Write<u32>(0);

            SendPacket(updateAccountDataComplete, Common::Opcode::SMSG_UPDATE_ACCOUNT_DATA_COMPLETE);
            break;
        }
        case Common::Opcode::CMSG_REALM_SPLIT:
        {
            std::string split_date = "01/01/01";
            u32 unk = 0;
            _packetBuffer.Read<u32>(unk);

            Common::ByteBuffer realmSplit;
            realmSplit.Write<u32>(unk);
            realmSplit.Write<u32>(0x0); // split states: 0x0 realm normal, 0x1 realm split, 0x2 realm split pending
            realmSplit.WriteString(split_date);

            SendPacket(realmSplit, Common::Opcode::SMSG_REALM_SPLIT);
            break;
        }
        case Common::Opcode::CMSG_CHAR_ENUM:
        {
            PreparedStatement stmt("SELECT characters.guid, characters.name, characters.race, characters.class, characters.gender, character_visual_data.skin, character_visual_data.face, character_visual_data.hair_style, character_visual_data.hair_color, character_visual_data.facial_style, characters.level, characters.zoneId, characters.mapId, characters.coordinate_x, characters.coordinate_y, characters.coordinate_z FROM characters INNER JOIN character_visual_data ON characters.guid=character_visual_data.guid WHERE characters.account={u};");
            stmt.Bind(account);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this](amy::result_set & results, DatabaseConnector & connector)
                {
                    Common::ByteBuffer charEnum;

                    // Number of characters
                    charEnum.Write<u8>(static_cast<u8>(results.affected_rows()));

                    /* Template for loading a character */
                    for (auto& row : results)
                    {
                        charEnum.Write<u64>(row[0].GetU64()); // Guid
                        charEnum.WriteString(row[1].GetString()); // Name
                        charEnum.Write<u8>(row[2].GetU8()); // Race
                        charEnum.Write<u8>(row[3].GetU8()); // Class
                        charEnum.Write<u8>(row[4].GetU8()); // Gender

                        charEnum.Write<u8>(row[5].GetU8()); // Skin
                        charEnum.Write<u8>(row[6].GetU8()); // Face
                        charEnum.Write<u8>(row[7].GetU8()); // Hairstyle
                        charEnum.Write<u8>(row[8].GetU8()); // Haircolor
                        charEnum.Write<u8>(row[9].GetU8()); // Facialstyle

                        charEnum.Write<u8>(row[10].GetU8()); // Level
                        charEnum.Write<u32>(row[11].GetU16()); // Zone Id
                        charEnum.Write<u32>(row[12].GetU16()); // Map Id

                        charEnum.Write<f32>(row[13].GetF32()); // X
                        charEnum.Write<f32>(row[14].GetF32()); // Y
                        charEnum.Write<f32>(row[15].GetF32()); // Z

                        charEnum.Write<u32>(0); // Guild Id

                        charEnum.Write<u32>(0); // Character Flags
                        charEnum.Write<u32>(0); // characterCustomize Flag

                        charEnum.Write<u8>(1); // First Login (Here we should probably do a playerTime check to determin if its the player's first login)

                        charEnum.Write<u32>(0); // Pet Display Id (Lich King: 22234)
                        charEnum.Write<u32>(0);  // Pet Level
                        charEnum.Write<u32>(0);  // Pet Family

                        u32 equipmentDataNull = 0;
                        for (i32 i = 0; i < 23; ++i)
                        {
                            charEnum.Write<u32>(equipmentDataNull);
                            charEnum.Write<u8>(0);
                            charEnum.Write<u32>(equipmentDataNull);
                        }
                    }

                    SendPacket(charEnum, Common::Opcode::SMSG_CHAR_ENUM);
                });
            break;
        }
        case Common::Opcode::CMSG_CHAR_CREATE:
        {
            cCharacterCreateData* createData = new cCharacterCreateData();
            createData->Read(_packetBuffer);

            PreparedStatement stmt("SELECT name FROM characters WHERE name={s};");
            stmt.Bind(createData->charName);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, createData](amy::result_set& results, DatabaseConnector& connector)
                {
                    Common::ByteBuffer characterCreateResult;

                    if (results.affected_rows() > 0)
                    {
                        characterCreateResult.Write<u8>(CHAR_CREATE_NAME_IN_USE);
                        SendPacket(characterCreateResult, Common::Opcode::SMSG_CHAR_CREATE);
                        delete createData;
                        return;
                    }

                    /* Convert name to proper format */
                    std::transform(createData->charName.begin(), createData->charName.end(), createData->charName.begin(), ::tolower);
                    createData->charName[0] = std::toupper(createData->charName[0]);

                    CharacterUtils::SpawnPosition spawnPosition;
                    if (!CharacterUtils::BuildGetDefaultSpawn(_cache.GetDefaultSpawnStorageData(), createData->charRace, createData->charClass, spawnPosition))
                    {
                        characterCreateResult.Write<u8>(CHAR_CREATE_DISABLED);
                        SendPacket(characterCreateResult, Common::Opcode::SMSG_CHAR_CREATE);
                        delete createData;
                        return;
                    }

                    PreparedStatement characterBaseData("INSERT INTO characters(account, name, race, gender, class, mapId, zoneId, coordinate_x, coordinate_y, coordinate_z, orientation) VALUES({u}, {s}, {u}, {u}, {u}, {u}, {u}, {f}, {f}, {f}, {f});");
                    characterBaseData.Bind(account);
                    characterBaseData.Bind(createData->charName);
                    characterBaseData.Bind(createData->charRace);
                    characterBaseData.Bind(createData->charGender);
                    characterBaseData.Bind(createData->charClass);
                    characterBaseData.Bind(spawnPosition.mapId);
                    characterBaseData.Bind(spawnPosition.zoneId);
                    characterBaseData.Bind(spawnPosition.coordinate_x);
                    characterBaseData.Bind(spawnPosition.coordinate_y);
                    characterBaseData.Bind(spawnPosition.coordinate_z);
                    characterBaseData.Bind(spawnPosition.orientation);
                    connector.Execute(characterBaseData);

                    // This needs to be non-async as we rely on LAST_INSERT_ID() to retrieve the character's guid
                    amy::result_set guidResult;
                    connector.Query("SELECT LAST_INSERT_ID();", guidResult);
                    u64 characterGuid = guidResult[0][0].as<amy::sql_bigint_unsigned>();

                    PreparedStatement characterVisualData("INSERT INTO character_visual_data(guid, skin, face, facial_style, hair_style, hair_color) VALUES({u}, {u}, {u}, {u}, {u}, {u});");
                    characterVisualData.Bind(characterGuid);
                    characterVisualData.Bind(createData->charSkin);
                    characterVisualData.Bind(createData->charFace);
                    characterVisualData.Bind(createData->charFacialStyle);
                    characterVisualData.Bind(createData->charHairStyle);
                    characterVisualData.Bind(createData->charHairColor);
                    connector.ExecuteAsync(characterVisualData);

                    // Baseline Skills
                    std::string skillSql;
                    if (CharacterUtils::BuildDefaultSkillSQL(_cache.GetDefaultSkillStorageData(), characterGuid, createData->charRace, createData->charClass, skillSql))
                    {
                        connector.ExecuteAsync(skillSql);
                    }

                    // Baseline Spells
                    std::string spellSql;
                    if (CharacterUtils::BuildDefaultSpellSQL(_cache.GetDefaultSpellStorageData(), characterGuid, createData->charRace, createData->charClass, spellSql))
                    {
                        connector.ExecuteAsync(spellSql);
                    }

                    DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, [this](std::shared_ptr<DatabaseConnector> & connector)
                    {
                            u32 realmId = 1;
                        PreparedStatement realmCharacterCount("INSERT INTO realm_characters(account, realmid, characters) VALUES({u}, {u}, 1) ON DUPLICATE KEY UPDATE characters = characters + 1;");
                        realmCharacterCount.Bind(account);
                        realmCharacterCount.Bind(realmId); // Realm Id
                        connector->Execute(realmCharacterCount);
                    });

                    characterCreateResult.Write<u8>(CHAR_CREATE_SUCCESS);
                    SendPacket(characterCreateResult, Common::Opcode::SMSG_CHAR_CREATE);

                    delete createData;
                });

            break;
        }
        case Common::Opcode::CMSG_CHAR_DELETE:
        {
            u64 guid = 0;
            _packetBuffer.Read<u64>(guid);

            PreparedStatement stmt("SELECT account FROM characters WHERE guid={u};");
            stmt.Bind(guid);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, guid](amy::result_set& results, DatabaseConnector& connector)
                {
                    Common::ByteBuffer characterDeleteResult;

                    // Char doesn't exist
                    if (results.affected_rows() == 0)
                    {
                        characterDeleteResult.Write<u8>(CHAR_DELETE_FAILED);
                        SendPacket(characterDeleteResult, Common::Opcode::SMSG_CHAR_DELETE);
                        return;
                    }

                    // Prevent deleting other player's characters
                    u64 characterAccountGuid = results[0][0].as<amy::sql_bigint_unsigned>();
                    if (account != characterAccountGuid)
                    {
                        characterDeleteResult.Write<u8>(CHAR_DELETE_FAILED);
                        SendPacket(characterDeleteResult, Common::Opcode::SMSG_CHAR_DELETE);
                        return;
                    }

                    PreparedStatement characterBaseData("DELETE FROM characters WHERE guid={u};");
                    characterBaseData.Bind(guid);

                    PreparedStatement characterVisualData("DELETE FROM character_visual_data WHERE guid={u};");
                    characterVisualData.Bind(guid);

                    PreparedStatement charcaterSkillStorage("DELETE FROM character_skill_storage WHERE guid={u};");
                    charcaterSkillStorage.Bind(guid);

                    PreparedStatement charcaterSpellStorage("DELETE FROM character_spell_storage WHERE guid={u};");
                    charcaterSpellStorage.Bind(guid);

                    connector.Execute(characterBaseData);
                    connector.Execute(characterVisualData);
                    connector.Execute(charcaterSkillStorage);
                    connector.Execute(charcaterSpellStorage);

                    DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, [this](std::shared_ptr<DatabaseConnector> & connector)
                        {
                            u32 realmId = 1;
                            PreparedStatement realmCharacterCount("UPDATE realm_characters SET characters=characters-1 WHERE account={u} and realmid={u};");
                            realmCharacterCount.Bind(account);
                            realmCharacterCount.Bind(realmId);
                            connector->Execute(realmCharacterCount);
                        });

                    characterDeleteResult.Write<u8>(CHAR_DELETE_SUCCESS);
                    SendPacket(characterDeleteResult, Common::Opcode::SMSG_CHAR_DELETE);
                });
            break;
        }
        default:
            break;
    }

    return true;
}

void RealmConnection::SendPacket(Common::ByteBuffer& packet, Common::Opcode opcode)
{
    i32 packetSize = packet.GetActualSize() + 5;
    Common::ByteBuffer buffer(packetSize);
    buffer.Resize(packetSize);

    Common::ServerPacketHeader header(packet.size() + 2, opcode);
    _streamCrypto.Encrypt(header.headerArray, header.GetLength());

    buffer.Write(header.headerArray, header.GetLength());
    buffer.Write(packet.data(), packet.size());
    Send(buffer);
}


void RealmConnection::HandleContinueAuthSession()
{
    std::string username = "";
    u64 dosResponse = 0;
    u8 digest[SHA_DIGEST_LENGTH];

    _packetBuffer.Read(username);
    _packetBuffer.Read<u64>(dosResponse);
    _packetBuffer.Read(&digest, 20);

    PreparedStatement stmt("SELECT guid, sessionKey FROM accounts WHERE username={s};");
    stmt.Bind(username);
    DatabaseConnector::QueryAsync(DATABASE_TYPE::AUTHSERVER, stmt, [this, username, digest](amy::result_set & results, DatabaseConnector & connector)
    {
        // Make sure the account exist.
        if (results.affected_rows() != 1)
        {
            Close(asio::error::interrupted);
            return;
        }

        // We need to try to use the session key that we have, if we don't the client won't be able to read the auth response error.
        sessionKey->Hex2BN(results[0][1].as<amy::sql_varchar>().c_str());

        SHA1Hasher sha;
        //u32 t = 0;
        sha.UpdateHash(username);
        sha.UpdateHashForBn(1, sessionKey);
        sha.UpdateHash((u8*)&_seed, 4);
        sha.Finish();

        if (memcmp(sha.GetData(), digest, SHA_DIGEST_LENGTH) != 0)
        {
            Close(asio::error::interrupted);
            return;
        }


        _streamCrypto.SetupServer(sessionKey);
        account = results[0][0].as<amy::sql_int_unsigned>();

        Common::ByteBuffer empty;
        SendPacket(empty, Common::Opcode::SMSG_RESUME_COMMS);


        Common::ByteBuffer accountDataTimes;
        u32 mask = 0x15;
        accountDataTimes.Write<u32>(static_cast<u32>(time(nullptr)));
        accountDataTimes.Write<u8>(1); // bitmask blocks count
        accountDataTimes.Write<u32>(mask); // PER_CHARACTER_CACHE_MASK

        for (u32 i = 0; i < 8; ++i)
        {
            if (mask & (1 << i))
                accountDataTimes.Write<u32>(0);
        }

        SendPacket(accountDataTimes, Common::Opcode::SMSG_ACCOUNT_DATA_TIMES);

        std::string split_date = "01/01/01";
        u32 unk = 0;

        Common::ByteBuffer realmSplit;
        realmSplit.Write<u32>(unk);
        realmSplit.Write<u32>(0x0); // split states: 0x0 realm normal, 0x1 realm split, 0x2 realm split pending
        realmSplit.WriteString(split_date);

        SendPacket(realmSplit, Common::Opcode::SMSG_REALM_SPLIT);

        PreparedStatement stmt("SELECT characters.guid, characters.name, characters.race, characters.class, characters.gender, character_visual_data.skin, character_visual_data.face, character_visual_data.hair_style, character_visual_data.hair_color, character_visual_data.facial_style, characters.level, characters.zoneId, characters.mapId, characters.coordinate_x, characters.coordinate_y, characters.coordinate_z FROM characters INNER JOIN character_visual_data ON characters.guid=character_visual_data.guid WHERE characters.account={u};");
        stmt.Bind(account);
        DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this](amy::result_set & results, DatabaseConnector & connector)
            {
                Common::ByteBuffer charEnum;

                // Number of characters
                charEnum.Write<u8>(static_cast<u8>(results.affected_rows()));

                /* Template for loading a character */
                for (auto& row : results)
                {
                    charEnum.Write<u64>(row[0].GetU64()); // Guid
                    charEnum.WriteString(row[1].GetString()); // Name
                    charEnum.Write<u8>(row[2].GetU8()); // Race
                    charEnum.Write<u8>(row[3].GetU8()); // Class
                    charEnum.Write<u8>(row[4].GetU8()); // Gender

                    charEnum.Write<u8>(row[5].GetU8()); // Skin
                    charEnum.Write<u8>(row[6].GetU8()); // Face
                    charEnum.Write<u8>(row[7].GetU8()); // Hairstyle
                    charEnum.Write<u8>(row[8].GetU8()); // Haircolor
                    charEnum.Write<u8>(row[9].GetU8()); // Facialstyle

                    charEnum.Write<u8>(row[10].GetU8()); // Level
                    charEnum.Write<u32>(row[11].GetU16()); // Zone Id
                    charEnum.Write<u32>(row[12].GetU16()); // Map Id

                    charEnum.Write<f32>(row[13].GetF32()); // X
                    charEnum.Write<f32>(row[14].GetF32()); // Y
                    charEnum.Write<f32>(row[15].GetF32()); // Z

                    charEnum.Write<u32>(0); // Guild Id

                    charEnum.Write<u32>(0); // Character Flags
                    charEnum.Write<u32>(0); // characterCustomize Flag

                    charEnum.Write<u8>(1); // First Login (Here we should probably do a playerTime check to determin if its the player's first login)

                    charEnum.Write<u32>(0); // Pet Display Id (Lich King: 22234)
                    charEnum.Write<u32>(0);  // Pet Level
                    charEnum.Write<u32>(0);  // Pet Family

                    u32 equipmentDataNull = 0;
                    for (i32 i = 0; i < 23; ++i)
                    {
                        charEnum.Write<u32>(equipmentDataNull);
                        charEnum.Write<u8>(0);
                        charEnum.Write<u32>(equipmentDataNull);
                    }
                }

                SendPacket(charEnum, Common::Opcode::SMSG_CHAR_ENUM);
            });
    });
}

void RealmConnection::HandleAuthSession()
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
            u32 pos = static_cast<u32>(AddonInfo._readPos);

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

        _streamCrypto.SetupServer(sessionKey);
        account = results[0][0].GetU32();

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
        for (i32 i = 0; i < 8; i++)
            tutorialFlags.Write<u32>(0xFF);

        SendPacket(tutorialFlags, Common::Opcode::SMSG_TUTORIAL_FLAGS);

		/* SMSG_REDIRECT_CLIENT 
        Common::ByteBuffer redirectClient;
		i32 ip = 16777343;
		i16 port = 8001;

		// 127.0.0.1/1.0.0.127
		// 2130706433/16777343(https://www.browserling.com/tools/ip-to-dec)
		redirectClient.Write<i32>(ip);
		redirectClient.Write<i16>(port);
		redirectClient.Write<i32>(1); // unk
#pragma warning(push)
#pragma warning(disable: 4312)
		HMACH hmac(40, sessionKey->BN2BinArray(20).get());
		hmac.UpdateHash((u8*)& ip, 4);
		hmac.UpdateHash((u8*)& port, 2);
		hmac.Finish();
		redirectClient.Append(hmac.GetData(), 20);
#pragma warning(pop)
		SendPacket(redirectClient, Common::Opcode::SMSG_REDIRECT_CLIENT);*/
    });
}