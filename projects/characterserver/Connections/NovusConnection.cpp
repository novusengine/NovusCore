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
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/

#include "NovusConnection.h"
#include <Networking\ByteBuffer.h>
#include <Networking\Opcode\Opcode.h>
#include <Database\DatabaseConnector.h>

enum CharacterResponses
{    
    CHAR_CREATE_IN_PROGRESS                                = 46,
    CHAR_CREATE_SUCCESS                                    = 47,
    CHAR_CREATE_ERROR                                      = 48,
    CHAR_CREATE_FAILED                                     = 49,
    CHAR_CREATE_NAME_IN_USE                                = 50,
    CHAR_CREATE_DISABLED                                   = 51,
    CHAR_CREATE_PVP_TEAMS_VIOLATION                        = 52,
    CHAR_CREATE_SERVER_LIMIT                               = 53,
    CHAR_CREATE_ACCOUNT_LIMIT                              = 54,
    CHAR_CREATE_SERVER_QUEUE                               = 55,
    CHAR_CREATE_ONLY_EXISTING                              = 56,
    CHAR_CREATE_EXPANSION                                  = 57,
    CHAR_CREATE_EXPANSION_CLASS                            = 58,
    CHAR_CREATE_LEVEL_REQUIREMENT                          = 59,
    CHAR_CREATE_UNIQUE_CLASS_LIMIT                         = 60,
    CHAR_CREATE_CHARACTER_IN_GUILD                         = 61,
    CHAR_CREATE_RESTRICTED_RACECLASS                       = 62,
    CHAR_CREATE_CHARACTER_CHOOSE_RACE                      = 63,
    CHAR_CREATE_CHARACTER_ARENA_LEADER                     = 64,
    CHAR_CREATE_CHARACTER_DELETE_MAIL                      = 65,
    CHAR_CREATE_CHARACTER_SWAP_FACTION                     = 66,
    CHAR_CREATE_CHARACTER_RACE_ONLY                        = 67,
    CHAR_CREATE_CHARACTER_GOLD_LIMIT                       = 68,
    CHAR_CREATE_FORCE_LOGIN                                = 69,

    CHAR_DELETE_IN_PROGRESS                                = 70,
    CHAR_DELETE_SUCCESS                                    = 71,
    CHAR_DELETE_FAILED                                     = 72,
    CHAR_DELETE_FAILED_LOCKED_FOR_TRANSFER                 = 73,
    CHAR_DELETE_FAILED_GUILD_LEADER                        = 74,
    CHAR_DELETE_FAILED_ARENA_CAPTAIN                       = 75,
};

std::unordered_map<uint8_t, NovusMessageHandler> NovusConnection::InitMessageHandlers()
{
    std::unordered_map<uint8_t, NovusMessageHandler> messageHandlers;

    messageHandlers[NOVUS_CHALLENGE]    = { NOVUSSTATUS_CHALLENGE,    sizeof(sNovusChallenge),  &NovusConnection::HandleCommandChallenge };
    messageHandlers[NOVUS_PROOF]        = { NOVUSSTATUS_PROOF,        1,                        &NovusConnection::HandleCommandProof };
    messageHandlers[NOVUS_FOWARDPACKET] = { NOVUSSTATUS_AUTHED,       9,                        &NovusConnection::HandleCommandForwardPacket };

    return messageHandlers;
}
std::unordered_map<uint8_t, NovusMessageHandler> const MessageHandlers = NovusConnection::InitMessageHandlers();

bool NovusConnection::Start()
{
    try
    {
        _socket->connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(_address), _port));

        /* NODE_CHALLENGE */
        Common::ByteBuffer packet(5);
        packet.Write<uint8_t>(0);       // Command
        packet.Write<uint16_t>(335);    // Version
        packet.Write<uint16_t>(12340);  // Build

        AsyncRead();
        Send(packet);
        return true;
    }
    catch (asio::system_error error)
    {
        std::cout << "ERROR: " << error.what() << std::endl;
        return false;
    }
}

