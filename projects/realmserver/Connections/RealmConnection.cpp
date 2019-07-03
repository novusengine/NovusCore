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
#include <Networking/DataStore.h>
#include <Cryptography/HMAC.h>
#include <Cryptography/BigNumber.h>
#include <Database/DatabaseConnector.h>

#include "../Utils/CharacterUtils.h"
#include "../DatabaseCache/AuthDatabaseCache.h"

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

    void AddTo(DataStore& buffer)
    {
        buffer.PutBytes(reinterpret_cast<u8*>(this), sizeof(sAuthChallenge));
    }

    void PutBytes(u8* dest, const u8* src, size_t size)
    {
        std::memcpy(dest, src, size);
    }
};
#pragma pack(pop)

bool RealmConnection::Start()
{
    DataStore authPacket;
    BigNumber seed1, seed2;
    sAuthChallenge challenge;
    seed1.Rand(16 * 8);
    seed2.Rand(16 * 8);

    challenge.unk = 1;
    challenge.authSeed = _seed;
    challenge.PutBytes(challenge.seed1, seed1.BN2BinArray(16).get(), 16);
    challenge.PutBytes(challenge.seed2, seed2.BN2BinArray(16).get(), 16);
    challenge.AddTo(authPacket);

    if (_resumeConnection)
    {
        DataStore resumeComms;
        SendPacket(resumeComms, Opcode::SMSG_RESUME_COMMS);
    }
    SendPacket(authPacket, Opcode::SMSG_AUTH_CHALLENGE);

    AsyncRead();
    return true;
}

