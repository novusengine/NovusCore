#include <NovusTypes.h>
#include <Networking/ByteBuffer.h>
#include <Database\DatabaseConnector.h>
#include "../../NovusEnums.h"
#include "../../MessageHandler.h"
#include "../../RealmHandler.h"
#include "../../Connections/RealmConnection.h"
#include "../CharacterUtils.h"

namespace CharacterPacketUtils
{
bool HandlePlayerLogin(RealmConnection* connection)
{
    if (connection->enteringWorld)
        return false;

    connection->enteringWorld = true;

    std::shared_ptr<DatabaseConnector> connector = nullptr;
    if (!DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector))
    {
        connection->Close(asio::error::interrupted);
        return false;
    }

    RealmHandler* realmHandler = RealmHandler::Instance();
    i32 ip = realmHandler->worldNodeAddress;
    i16 port = realmHandler->worldNodePort;

    std::shared_ptr<ByteBuffer> redirectClient = ByteBuffer::Borrow<30>();
    redirectClient->PutI32(ip);
    redirectClient->PutI16(port);
    redirectClient->PutI32(0); // unk
#pragma warning(push)
#pragma warning(disable : 4312)
    HMACH hmac(40, connection->sessionKey->BN2BinArray(20).get());
    hmac.UpdateHash((u8*)&ip, sizeof(i32));
    hmac.UpdateHash((u8*)&port, sizeof(i16));
    hmac.Finish();
    redirectClient->PutBytes(hmac.GetData(), 20);
#pragma warning(pop)

    // Ensure the database is updated prior to sending REDIRECT.
    connection->GetPacketBuffer().GetU64(connection->characterGuid);
    PreparedStatement stmt("UPDATE characters SET online=1, lastLogin={u} WHERE guid={u};");
    stmt.Bind(static_cast<u32>(time(nullptr)));
    stmt.Bind(connection->characterGuid);
    connector->Execute(stmt);

