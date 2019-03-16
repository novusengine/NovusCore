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

CharacterData CharacterDatabaseCache::GetCharacterData(u32 guid)
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

const CharacterData CharacterDatabaseCache::GetCharacterDataReadOnly(u32 guid)
{
    CharacterData characterData(GetCharacterData(guid), true);
    return characterData;
}