void RealmConnection::HandleRead()
{
    DataStore& buffer = GetReceiveBuffer();
    while (buffer.GetActiveSize())
    {
        // Check if we should read header
        if (!_headerBuffer.IsFull())
        {
            size_t headerSize = std::min(buffer.GetActiveSize(), _headerBuffer.GetRemainingSpace());
            _headerBuffer.PutBytes(buffer.GetReadPointer(), headerSize);

            // Wait for full header
            if (!_headerBuffer.IsFull())
                break;

            // Handle newly received header
            if (!HandleNewHeader())
            {
                Close(asio::error::shut_down);
                return;
            }

            buffer.ReadData += headerSize;
        }

        if (!_packetBuffer.IsFull() || _packetBuffer.Size == 0)
        {
            if (_packetBuffer.Size != 0)
            {
                u32 packetSize = std::min(buffer.GetActiveSize(), _packetBuffer.GetRemainingSpace());
                if (packetSize != 0)
                {
                    _packetBuffer.PutBytes(buffer.GetReadPointer(), packetSize);
                    buffer.ReadData += packetSize;
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

    printf("Received Opcode: %u\n", opcode);
    switch (opcode)
    {
        case Opcode::CMSG_PLAYER_LOGIN:
        {
            DataStore redirectClient;
            i32 ip = 16777343;
            i16 port = 9000;

            // 127.0.0.1/1.0.0.127
            // 2130706433/16777343(https://www.browserling.com/tools/ip-to-dec)
            redirectClient.PutI32(ip);
            redirectClient.PutI16(port);
            redirectClient.PutI32(0); // unk
#pragma warning(push)
#pragma warning(disable: 4312)
            HMACH hmac(40, sessionKey->BN2BinArray(20).get());
            hmac.UpdateHash((u8*)&ip, sizeof(i32));
            hmac.UpdateHash((u8*)&port, sizeof(i16));
            hmac.Finish();
            redirectClient.PutBytes(hmac.GetData(), 20);
#pragma warning(pop)
            SendPacket(redirectClient, Opcode::SMSG_REDIRECT_CLIENT);

            u64 characterGuid = 0;
            _packetBuffer.GetU64(characterGuid);

            DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, [&, characterGuid](std::shared_ptr<DatabaseConnector> & connector)
            {
                PreparedStatement stmt("UPDATE characters SET online=1, lastLogin={u} WHERE guid={u};");
                stmt.Bind(static_cast<u32>(time(nullptr)));
                stmt.Bind(characterGuid);
                connector->Execute(stmt);
                
                /* I'm am not 100% sure where this fits into the picture yet, but I'm sure it has a purpose
                DataStore suspendComms;
                suspendComms.PutU32(1);
                SendPacket(suspendComms, Opcode::SMSG_SUSPEND_COMMS);*/
                
            });
            break;
        }
        case Opcode::CMSG_CONNECT_TO_FAILED:
        {
            DataStore loginFailed;
            loginFailed.PutU8(ENTER_FAILED_WORLDSERVER_DOWN);
            SendPacket(loginFailed, Opcode::SMSG_CHARACTER_LOGIN_FAILED);
            break;
        }
        case Opcode::CMSG_SUSPEND_COMMS_ACK:
        {
            u32 response = 0;
            _packetBuffer.GetU32(response);
            break;
        }
        case Opcode::CMSG_PING:
        {
            DataStore pong;
            pong.PutU32(0);
            SendPacket(pong, Opcode::SMSG_PONG);
            break;
        }
        case Opcode::CMSG_KEEP_ALIVE:
            break;
        case Opcode::CMSG_AUTH_SESSION:
        {
            if (account == 0)
            {
                HandleAuthSession();
            }
            break;
        }
        case Opcode::CMSG_READY_FOR_ACCOUNT_DATA_TIMES:
        {
            /* Packet Structure */
            // UInt32:  Server Time (time(nullptr))
            // UInt8:   Unknown Byte Value
            // UInt32:  Mask for the account data fields

            DataStore accountDataTimes(nullptr, 9 + (4 * 8));

            u32 mask = 0x15;
            accountDataTimes.PutU32(static_cast<u32>(time(nullptr)));
            accountDataTimes.PutU8(1); // bitmask blocks count
            accountDataTimes.PutU32(mask);

            for (u32 i = 0; i < 8; ++i)
            {
                if (mask & (1 << i))
                {
                    AccountData accountData;
                    if (_authCache.GetAccountData(account, i, accountData))
                    {
                        accountDataTimes.PutU32(accountData.timestamp);
                    }
                }
            }

            SendPacket(accountDataTimes, Opcode::SMSG_ACCOUNT_DATA_TIMES);
            break;
        }
        case Opcode::CMSG_UPDATE_ACCOUNT_DATA:
        {
            u32 type, timestamp, decompressedSize;
            _packetBuffer.GetU32(type);
            _packetBuffer.GetU32(timestamp);
            _packetBuffer.GetU32(decompressedSize);

            if (type > 8)
            {
                break;
            }
            
            bool accountDataUpdate = ((1 << type) & 0x15);

            // Clear Data
            if (decompressedSize == 0)
            { 
                if (accountDataUpdate)
                {
                    AccountData accountData;
                    if (_authCache.GetAccountData(account, type, accountData))
                    {
                        accountData.timestamp = 0;
                        accountData.data = "";
                        accountData.UpdateCache();
                    }
                }
                else
                {
                    CharacterData characterData;
                    if (_charCache.GetCharacterData(characterGuid, type, characterData))
                    {
                        characterData.timestamp = 0;
                        characterData.data = "";
                        characterData.UpdateCache();
                    }
                }
            }
            else
            {
                if (decompressedSize > 0xFFFF)
                {
                    break;
                }

                DataStore DataInfo(nullptr, _packetBuffer.Size - _packetBuffer.ReadData);
                DataInfo.PutBytes(_packetBuffer.GetInternalData() + _packetBuffer.ReadData, DataInfo.Size);

                uLongf uSize = decompressedSize;
                u32 pos = static_cast<u32>(DataInfo.ReadData);

                DataStore dataInfo(nullptr, decompressedSize);
                if (uncompress(dataInfo.GetInternalData(), &uSize, DataInfo.GetInternalData() + pos, static_cast<uLong>(DataInfo.Size - pos)) != Z_OK)
                {
                    break;
                }

                std::string finalData = "";
                dataInfo.GetString(finalData);

                if (accountDataUpdate)
                {
                    AccountData accountData;
                    if (_authCache.GetAccountData(account, type, accountData))
                    {
                        accountData.timestamp = timestamp;
                        accountData.data = finalData;
                        accountData.UpdateCache();
                    }
                }
                else
                {
                    CharacterData characterData;
                    if (_charCache.GetCharacterData(characterGuid, type, characterData))
                    {
                        characterData.timestamp = timestamp;
                        characterData.data = finalData;
                        characterData.UpdateCache();
                    }
                }
            }

            DataStore updateAccountDataComplete;
            updateAccountDataComplete.PutU32(type);
            updateAccountDataComplete.PutU32(0);

            SendPacket(updateAccountDataComplete, Opcode::SMSG_UPDATE_ACCOUNT_DATA_COMPLETE);
            break;
        }
        case Opcode::CMSG_REALM_SPLIT:
        {
            std::string split_date = "01/01/01";
            u32 unk = 0;
            _packetBuffer.GetU32(unk);

            DataStore realmSplit;
            realmSplit.PutU32(unk);
            realmSplit.PutU32(0x0); // split states: 0x0 realm normal, 0x1 realm split, 0x2 realm split pending
            realmSplit.PutString(split_date);

            SendPacket(realmSplit, Opcode::SMSG_REALM_SPLIT);
            break;
        }
        case Opcode::CMSG_CHAR_ENUM:
        {
            PreparedStatement stmt("SELECT characters.guid, characters.name, characters.race, characters.class, characters.gender, character_visual_data.skin, character_visual_data.face, character_visual_data.hair_style, character_visual_data.hair_color, character_visual_data.facial_style, characters.level, characters.zoneId, characters.mapId, characters.coordinate_x, characters.coordinate_y, characters.coordinate_z FROM characters INNER JOIN character_visual_data ON characters.guid=character_visual_data.guid WHERE characters.account={u};");
            stmt.Bind(account);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this](amy::result_set & results, DatabaseConnector & connector)
                {
                    u8 characters = static_cast<u8>(results.affected_rows());
                    DataStore charEnum(nullptr, 1 + (characters * 285));

                    // Number of characters
                    charEnum.PutU8(characters);

                    /* Template for loading a character */
                    for (auto& row : results)
                    {
                        charEnum.PutU64(row[0].GetU64()); // Guid
                        charEnum.PutString(row[1].GetString()); // Name
                        charEnum.PutU8(row[2].GetU8()); // Race
                        charEnum.PutU8(row[3].GetU8()); // Class
                        charEnum.PutU8(row[4].GetU8()); // Gender

                        charEnum.PutU8(row[5].GetU8()); // Skin
                        charEnum.PutU8(row[6].GetU8()); // Face
                        charEnum.PutU8(row[7].GetU8()); // Hairstyle
                        charEnum.PutU8(row[8].GetU8()); // Haircolor
                        charEnum.PutU8(row[9].GetU8()); // Facialstyle

                        charEnum.PutU8(row[10].GetU8()); // Level
                        charEnum.PutU32(row[11].GetU16()); // Zone Id
                        charEnum.PutU32(row[12].GetU16()); // Map Id

                        charEnum.PutF32(row[13].GetF32()); // X
                        charEnum.PutF32(row[14].GetF32()); // Y
                        charEnum.PutF32(row[15].GetF32()); // Z

                        charEnum.PutU32(0); // Guild Id

                        charEnum.PutU32(0); // Character Flags
                        charEnum.PutU32(0); // characterCustomize Flag

                        charEnum.PutU8(1); // First Login (Here we should probably do a playerTime check to determin if its the player's first login)

                        charEnum.PutU32(0); // Pet Display Id (Lich King: 22234)
                        charEnum.PutU32(0);  // Pet Level
                        charEnum.PutU32(0);  // Pet Family

                        u32 equipmentDataNull = 0;
                        for (i32 i = 0; i < 23; ++i)
                        {
                            charEnum.PutU32(equipmentDataNull);
                            charEnum.PutU8(0);
                            charEnum.PutU32(equipmentDataNull);
                        }
                    }

                    SendPacket(charEnum, Opcode::SMSG_CHAR_ENUM);
                });
            break;
        }
        case Opcode::CMSG_CHAR_CREATE:
        {
            cCharacterCreateData* createData = new cCharacterCreateData();
            createData->Read(_packetBuffer);

            PreparedStatement stmt("SELECT name FROM characters WHERE name={s};");
            stmt.Bind(createData->charName);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, createData](amy::result_set& results, DatabaseConnector& connector)
                {
                    DataStore characterCreateResult;

                    if (results.affected_rows() > 0)
                    {
                        characterCreateResult.PutU8(CHAR_CREATE_NAME_IN_USE);
                        SendPacket(characterCreateResult, Opcode::SMSG_CHAR_CREATE);
                        delete createData;
                        return;
                    }

                    /* Convert name to proper format */
                    std::transform(createData->charName.begin(), createData->charName.end(), createData->charName.begin(), ::tolower);
                    createData->charName[0] = std::toupper(createData->charName[0]);

                    CharacterUtils::SpawnPosition spawnPosition;
                    if (!CharacterUtils::BuildGetDefaultSpawn(_charCache.GetDefaultSpawnStorageData(), createData->charRace, createData->charClass, spawnPosition))
                    {
                        characterCreateResult.PutU8(CHAR_CREATE_DISABLED);
                        SendPacket(characterCreateResult, Opcode::SMSG_CHAR_CREATE);
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
                    if (CharacterUtils::BuildDefaultSkillSQL(_charCache.GetDefaultSkillStorageData(), characterGuid, createData->charRace, createData->charClass, skillSql))
                    {
                        connector.ExecuteAsync(skillSql);
                    }

                    // Baseline Spells
                    std::string spellSql;
                    if (CharacterUtils::BuildDefaultSpellSQL(_charCache.GetDefaultSpellStorageData(), characterGuid, createData->charRace, createData->charClass, spellSql))
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

                    characterCreateResult.PutU8(CHAR_CREATE_SUCCESS);
                    SendPacket(characterCreateResult, Opcode::SMSG_CHAR_CREATE);

                    delete createData;
                });

            break;
        }
        case Opcode::CMSG_CHAR_DELETE:
        {
            u64 guid = 0;
            _packetBuffer.GetU64(guid);

            PreparedStatement stmt("SELECT account FROM characters WHERE guid={u};");
            stmt.Bind(guid);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, guid](amy::result_set& results, DatabaseConnector& connector)
                {
                    DataStore characterDeleteResult;

                    // Char doesn't exist
                    if (results.affected_rows() == 0)
                    {
                        characterDeleteResult.PutU8(CHAR_DELETE_FAILED);
                        SendPacket(characterDeleteResult, Opcode::SMSG_CHAR_DELETE);
                        return;
                    }

                    // Prevent deleting other player's characters
                    u64 characterAccountGuid = results[0][0].as<amy::sql_bigint_unsigned>();
                    if (account != characterAccountGuid)
                    {
                        characterDeleteResult.PutU8(CHAR_DELETE_FAILED);
                        SendPacket(characterDeleteResult, Opcode::SMSG_CHAR_DELETE);
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

                    characterDeleteResult.PutU8(CHAR_DELETE_SUCCESS);
                    SendPacket(characterDeleteResult, Opcode::SMSG_CHAR_DELETE);
                });
            break;
        }
        default:
            break;
    }

    return true;
}