    connection->SendPacket(redirectClient.get(), Opcode::SMSG_REDIRECT_CLIENT);
    return true;
}
bool HandleCharacterEnum(RealmConnection* connection)
{
    PreparedStatement stmt("SELECT characters.guid, characters.name, characters.race, characters.class, characters.gender, character_visual_data.skin, character_visual_data.face, character_visual_data.hair_style, character_visual_data.hair_color, character_visual_data.facial_style, characters.level, characters.zoneId, characters.mapId, characters.coordinate_x, characters.coordinate_y, characters.coordinate_z FROM characters INNER JOIN character_visual_data ON characters.guid=character_visual_data.guid WHERE characters.account={u};");
    stmt.Bind(connection->account);
    DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [connection](amy::result_set& results, DatabaseConnector& connector) {
        u8 characters = static_cast<u8>(results.affected_rows());
        std::shared_ptr<ByteBuffer> charEnum = ByteBuffer::Borrow<4096>();

        // Number of characters
        charEnum->PutU8(characters);

        /* Template for loading a character */
        for (auto& row : results)
        {
            charEnum->PutU64(row[0].GetU64());       // Guid
            charEnum->PutString(row[1].GetString()); // Name
            charEnum->PutU8(row[2].GetU8());         // Race
            charEnum->PutU8(row[3].GetU8());         // Class
            charEnum->PutU8(row[4].GetU8());         // Gender

            charEnum->PutU8(row[5].GetU8()); // Skin
            charEnum->PutU8(row[6].GetU8()); // Face
            charEnum->PutU8(row[7].GetU8()); // Hairstyle
            charEnum->PutU8(row[8].GetU8()); // Haircolor
            charEnum->PutU8(row[9].GetU8()); // Facialstyle

            charEnum->PutU8(row[10].GetU8());   // Level
            charEnum->PutU32(row[11].GetU16()); // Zone Id
            charEnum->PutU32(row[12].GetU16()); // Map Id

            charEnum->PutF32(row[13].GetF32()); // X
            charEnum->PutF32(row[14].GetF32()); // Y
            charEnum->PutF32(row[15].GetF32()); // Z

            charEnum->PutU32(0); // Guild Id

            charEnum->PutU32(0); // Character Flags
            charEnum->PutU32(0); // characterCustomize Flag

            charEnum->PutU8(1); // First Login (Here we should probably do a playerTime check to determin if its the player's first login)

            charEnum->PutU32(0); // Pet Display Id (Lich King: 22234)
            charEnum->PutU32(0); // Pet Level
            charEnum->PutU32(0); // Pet Family

            u32 equipmentDataNull = 0;
            for (i32 i = 0; i < 23; ++i)
            {
                charEnum->PutU32(equipmentDataNull);
                charEnum->PutU8(0);
                charEnum->PutU32(equipmentDataNull);
            }
        }

        connection->SendPacket(charEnum.get(), Opcode::SMSG_CHAR_ENUM);
    });

    return true;
}
bool HandleCharacterCreate(RealmConnection* connection)
{
    ByteBuffer& buffer = connection->GetPacketBuffer();
    cCharacterCreateData* createData = new cCharacterCreateData();
    createData->Read(buffer);

    PreparedStatement stmt("SELECT name FROM characters WHERE name={s};");
    stmt.Bind(createData->charName);
    DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [connection, createData](amy::result_set& results, DatabaseConnector& connector) {
        std::shared_ptr<ByteBuffer> characterCreateResult = ByteBuffer::Borrow<1>();
        if (results.affected_rows() > 0)
        {
            characterCreateResult->PutU8(CharacterResponses::CHAR_CREATE_NAME_IN_USE);
            connection->SendPacket(characterCreateResult.get(), Opcode::SMSG_CHAR_CREATE);
            delete createData;
            return;
        }

        /* Convert name to proper format */
        std::transform(createData->charName.begin(), createData->charName.end(), createData->charName.begin(), ::tolower);
        createData->charName[0] = std::toupper(createData->charName[0]);

        CharacterUtils::SpawnPosition spawnPosition;
        if (!CharacterUtils::BuildGetDefaultSpawn(connection->charCache.GetDefaultSpawnStorageData(), createData->charRace, createData->charClass, spawnPosition))
        {
            characterCreateResult->PutU8(CharacterResponses::CHAR_CREATE_DISABLED);
            connection->SendPacket(characterCreateResult.get(), Opcode::SMSG_CHAR_CREATE);
            delete createData;
            return;
        }

        PreparedStatement characterBaseData("INSERT INTO characters(account, name, race, gender, class, mapId, zoneId, coordinate_x, coordinate_y, coordinate_z, orientation) VALUES({u}, {s}, {u}, {u}, {u}, {u}, {u}, {f}, {f}, {f}, {f});");
        characterBaseData.Bind(connection->account);
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
        connection->characterGuid = guidResult[0][0].as<amy::sql_bigint_unsigned>();

        PreparedStatement characterVisualData("INSERT INTO character_visual_data(guid, skin, face, facial_style, hair_style, hair_color) VALUES({u}, {u}, {u}, {u}, {u}, {u});");
        characterVisualData.Bind(connection->characterGuid);
        characterVisualData.Bind(createData->charSkin);
        characterVisualData.Bind(createData->charFace);
        characterVisualData.Bind(createData->charFacialStyle);
        characterVisualData.Bind(createData->charHairStyle);
        characterVisualData.Bind(createData->charHairColor);
        connector.ExecuteAsync(characterVisualData);

        // Baseline Skills
        std::string skillSql;
        if (CharacterUtils::BuildDefaultSkillSQL(connection->charCache.GetDefaultSkillStorageData(), connection->characterGuid, createData->charRace, createData->charClass, skillSql))
        {
            connector.ExecuteAsync(skillSql);
        }

        // Baseline Spells
        std::string spellSql;
        if (CharacterUtils::BuildDefaultSpellSQL(connection->charCache.GetDefaultSpellStorageData(), connection->characterGuid, createData->charRace, createData->charClass, spellSql))
        {
            connector.ExecuteAsync(spellSql);
        }

        DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, [connection](std::shared_ptr<DatabaseConnector>& connector) {
            u32 realmId = 1;
            PreparedStatement realmCharacterCount("INSERT INTO realm_characters(account, realmid, characters) VALUES({u}, {u}, 1) ON DUPLICATE KEY UPDATE characters = characters + 1;");
            realmCharacterCount.Bind(connection->account);
            realmCharacterCount.Bind(realmId); // Realm Id
            connector->Execute(realmCharacterCount);
        });

        characterCreateResult->PutU8(CharacterResponses::CHAR_CREATE_SUCCESS);
        connection->SendPacket(characterCreateResult.get(), Opcode::SMSG_CHAR_CREATE);

        delete createData;
    });

    return true;
}
bool HandleCharacterDelete(RealmConnection* connection)
{
    ByteBuffer& buffer = connection->GetPacketBuffer();
    u64 guid = 0;
    buffer.GetU64(guid);

    PreparedStatement stmt("SELECT account FROM characters WHERE guid={u};");
    stmt.Bind(guid);
    DatabaseConnector::QueryAsync(DATABASE_TYPE::CHARSERVER, stmt, [connection, guid](amy::result_set& results, DatabaseConnector& connector) {
        std::shared_ptr<ByteBuffer> characterDeleteResult = ByteBuffer::Borrow<1>();

        // Char doesn't exist
        if (results.affected_rows() == 0)
        {
            characterDeleteResult->PutU8(CharacterResponses::CHAR_DELETE_FAILED);
            connection->SendPacket(characterDeleteResult.get(), Opcode::SMSG_CHAR_DELETE);
            return;
        }

        // Prevent deleting other player's characters
        u64 characterAccountGuid = results[0][0].as<amy::sql_bigint_unsigned>();
        if (connection->account != characterAccountGuid)
        {
            characterDeleteResult->PutU8(CharacterResponses::CHAR_DELETE_FAILED);
            connection->SendPacket(characterDeleteResult.get(), Opcode::SMSG_CHAR_DELETE);
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

        DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, [connection](std::shared_ptr<DatabaseConnector>& connector) {
            u32 realmId = 1;
            PreparedStatement realmCharacterCount("UPDATE realm_characters SET characters=characters-1 WHERE account={u} and realmid={u};");
            realmCharacterCount.Bind(connection->account);
            realmCharacterCount.Bind(realmId);
            connector->Execute(realmCharacterCount);
        });

        characterDeleteResult->PutU8(CharacterResponses::CHAR_DELETE_SUCCESS);
        connection->SendPacket(characterDeleteResult.get(), Opcode::SMSG_CHAR_DELETE);
    });

    return true;
}


void RegisterCharacterPacketHandlers()
{
    MessageHandler* messageHandler = MessageHandler::Instance();

    messageHandler->SetMessageHandler(Opcode::CMSG_PLAYER_LOGIN, HandlePlayerLogin);
    messageHandler->SetMessageHandler(Opcode::CMSG_CHAR_ENUM, HandleCharacterEnum);
    messageHandler->SetMessageHandler(Opcode::CMSG_CHAR_CREATE, HandleCharacterCreate);
    messageHandler->SetMessageHandler(Opcode::CMSG_CHAR_DELETE, HandleCharacterDelete);
}
} // namespace CharacterPacketUtils