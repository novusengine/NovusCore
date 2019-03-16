#include "CharacterDatabaseCache.h"
#include <Database/DatabaseConnector.h>
#include <Database/PreparedStatement.h>

CharacterDatabaseCache::CharacterDatabaseCache()
{
}

CharacterDatabaseCache::~CharacterDatabaseCache()
{
}

void CharacterDatabaseCache::Load()
{
    std::shared_ptr<DatabaseConnector> connector;
    bool result = DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector);
    assert(result);

    amy::result_set resultSet;
    connector->Query("SELECT characters.guid, characters.account, characters.name, characters.race, characters.gender, characters.class, characters.level, characters.map_id, characters.zone_id, characters.coordinate_x, characters.coordinate_y, characters.coordinate_z, characters.orientation, character_visual_data.skin, character_visual_data.face, character_visual_data.facial_style, character_visual_data.hair_style, character_visual_data.hair_color FROM characters INNER JOIN character_visual_data ON characters.guid = character_visual_data.guid;", resultSet);

    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            CharacterData newCharacterData(this, false);
            newCharacterData.guid = row[0].as<amy::sql_int_unsigned>();
            newCharacterData.account = row[1].as<amy::sql_int_unsigned>();
            newCharacterData.name = row[2].as<amy::sql_varchar>();
            newCharacterData.race = row[3].as<amy::sql_tinyint_unsigned>();
            newCharacterData.gender = row[4].as<amy::sql_tinyint_unsigned>();
            newCharacterData.classId = row[5].as<amy::sql_tinyint_unsigned>();
            newCharacterData.level = row[6].as<amy::sql_tinyint_unsigned>();
            newCharacterData.mapId = row[7].as<amy::sql_int_unsigned>();
            newCharacterData.zoneId = row[8].as<amy::sql_int_unsigned>();
            newCharacterData.coordinateX = row[9].as<amy::sql_float>();
            newCharacterData.coordinateY = row[10].as<amy::sql_float>();
            newCharacterData.coordinateZ = row[11].as<amy::sql_float>();
            newCharacterData.orientation = row[12].as<amy::sql_float>();

            CharacterVisualData newCharacterVisualData(this, false);
            newCharacterVisualData.guid = newCharacterData.guid;
            newCharacterVisualData.skin = row[13].as<amy::sql_tinyint_unsigned>();
            newCharacterVisualData.face = row[14].as<amy::sql_tinyint_unsigned>();
            newCharacterVisualData.facialStyle = row[15].as<amy::sql_tinyint_unsigned>();
            newCharacterVisualData.hairStyle = row[16].as<amy::sql_tinyint_unsigned>();
            newCharacterVisualData.hairColor = row[17].as<amy::sql_tinyint_unsigned>();

            _accessMutex.lock();
            _characterDataCache.insert({ newCharacterData.guid, newCharacterData });
            _characterVisualDataCache.insert({ newCharacterVisualData.guid, newCharacterVisualData });
            _accessMutex.unlock();
        }
    }
}

void CharacterDatabaseCache::LoadAsync()
{
}

void CharacterDatabaseCache::Save()
{
}

void CharacterDatabaseCache::SaveAsync()
{
}

CharacterData CharacterDatabaseCache::GetCharacterData(u64 guid)
{
    auto cache = _characterDataCache.find(guid);
    if (cache != _characterDataCache.end())
    {
        _accessMutex.lock_shared();
        CharacterData characterData = cache->second;
        _accessMutex.unlock_shared();

        return characterData;
    }
    else
    {
        // We don't have the character, so we load it
        std::shared_ptr<DatabaseConnector> connector;
        bool result = DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector);
        assert(result);

        PreparedStatement stmt("SELECT * FROM characters WHERE guid = {u};");
        stmt.Bind(guid);

        amy::result_set resultSet;
        connector->Query(stmt, resultSet);

        assert(resultSet.affected_rows() == 1);

        CharacterData newCharacterData(this, false);
        newCharacterData.guid = resultSet[0][0].as<amy::sql_int_unsigned>();
        newCharacterData.account = resultSet[0][1].as<amy::sql_int_unsigned>();

        newCharacterData.name = resultSet[0][2].as<amy::sql_varchar>();

        newCharacterData.race = resultSet[0][3].as<amy::sql_tinyint_unsigned>();
        newCharacterData.gender = resultSet[0][4].as<amy::sql_tinyint_unsigned>();
        newCharacterData.classId = resultSet[0][5].as<amy::sql_tinyint_unsigned>();
        newCharacterData.level = resultSet[0][6].as<amy::sql_tinyint_unsigned>();
        newCharacterData.mapId = resultSet[0][7].as<amy::sql_int_unsigned>();
        newCharacterData.zoneId = resultSet[0][8].as<amy::sql_int_unsigned>();
        newCharacterData.coordinateX = resultSet[0][9].as<amy::sql_float>();
        newCharacterData.coordinateY = resultSet[0][10].as<amy::sql_float>();
        newCharacterData.coordinateZ = resultSet[0][11].as<amy::sql_float>();
        newCharacterData.orientation = resultSet[0][12].as<amy::sql_float>();

        _accessMutex.lock();
        _characterDataCache.insert({ guid, newCharacterData });
        _accessMutex.unlock();

        return newCharacterData;
    }
}
const CharacterData CharacterDatabaseCache::GetCharacterDataReadOnly(u64 guid)
{
    CharacterData characterData(GetCharacterData(guid), true);
    return characterData;
}

CharacterVisualData CharacterDatabaseCache::GetCharacterVisualData(u64 guid)
{
    auto cache = _characterVisualDataCache.find(guid);
    if (cache != _characterVisualDataCache.end())
    {
        _accessMutex.lock_shared();
        CharacterVisualData characterVisualData = cache->second;
        _accessMutex.unlock_shared();

        return characterVisualData;
    }
    else
    {
        // We don't have the character, so we load it
        std::shared_ptr<DatabaseConnector> connector;
        bool result = DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector);
        assert(result);

        PreparedStatement stmt("SELECT * FROM character_visual_data WHERE guid = {u};");
        stmt.Bind(guid);

        amy::result_set resultSet;
        connector->Query(stmt, resultSet);

        assert(resultSet.affected_rows() == 1);

        CharacterVisualData newCharacterVisualData(this, false);
        newCharacterVisualData.guid = resultSet[0][0].as<amy::sql_int_unsigned>();
        newCharacterVisualData.skin = resultSet[0][1].as<amy::sql_tinyint_unsigned>();
        newCharacterVisualData.face = resultSet[0][2].as<amy::sql_tinyint_unsigned>();
        newCharacterVisualData.facialStyle = resultSet[0][3].as<amy::sql_tinyint_unsigned>();
        newCharacterVisualData.hairStyle = resultSet[0][4].as<amy::sql_tinyint_unsigned>();
        newCharacterVisualData.hairColor = resultSet[0][5].as<amy::sql_tinyint_unsigned>();

        _accessMutex.lock();
        _characterVisualDataCache.insert({ guid, newCharacterVisualData });
        _accessMutex.unlock();

        return newCharacterVisualData;
    }
}
const CharacterVisualData CharacterDatabaseCache::GetCharacterVisualDataReadOnly(u64 guid)
{
    CharacterVisualData characterVisualData(GetCharacterVisualData(guid), true);
    return characterVisualData;
}