void RealmConnection::SendPacket(DataStore& packet, Opcode opcode)
{
    ServerPacketHeader header(packet.GetActiveSize() + 2, opcode);
    u8 headerSize = header.GetLength();
    i32 packetSize = packet.GetActiveSize() + headerSize;

    _streamEncryption.Encrypt(header.data, headerSize);
    _sendBuffer.Size = packetSize;

    if (!_sendBuffer.PutBytes(header.data, headerSize))
        return;

    if (!_sendBuffer.IsFull())
    {
        if (!_sendBuffer.PutBytes(packet.GetInternalData(), packet.WrittenData))
            return;
    }

    Send(_sendBuffer);
    _sendBuffer.Reset();
}

void RealmConnection::HandleAuthSession()
{
    /* Read AuthSession Data */
    sessionData.Read(_packetBuffer);

    DataStore AddonInfo(nullptr, _packetBuffer.Size - _packetBuffer.ReadData);
    AddonInfo.PutBytes(_packetBuffer.GetInternalData() + _packetBuffer.ReadData, AddonInfo.Size);

    if (AddonInfo.ReadData + 4 <= AddonInfo.Size)
    {
        u32 size = 0;
        AddonInfo.GetU32(size);

        if (size > 0 && size < 0xFFFFF)
        {
            uLongf uSize = size;
            u32 pos = static_cast<u32>(AddonInfo.ReadData);

            DataStore addonInfo(nullptr, size);
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
    sha.UpdateHash((u8*)& t, 4);
    sha.UpdateHash((u8*)& sessionData.localChallenge, 4);
    sha.UpdateHash((u8*)& _seed, 4);
    sha.UpdateHashForBn(1, sessionKey);
    sha.Finish();

    if (memcmp(sha.GetData(), sessionData.digest, SHA_DIGEST_LENGTH) != 0)
    {
        Close(asio::error::interrupted);
        return;
    }

    _streamEncryption.Setup(sessionKey);
    account = results[0][0].GetU32();

    /* SMSG_AUTH_RESPONSE */
    DataStore authResponse;
    authResponse.PutU8(AUTH_OK);
    authResponse.PutU32(0);
    authResponse.PutU8(0);
    authResponse.PutU32(0);
    authResponse.PutU8(2); // Expansion
    SendPacket(authResponse, Opcode::SMSG_AUTH_RESPONSE);

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

    DataStore addonInfo;
    for (auto addon : addonMap)
    {
        addonInfo.PutU8(2); // State
        addonInfo.PutU8(1); // UsePublicKeyOrCRC

        // if (UsePublicKeyOrCRC)
        {
            u8 usepk = (addon.second != 1276933997);
            addonInfo.PutU8(usepk);

            if (usepk)
            {
                std::cout << "Addon Mismatch (" << addon.first << "," << addon.second << ")" << std::endl;
            }

            addonInfo.PutU32(0); // What does this mean?
        }

        addonInfo.PutU8(0); // Uses URL
    }

    addonInfo.PutU32(0); // Size of banned addon list
    SendPacket(addonInfo, Opcode::SMSG_ADDON_INFO);

    DataStore clientCache;
    clientCache.PutU32(0);
    SendPacket(clientCache, Opcode::SMSG_CLIENTCACHE_VERSION);

    // Tutorial Flags : REQUIRED
    DataStore tutorialFlags;
    for (i32 i = 0; i < 8; i++)
        tutorialFlags.PutU32(0xFF);

    SendPacket(tutorialFlags, Opcode::SMSG_TUTORIAL_FLAGS);

    if (_resumeConnection)
    {
        DataStore logoutRequest;
        SendPacket(logoutRequest, Opcode::SMSG_LOGOUT_COMPLETE);

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