void NovusConnection::HandleRead()
{
    Common::ByteBuffer& buffer = GetByteBuffer();

    bool isDecrypted = false;
    while (buffer.GetActualSize())
    {
        // Decrypt data post CHALLENGE Status
        if (!isDecrypted && (_status == NOVUSSTATUS_PROOF || _status == NOVUSSTATUS_AUTHED))
        {
            _crypto->Decrypt(buffer.GetReadPointer(), buffer.GetActualSize());
            isDecrypted = true;
        }

        uint8_t command = buffer.GetDataPointer()[0];

        auto itr = MessageHandlers.find(command);
        if (itr == MessageHandlers.end())
        {
            std::cout << "Received HandleRead with no MessageHandler to respond." << std::endl;
            buffer.Clean();
            break;
        }

        if (_status != itr->second.status)
        {
            Close(asio::error::shut_down);
            return;
        }

        if (command == NOVUS_FOWARDPACKET)
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

                /* Read Header */
                NovusHeader* header = reinterpret_cast<NovusHeader*>(_headerBuffer.GetReadPointer());
                _packetBuffer.Resize(header->size);
                _packetBuffer.ResetPos();
            }

            // We have a header, now check the packet data
            if (_packetBuffer.GetSpaceLeft() > 0)
            {
                std::size_t packetSize = std::min(buffer.GetActualSize(), _packetBuffer.GetSpaceLeft());
                _packetBuffer.Write(buffer.GetReadPointer(), packetSize);
                buffer.ReadBytes(packetSize);

                if (_packetBuffer.GetSpaceLeft() > 0)
                {
                    // Wait until we have all of the packet data
                    assert(buffer.GetActualSize() == 0);
                    break;
                }
            }

            if (!HandleCommandForwardPacket())
            {
                Close(asio::error::shut_down);
                return;
            }
            _headerBuffer.ResetPos();
        }
        else
        {
            uint16_t size = uint16_t(itr->second.packetSize);
            if (buffer.GetActualSize() < size)
                break;

            if (!(*this.*itr->second.handler)())
            {
                Close(asio::error::shut_down);
                return;
            }

            buffer.ReadBytes(size);
        }
    }

    AsyncRead();
}

bool NovusConnection::HandleCommandChallenge()
{
    _status = NOVUSSTATUS_CLOSED;
    sNovusChallenge* novusChallenge = reinterpret_cast<sNovusChallenge*>(GetByteBuffer().GetReadPointer());

    _key->Bin2BN(novusChallenge->K, 32);
    _crypto->SetupClient(_key);

    /* Send fancy encrypted packet here */
    Common::ByteBuffer packet;
    packet.Write<uint8_t>(NOVUS_PROOF); // RELAY_PROOF
    _crypto->Encrypt(packet.GetReadPointer(), packet.GetActualSize());
    _status = NOVUSSTATUS_PROOF;

    Send(packet);
    return true;
}
bool NovusConnection::HandleCommandProof()
{
    _status = NOVUSSTATUS_AUTHED;

    return true;
}

