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

std::unordered_map<uint8_t, NovusMessageHandler> NovusConnection::InitMessageHandlers()
{
    std::unordered_map<uint8_t, NovusMessageHandler> messageHandlers;

    messageHandlers[NOVUS_CHALLENGE]    =   { NOVUSSTATUS_CHALLENGE,    sizeof(sNovusChallenge),    &NovusConnection::HandleCommandChallenge        };
    messageHandlers[NOVUS_PROOF]        =   { NOVUSSTATUS_PROOF,        1,                          &NovusConnection::HandleCommandProof            };
    messageHandlers[NOVUS_FOWARDPACKET] =   { NOVUSSTATUS_AUTHED,       11,                         &NovusConnection::HandleCommandForwardPacket    };

    return messageHandlers;
}
std::unordered_map<uint8_t, NovusMessageHandler> const MessageHandlers = NovusConnection::InitMessageHandlers();

bool NovusConnection::Start()
{
    try
    {
        _socket->connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(_address), _port));

        AsyncRead();

        /* NODE_CHALLENGE */
        Common::ByteBuffer packet(6);

        packet.Write<uint8_t>(0); // Command
        packet.Write<uint8_t>(0); // Type
        uint16_t version = 335;
        packet.Append((uint8_t*)&version, sizeof(version)); // Version
        uint16_t build = 12340;
        packet.Append((uint8_t*)&build, sizeof(build)); // Build

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
    Common::ByteBuffer& byteBuffer = GetByteBuffer();
    while (byteBuffer.GetActualSize())
    {
        // Decrypt data post CHALLENGE Status
        if (_status == NOVUSSTATUS_PROOF || _status == NOVUSSTATUS_AUTHED)
        {
            _crypto->Decrypt(byteBuffer.GetReadPointer(), byteBuffer.GetActualSize());
        }

        uint8_t command = byteBuffer.GetDataPointer()[0];

        auto itr = MessageHandlers.find(command);
        if (itr == MessageHandlers.end())
        {
            std::cout << "Received HandleRead with no MessageHandler to respond." << std::endl;
            byteBuffer.Clean();
            break;
        }

        // Client attempted incorrect auth step
        if (_status != itr->second.status)
        {
            Close(asio::error::shut_down);
            return;
        }

        uint16_t size = uint16_t(itr->second.packetSize);
        if (byteBuffer.GetActualSize() < size)
            break;

        if (!(*this.*itr->second.handler)())
        {
            Close(asio::error::shut_down);
            return;
        }

        byteBuffer.ReadBytes(byteBuffer.GetActualSize());
    }

    AsyncRead();
}


bool NovusConnection::HandleCommandChallenge()
{
    std::cout << "Received RelayChallenge" << std::endl;
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
    std::cout << "Received RelayProof" << std::endl;
    _status = NOVUSSTATUS_AUTHED;

    return true;
}