bool NovusConnection::HandleCommandForwardPacket()
{
    NovusHeader* header = reinterpret_cast<NovusHeader*>(_headerBuffer.GetReadPointer());
    std::cout << "Received opcode: 0x" << std::hex << std::uppercase << header->opcode << std::endl;

    switch ((Common::Opcode)header->opcode)
    {
        case Common::Opcode::CMSG_READY_FOR_ACCOUNT_DATA_TIMES:
        {
            /* Packet Structure */
            // UInt32:  Server Time (time(nullptr))
            // UInt8:   Unknown Byte Value
            // UInt32:  Mask for the account data fields

            Common::ByteBuffer forwardPacket;
            NovusHeader packetHeader;
            packetHeader.command = NOVUS_FOWARDPACKET;
            packetHeader.account = header->account;
            packetHeader.opcode = Common::Opcode::SMSG_ACCOUNT_DATA_TIMES;
            packetHeader.size = 4 + 1 + 4;
            packetHeader.AddTo(forwardPacket);

            forwardPacket.Write<uint32_t>((uint32_t)time(nullptr));
            forwardPacket.Write<uint8_t>(1);
            forwardPacket.Write<uint32_t>(0x15);

            SendPacket(forwardPacket);
            break;
        }
        case Common::Opcode::CMSG_REALM_SPLIT:
        {
            Common::ByteBuffer forwardPacket;
            Common::ByteBuffer realmSplit;

            std::string split_date = "01/01/01";
            uint32_t unk = 0;
            _packetBuffer.Read<uint32_t>(unk);


            NovusHeader packetHeader;
            packetHeader.command = NOVUS_FOWARDPACKET;
            packetHeader.account = header->account;
            packetHeader.opcode = Common::Opcode::SMSG_REALM_SPLIT;

            realmSplit.Write<uint32_t>(unk);
            realmSplit.Write<uint32_t>(0x0); // split states: 0x0 realm normal, 0x1 realm split, 0x2 realm split pending
            realmSplit.WriteString(split_date);

            packetHeader.size = realmSplit.GetActualSize();
            packetHeader.AddTo(forwardPacket);
            forwardPacket.Append(realmSplit);

            SendPacket(forwardPacket);
            break;
        }
        case Common::Opcode::CMSG_CHAR_ENUM:
        {
            PreparedStatement stmt("SELECT characters.guid, characters.name, characters.race, characters.class, characters.gender, character_visual_data.skin, character_visual_data.face, character_visual_data.hair_style, character_visual_data.hair_color, character_visual_data.facial_style, characters.level, characters.zone_id, characters.map_id, characters.coordinate_x, characters.coordinate_y, characters.coordinate_z FROM characters INNER JOIN character_visual_data ON characters.guid=character_visual_data.guid WHERE characters.account={u};");
            stmt.Bind(header->account);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, header](amy::result_set& results, DatabaseConnector& connector)
            {
                Common::ByteBuffer forwardPacket;
                Common::ByteBuffer charEnum;

                NovusHeader packetHeader;
                packetHeader.command = NOVUS_FOWARDPACKET;
                packetHeader.account = header->account;
                packetHeader.opcode = Common::Opcode::SMSG_CHAR_ENUM;

                // Number of characters
                charEnum.Write<uint8_t>((uint8_t)results.affected_rows());

                /* Template for loading a character */
                for (auto& row : results)
                {
                    charEnum.Write<uint64_t>(row[0].as<amy::sql_int_unsigned>()); // Guid
                    charEnum.WriteString(row[1].as<amy::sql_varchar>()); // Name
                    charEnum.Write<uint8_t>(row[2].as<amy::sql_tinyint_unsigned>()); // Race
                    charEnum.Write<uint8_t>(row[3].as<amy::sql_tinyint_unsigned>()); // Class
                    charEnum.Write<uint8_t>(row[4].as<amy::sql_tinyint_unsigned>()); // Gender

                    charEnum.Write<uint8_t>(row[5].as<amy::sql_tinyint_unsigned>()); // Skin
                    charEnum.Write<uint8_t>(row[6].as<amy::sql_tinyint_unsigned>()); // Face
                    charEnum.Write<uint8_t>(row[7].as<amy::sql_tinyint_unsigned>()); // Hairstyle
                    charEnum.Write<uint8_t>(row[8].as<amy::sql_tinyint_unsigned>()); // Haircolor
                    charEnum.Write<uint8_t>(row[9].as<amy::sql_tinyint_unsigned>()); // Facialstyle

                    charEnum.Write<uint8_t>(row[10].as<amy::sql_tinyint_unsigned>()); // Level
                    charEnum.Write<uint32_t>(row[11].as<amy::sql_tinyint_unsigned>()); // Zone Id
                    charEnum.Write<uint32_t>(row[12].as<amy::sql_tinyint_unsigned>()); // Map Id

                    charEnum.Write<float>(row[13].as<amy::sql_float>()); // X
                    charEnum.Write<float>(row[14].as<amy::sql_float>()); // Y
                    charEnum.Write<float>(row[15].as<amy::sql_float>()); // Z

                    charEnum.Write<uint32_t>(0); // Guild Id

                    charEnum.Write<uint32_t>(0); // Character Flags
                    charEnum.Write<uint32_t>(0); // characterCustomize Flag

                    charEnum.Write<uint8_t>(1); // First Login (Here we should probably do a playerTime check to determin if its the player's first login)

                    charEnum.Write<uint32_t>(22234); // Pet Display Id
                    charEnum.Write<uint32_t>(5);  // Pet Level
                    charEnum.Write<uint32_t>(1);  // Pet Family

                    uint32_t equipmentDataNull = 0;
                    for (int i = 0; i < 23; ++i)
                    {
                        charEnum.Write<uint32_t>(equipmentDataNull);
                        charEnum.Write<uint8_t>(0);
                        charEnum.Write<uint32_t>(equipmentDataNull);
                    }
                }

                packetHeader.size = charEnum.GetActualSize();
                packetHeader.AddTo(forwardPacket);
                forwardPacket.Append(charEnum);

                SendPacket(forwardPacket);
            });
            break;
        }
        case Common::Opcode::CMSG_CHAR_CREATE:
        {
            cCharacterCreateData* createData = new cCharacterCreateData();
            createData->Read(_packetBuffer);

            PreparedStatement stmt("SELECT name FROM characters WHERE name={s};");
            stmt.Bind(createData->charName);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, header, createData](amy::result_set& results, DatabaseConnector& connector)
            {
                Common::ByteBuffer forwardPacket;

                NovusHeader packetHeader;
                packetHeader.command = NOVUS_FOWARDPACKET;
                packetHeader.account = header->account;
                packetHeader.opcode = Common::Opcode::SMSG_CHAR_CREATE;
                packetHeader.size = 1;
                packetHeader.AddTo(forwardPacket);

                if (results.affected_rows() > 0)
                {
                    forwardPacket.Write<uint8_t>(CHAR_CREATE_NAME_IN_USE);
                    SendPacket(forwardPacket);
                    delete createData;
                    return;
                }

                PreparedStatement characterBaseData("INSERT INTO characters(account, name, race, gender, class, map_id, zone_id, coordinate_x, coordinate_y, coordinate_z) VALUES({u}, {s}, {u}, {u}, {u}, {i}, {i}, {f}, {f}, {f});");
                characterBaseData.Bind(header->account);
                characterBaseData.Bind(createData->charName);
                characterBaseData.Bind(createData->charRace);
                characterBaseData.Bind(createData->charGender);
                characterBaseData.Bind(createData->charClass);
                characterBaseData.Bind(0);
                characterBaseData.Bind(12);
                characterBaseData.Bind(-8949.950195f);
                characterBaseData.Bind(-132.492996f);
                characterBaseData.Bind(83.531197f);

                PreparedStatement characterVisualData("INSERT INTO character_visual_data(guid, skin, face, facial_style, hair_style, hair_color) VALUES(LAST_INSERT_ID(), {u}, {u}, {u}, {u}, {u});");
                characterVisualData.Bind(createData->charSkin);
                characterVisualData.Bind(createData->charFace);
                characterVisualData.Bind(createData->charFacialStyle);
                characterVisualData.Bind(createData->charHairStyle);
                characterVisualData.Bind(createData->charHairColor);

                // This needs to be non-async as we rely on LAST_INSERT_ID() to retrieve the character's guid
                connector.Execute(characterBaseData);
                connector.Execute(characterVisualData);

                DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, [this, header](std::shared_ptr<DatabaseConnector>& connector)
                {
                    PreparedStatement realmCharacterCount("INSERT INTO realm_characters(account, realmid, characters) VALUES({u}, {u}, 1) ON DUPLICATE KEY UPDATE characters = characters + 1;");
                    realmCharacterCount.Bind(header->account);
                    realmCharacterCount.Bind(_realmId);
                    connector->Execute(realmCharacterCount);
                });

                forwardPacket.Write<uint8_t>(CHAR_CREATE_SUCCESS);
                SendPacket(forwardPacket);

                delete createData;
            });

            break;
        }
        case Common::Opcode::CMSG_CHAR_DELETE:
        {
            uint64_t guid = 0;
            _packetBuffer.Read<uint64_t>(guid);

            PreparedStatement stmt("SELECT account FROM characters WHERE guid={u};");
            stmt.Bind(guid);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, header, guid](amy::result_set& results, DatabaseConnector& connector)
            {
                Common::ByteBuffer forwardPacket;
                NovusHeader packetHeader;
                packetHeader.command = NOVUS_FOWARDPACKET;
                packetHeader.account = header->account;
                packetHeader.opcode = Common::Opcode::SMSG_CHAR_DELETE;
                packetHeader.size = 1;
                packetHeader.AddTo(forwardPacket);

                // Char doesn't exist
                if (results.affected_rows() == 0)
                {
                    forwardPacket.Write<uint8_t>(CHAR_DELETE_FAILED);
                    SendPacket(forwardPacket);
                    return;
                }

                // Prevent deleting other player's characters
                uint64_t characterAccountGuid = results[0][0].as<amy::sql_bigint_unsigned>();
                if (header->account != characterAccountGuid)
                {
                    forwardPacket.Write<uint8_t>(CHAR_DELETE_FAILED);
                    SendPacket(forwardPacket);
                    return;
                }

                PreparedStatement characterBaseData("DELETE FROM characters WHERE guid={u};");
                characterBaseData.Bind(guid);

                PreparedStatement characterVisualData("DELETE FROM character_visual_data WHERE guid={u};");
                characterVisualData.Bind(guid);

                connector.Execute(characterBaseData);
                connector.Execute(characterVisualData);

                DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, [this, header](std::shared_ptr<DatabaseConnector>& connector)
                {
                    PreparedStatement realmCharacterCount("UPDATE realm_characters SET characters=characters-1 WHERE account={u} and realmid={u};");
                    realmCharacterCount.Bind(header->account);
                    realmCharacterCount.Bind(_realmId);
                    connector->Execute(realmCharacterCount);
                });

                forwardPacket.Write<uint8_t>(CHAR_DELETE_SUCCESS); 
                SendPacket(forwardPacket);
            });
            break;
        }
        case Common::Opcode::CMSG_PLAYER_LOGIN:
        {
            NovusHeader packetHeader;
            packetHeader.command = NOVUS_FOWARDPACKET;
            packetHeader.account = header->account;

            uint64_t playerGuid = 0;
            _packetBuffer.Read<uint64_t>(playerGuid);


            /* SMSG_LOGIN_VERIFY_WORLD */
            Common::ByteBuffer verifyWorld;
            packetHeader.opcode = Common::Opcode::SMSG_LOGIN_VERIFY_WORLD;
            packetHeader.size = 4 + (4 * 4);
            packetHeader.AddTo(verifyWorld);

            verifyWorld.Write<uint32_t>(0); // Map (0 == Eastern Kingdom) & Elwynn Forest (Zone is 12) & Northshire (Area is 9)
            verifyWorld.Write<float>(-8949.950195f);
            verifyWorld.Write<float>(-132.492996f);
            verifyWorld.Write<float>(83.531197f);
            verifyWorld.Write<float>(0.0f);
            SendPacket(verifyWorld);


            /* SMSG_ACCOUNT_DATA_TIMES */
            Common::ByteBuffer accountDataForwardPacket;
            Common::ByteBuffer accountDataTimes;
            packetHeader.opcode = Common::Opcode::SMSG_ACCOUNT_DATA_TIMES;

            uint32_t mask = 0xEA;
            accountDataTimes.Write<uint32_t>((uint32_t)time(nullptr));
            accountDataTimes.Write<uint8_t>(1); // bitmask blocks count
            accountDataTimes.Write<uint32_t>(mask); // PER_CHARACTER_CACHE_MASK

            for (uint32_t i = 0; i < 8; ++i)
            {
                if (mask & (1 << i))
                    accountDataTimes.Write<uint32_t>(0);
            }

            packetHeader.size = (uint16_t)accountDataTimes.GetActualSize();
            packetHeader.AddTo(accountDataForwardPacket);
            accountDataForwardPacket.Append(accountDataTimes);
            SendPacket(accountDataForwardPacket);


            /* SMSG_FEATURE_SYSTEM_STATUS */
            Common::ByteBuffer featureSystemStatus;
            packetHeader.opcode = Common::Opcode::SMSG_FEATURE_SYSTEM_STATUS;
            packetHeader.size = 1 + 1;
            packetHeader.AddTo(featureSystemStatus);

            featureSystemStatus.Write<uint8_t>(2);
            featureSystemStatus.Write<uint8_t>(0);
            SendPacket(featureSystemStatus);


            /* SMSG_MOTD */
            Common::ByteBuffer motdForwardPacket;
            Common::ByteBuffer motd;
            packetHeader.opcode = Common::Opcode::SMSG_MOTD;
            packetHeader.AddTo(motd);

            motd.Write<uint32_t>(1);
            motd.WriteString("Welcome");

            packetHeader.size = (uint16_t)motd.GetActualSize();
            packetHeader.AddTo(motdForwardPacket);
            motdForwardPacket.Append(motd);
            SendPacket(motdForwardPacket);


            /* SMSG_LEARNED_DANCE_MOVES */
            Common::ByteBuffer learnedDanceMoves;
            packetHeader.opcode = Common::Opcode::SMSG_LEARNED_DANCE_MOVES;
            packetHeader.size = 4 + 4;
            packetHeader.AddTo(learnedDanceMoves);

            learnedDanceMoves.Write<uint32_t>(0);
            learnedDanceMoves.Write<uint32_t>(0);
            SendPacket(learnedDanceMoves);


            /* SMSG_ACTION_BUTTONS */
            Common::ByteBuffer actionButtons;
            packetHeader.opcode = Common::Opcode::SMSG_ACTION_BUTTONS;
            packetHeader.size = 1 + (4 * 144);
            packetHeader.AddTo(actionButtons);

            actionButtons.Write<uint8_t>(1);
            for (uint8_t button = 0; button < 144; ++button)
            {
                actionButtons.Write<uint32_t>(0);
            }
            SendPacket(actionButtons);


            /* SMSG_LOGIN_SETTIMESPEED */
            Common::ByteBuffer loginSetTimeSpeed;
            packetHeader.opcode = Common::Opcode::SMSG_LOGIN_SETTIMESPEED;
            packetHeader.size = 4 + 4 + 4;
            packetHeader.AddTo(loginSetTimeSpeed);

            tm lt;
            time_t const tmpServerTime = time(nullptr);
            localtime_s(&lt, &tmpServerTime);

            loginSetTimeSpeed.Write<uint32_t>(((lt.tm_year - 100) << 24 | lt.tm_mon << 20 | (lt.tm_mday - 1) << 14 | lt.tm_wday << 11 | lt.tm_hour << 6 | lt.tm_min));
            loginSetTimeSpeed.Write<float>(0.01666667f);
            loginSetTimeSpeed.Write<uint32_t>(0);
            SendPacket(loginSetTimeSpeed);

            break;
        }
        default:
            std::cout << "Could not handled opcode: 0x" << std::hex << std::uppercase << header->opcode << std::endl << std::endl;
            break;
    }

    return true;
}

void NovusConnection::SendPacket(Common::ByteBuffer& packet)
{
    _crypto->Encrypt(packet.GetReadPointer(), packet.GetActualSize());
    Send(packet);
}