bool NovusConnection::HandleCommandForwardPacket()
{
    Common::ByteBuffer& _packetBuffer = GetByteBuffer();
    _packetBuffer.ReadBytes(1); // Skip NovusCommand

    uint64_t accountGuid = 0;
    uint16_t clientOpcode = 0;
    _packetBuffer.Read(&accountGuid, 8);
    _packetBuffer.Read(&clientOpcode, 2);

    std::cout << "Received HandleCommandForwardPacket(" << accountGuid << "," << clientOpcode << "," << _packetBuffer.GetActualSize() << ")" << std::endl;


    switch ((Common::Opcode)clientOpcode)
    {
        case Common::Opcode::CMSG_READY_FOR_ACCOUNT_DATA_TIMES:
        {
            /* Packet Structure */
            // UInt32:  Server Time (time(nullptr))
            // UInt8:   Unknown Byte Value
            // UInt32:  Mask for the account data fields

            Common::ByteBuffer packetResponse;
            packetResponse.Write<uint8_t>(NOVUS_FOWARDPACKET);
            packetResponse.Write<uint64_t>(accountGuid);
            packetResponse.Write<uint16_t>(Common::Opcode::SMSG_ACCOUNT_DATA_TIMES);

            packetResponse.Write<uint32_t>((uint32_t)time(nullptr));
            packetResponse.Write<uint8_t>(1);
            packetResponse.Write<uint32_t>(0x15);

            SendPacket(packetResponse);
            break;
        }
        case Common::Opcode::CMSG_REALM_SPLIT:
        {
            std::string split_date = "01/01/01";
            uint32_t unk = 0;
            _packetBuffer.Read(&unk, 4);

            Common::ByteBuffer packetResponse;
            packetResponse.Write<uint8_t>(NOVUS_FOWARDPACKET);
            packetResponse.Write<uint64_t>(accountGuid);
            packetResponse.Write<uint16_t>(Common::Opcode::SMSG_REALM_SPLIT);

            packetResponse.Write<uint32_t>(unk);
            packetResponse.Write<uint32_t>(0x0); // split states: 0x0 realm normal, 0x1 realm split, 0x2 realm split pending
            packetResponse.WriteString(split_date);

            SendPacket(packetResponse);
            break;
        }
        case Common::Opcode::CMSG_CHAR_ENUM:
        {
            PreparedStatement stmt("SELECT characters.guid, characters.name, characters.race, characters.class, characters.gender, character_visual_data.skin, character_visual_data.face, character_visual_data.hair_style, character_visual_data.hair_color, character_visual_data.facial_style, characters.level, characters.zone_id, characters.map_id, characters.coordinate_x, characters.coordinate_y, characters.coordinate_z FROM characters INNER JOIN character_visual_data ON characters.guid=character_visual_data.guid WHERE characters.account={u};");
            stmt.Bind(accountGuid);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, accountGuid, clientOpcode](amy::result_set& results, DatabaseConnector& connector)
            {
                Common::ByteBuffer packetResponse;
                packetResponse.Write<uint8_t>(NOVUS_FOWARDPACKET);
                packetResponse.Write<uint64_t>(accountGuid);
                packetResponse.Write<uint16_t>(Common::Opcode::SMSG_CHAR_ENUM);

                // Number of characters
                packetResponse.Write<uint8_t>(results.affected_rows());

                /* Template for loading a character */
                for (auto& row : results)
                {
                    uint32_t lowGuid = row[0].as<amy::sql_int_unsigned>();
                    uint64_t actualGuid = uint64_t(lowGuid) | uint64_t(0x0000) << 48;

                    packetResponse.Write<uint64_t>(actualGuid); // Guid
                    packetResponse.WriteString(row[1].as<amy::sql_varchar>()); // Name
                    packetResponse.Write<uint8_t>(row[2].as<amy::sql_tinyint_unsigned>()); // Race
                    packetResponse.Write<uint8_t>(row[3].as<amy::sql_tinyint_unsigned>()); // Class
                    packetResponse.Write<uint8_t>(row[4].as<amy::sql_tinyint_unsigned>()); // Gender

                    packetResponse.Write<uint8_t>(row[5].as<amy::sql_tinyint_unsigned>()); // Skin
                    packetResponse.Write<uint8_t>(row[6].as<amy::sql_tinyint_unsigned>()); // Face
                    packetResponse.Write<uint8_t>(row[7].as<amy::sql_tinyint_unsigned>()); // Hairstyle
                    packetResponse.Write<uint8_t>(row[8].as<amy::sql_tinyint_unsigned>()); // Haircolor
                    packetResponse.Write<uint8_t>(row[9].as<amy::sql_tinyint_unsigned>()); // Facialstyle

                    packetResponse.Write<uint8_t>(row[10].as<amy::sql_tinyint_unsigned>()); // Level
                    packetResponse.Write<uint32_t>(row[11].as<amy::sql_tinyint_unsigned>()); // Zone Id
                    packetResponse.Write<uint32_t>(row[12].as<amy::sql_tinyint_unsigned>()); // Map Id

                    packetResponse.Write<float>(row[13].as<amy::sql_float>()); // X
                    packetResponse.Write<float>(row[14].as<amy::sql_float>()); // Y
                    packetResponse.Write<float>(row[15].as<amy::sql_float>()); // Z

                    packetResponse.Write<uint32_t>(0); // Guild Id

                    packetResponse.Write<uint32_t>(0); // Character Flags
                    packetResponse.Write<uint32_t>(0); // characterCustomize Flag

                    packetResponse.Write<uint8_t>(1); // First Login (Here we should probably do a playerTime check to determin if its the player's first login)

                    packetResponse.Write<uint32_t>(22234);  // Pet Display Id
                    packetResponse.Write<uint32_t>(5);  // Pet Level
                    packetResponse.Write<uint32_t>(1);  // Pet Family

                    uint32_t equipmentDataNull = 0;
                    for (int i = 0; i < 23; ++i)
                    {
                        packetResponse.Write<uint32_t>(equipmentDataNull);
                        packetResponse.Write<uint8_t>(0);
                        packetResponse.Write<uint32_t>(equipmentDataNull);
                    }
                }

                SendPacket(packetResponse);
            });
            break;
        }
        case Common::Opcode::CMSG_CHAR_CREATE:
        {
            std::string charName;
            uint8_t charRace;
            uint8_t charClass;
            uint8_t charGender;
            uint8_t charSkin;
            uint8_t charFace;
            uint8_t charHairStyle;
            uint8_t charHairColor;
            uint8_t charFacialStyle;
            uint8_t charOutfitId;

            _packetBuffer.Read(charName);
            _packetBuffer.Read(&charRace, 1);
            _packetBuffer.Read(&charClass, 1);
            _packetBuffer.Read(&charGender, 1);
            _packetBuffer.Read(&charSkin, 1);
            _packetBuffer.Read(&charFace, 1);
            _packetBuffer.Read(&charHairStyle, 1);
            _packetBuffer.Read(&charHairColor, 1);
            _packetBuffer.Read(&charFacialStyle, 1);
            _packetBuffer.Read(&charOutfitId, 1);

            PreparedStatement stmt("SELECT name FROM characters WHERE name={s};");
            stmt.Bind(charName);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, accountGuid, charName, charRace, charClass, charGender, charSkin, charFace, charHairStyle, charHairColor, charFacialStyle](amy::result_set& results, DatabaseConnector& connector)
            {
                Common::ByteBuffer packetResponse;
                packetResponse.Write<uint8_t>(NOVUS_FOWARDPACKET);
                packetResponse.Write<uint64_t>(accountGuid);
                packetResponse.Write<uint16_t>(Common::Opcode::SMSG_CHAR_CREATE);

                if (results.affected_rows() > 0)
                {
                    packetResponse.Write<uint8_t>(50); // CHAR_CREATE_NAME_IN_USE
                    SendPacket(packetResponse);
                    return;
                }

                PreparedStatement characterBaseData("INSERT INTO characters(account, name, race, gender, class, map_id, zone_id, coordinate_x, coordinate_y, coordinate_z) VALUES({u}, {s}, {i}, {i}, {i}, {i}, {i}, {f}, {f}, {f});");
                characterBaseData.Bind(accountGuid);
                characterBaseData.Bind(charName);
                characterBaseData.Bind(charRace);
                characterBaseData.Bind(charGender);
                characterBaseData.Bind(charClass);
                characterBaseData.Bind(0);
                characterBaseData.Bind(12);
                characterBaseData.Bind(-8949.950195f);
                characterBaseData.Bind(-132.492996f);
                characterBaseData.Bind(83.531197f);

                PreparedStatement characterVisualData("INSERT INTO character_visual_data(guid, skin, face, facial_style, hair_style, hair_color) VALUES(LAST_INSERT_ID(), {i}, {i}, {i}, {i}, {i});");
                characterVisualData.Bind(charSkin);
                characterVisualData.Bind(charFace);
                characterVisualData.Bind(charFacialStyle);
                characterVisualData.Bind(charHairStyle);
                characterVisualData.Bind(charHairColor);

                // This needs to be non-async as we rely on LAST_INSERT_ID() to retrieve the character's guid
                connector.Execute(characterBaseData);
                connector.Execute(characterVisualData);

                packetResponse.Write<uint8_t>(47); // CHAR_CREATE_SUCCESS
                SendPacket(packetResponse);
            });

            break;
        }
        case Common::Opcode::CMSG_CHAR_DELETE:
        {
            uint64_t guid;
            _packetBuffer.Read(&guid, 8);

            PreparedStatement stmt("SELECT account FROM characters WHERE guid={u};");
            stmt.Bind(guid);
            DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, accountGuid, guid](amy::result_set& results, DatabaseConnector& connector)
            {
                Common::ByteBuffer* packetResponse = new Common::ByteBuffer();
                packetResponse->Write<uint8_t>(NOVUS_FOWARDPACKET);
                packetResponse->Write<uint64_t>(accountGuid);
                packetResponse->Write<uint16_t>(Common::Opcode::SMSG_CHAR_DELETE);

                // Char doesn't exist
                if (results.affected_rows() == 0)
                {
                    packetResponse->Write<uint8_t>(72); // CHAR_DELETE_FAILED
                    SendPacket(*packetResponse);
                    return;
                }

                // Prevent deleting other player's characters
                uint64_t characterAccountGuid = results[0][0].as<amy::sql_bigint_unsigned>();
                if (accountGuid != characterAccountGuid)
                {
                    packetResponse->Write<uint8_t>(72); // CHAR_DELETE_FAILED
                    SendPacket(*packetResponse);
                    return;
                }

                PreparedStatement stmt("DELETE FROM characters WHERE guid={u};");
                stmt.Bind(guid);
                DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, packetResponse, guid](amy::result_set& results, DatabaseConnector& connector)
                {
                    PreparedStatement stmt("DELETE FROM character_visual_data WHERE guid={u};");
                    stmt.Bind(guid);
                    DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [this, packetResponse](amy::result_set& results, DatabaseConnector& connector)
                    {
                        packetResponse->Write<uint8_t>(71); // CHAR_DELETE_SUCCESS
                        SendPacket(*packetResponse);

                        delete packetResponse;
                    });
                });
            });

            break;
        }
        default:
            std::cout << "Received Unhandled opcode" << std::endl << std::endl;
            break;
    }

    return true;
}

void NovusConnection::SendPacket(Common::ByteBuffer& packet)
{
    _crypto->Encrypt(packet.GetReadPointer(), packet.GetActualSize());
    Send(packet);
}