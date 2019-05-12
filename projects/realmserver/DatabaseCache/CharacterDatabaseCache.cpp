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
    if (!result)
        return;

    amy::result_set resultSet;
    connector->Query("SELECT characters.guid, characters.account, characters.name, characters.race, characters.gender, characters.class, characters.level, characters.mapId, characters.zoneId, characters.coordinate_x, characters.coordinate_y, characters.coordinate_z, characters.orientation, character_visual_data.skin, character_visual_data.face, character_visual_data.facial_style, character_visual_data.hair_style, character_visual_data.hair_color FROM characters INNER JOIN character_visual_data ON characters.guid = character_visual_data.guid;", resultSet);

    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            CharacterData newCharacterData(this);
            newCharacterData.guid = row[0].as<amy::sql_bigint_unsigned>();
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

            CharacterVisualData newCharacterVisualData(this);
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

    connector->Query("SELECT raceMask, classMask, spell FROM default_spells;", resultSet); 
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            DefaultSpellStorage newDefaultSpellStorage(this);
            newDefaultSpellStorage.raceMask = row[0].as<amy::sql_smallint>();
            newDefaultSpellStorage.classMask = row[1].as<amy::sql_smallint>();
            newDefaultSpellStorage.id = row[2].as<amy::sql_int_unsigned>();

            _accessMutex.lock();
            _defaultSpellStorageCache.push_back(newDefaultSpellStorage);
            _accessMutex.unlock();
        }
    }

    connector->Query("SELECT raceMask, classMask, skill, value, default_skills.maxValue FROM default_skills;", resultSet);
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            DefaultSkillStorage newDefaultSkillStorage(this);
            newDefaultSkillStorage.raceMask = row[0].as<amy::sql_smallint>();
            newDefaultSkillStorage.classMask = row[1].as<amy::sql_smallint>();
            newDefaultSkillStorage.id = row[2].as<amy::sql_smallint_unsigned>();
            newDefaultSkillStorage.value = row[3].as<amy::sql_smallint_unsigned>();
            newDefaultSkillStorage.maxValue = row[4].as<amy::sql_smallint_unsigned>();

            _accessMutex.lock();
            _defaultSkillStorageCache.push_back(newDefaultSkillStorage);
            _accessMutex.unlock();
        }
    }

    connector->Query("SELECT raceMask, classMask, mapId, zoneId, coordinate_x, coordinate_y, coordinate_z, orientation FROM default_spawns;", resultSet);
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            DefaultSpawnStorage newDefaultSpawnStorage(this);
            newDefaultSpawnStorage.raceMask = row[0].as<amy::sql_smallint>();
            newDefaultSpawnStorage.classMask = row[1].as<amy::sql_smallint>();
            newDefaultSpawnStorage.mapId = row[2].as<amy::sql_smallint_unsigned>();
            newDefaultSpawnStorage.zoneId = row[3].as<amy::sql_smallint_unsigned>();
            newDefaultSpawnStorage.coordinate_x = row[4].as<amy::sql_float>();
            newDefaultSpawnStorage.coordinate_y = row[5].as<amy::sql_float>();
            newDefaultSpawnStorage.coordinate_z = row[6].as<amy::sql_float>();
            newDefaultSpawnStorage.orientation = row[7].as<amy::sql_float>();

            _accessMutex.lock();
            _defaultSpawnStorageCache.push_back(newDefaultSpawnStorage);
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

bool CharacterDatabaseCache::GetCharacterData(u64 guid, CharacterData& output)
{
    auto cache = _characterDataCache.find(guid);
    if (cache != _characterDataCache.end())
    {
        _accessMutex.lock_shared();
        CharacterData characterData = cache->second;
        _accessMutex.unlock_shared();

        output = characterData;
        return true;
    }
    else
    {
        // We don't have the character, so we load it
        std::shared_ptr<DatabaseConnector> connector;
        bool result = DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector);
        assert(result);
        if (!result)
            return false;

        PreparedStatement stmt("SELECT * FROM characters WHERE guid = {u};");
        stmt.Bind(guid);

        amy::result_set resultSet;
        connector->Query(stmt, resultSet);

        if (resultSet.affected_rows() == 0)
            return false;

        CharacterData newCharacterData(this);
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

        output = newCharacterData;
        return true;
    }
}
bool CharacterDatabaseCache::GetCharacterVisualData(u64 guid, CharacterVisualData& output)
{
    auto cache = _characterVisualDataCache.find(guid);
    if (cache != _characterVisualDataCache.end())
    {
        _accessMutex.lock_shared();
        CharacterVisualData characterVisualData = cache->second;
        _accessMutex.unlock_shared();

        output = characterVisualData;
        return true;
    }
    else
    {
        // We don't have the character, so we load it
        std::shared_ptr<DatabaseConnector> connector;
        bool result = DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector);
        assert(result);
        if (!result)
            return false;

        PreparedStatement stmt("SELECT * FROM character_visual_data WHERE guid = {u};");
        stmt.Bind(guid);

        amy::result_set resultSet;
        connector->Query(stmt, resultSet);

        if (resultSet.affected_rows() == 0)
            return false;

        CharacterVisualData newCharacterVisualData(this);
        newCharacterVisualData.guid = resultSet[0][0].as<amy::sql_int_unsigned>();
        newCharacterVisualData.skin = resultSet[0][1].as<amy::sql_tinyint_unsigned>();
        newCharacterVisualData.face = resultSet[0][2].as<amy::sql_tinyint_unsigned>();
        newCharacterVisualData.facialStyle = resultSet[0][3].as<amy::sql_tinyint_unsigned>();
        newCharacterVisualData.hairStyle = resultSet[0][4].as<amy::sql_tinyint_unsigned>();
        newCharacterVisualData.hairColor = resultSet[0][5].as<amy::sql_tinyint_unsigned>();

        _accessMutex.lock();
        _characterVisualDataCache.insert({ guid, newCharacterVisualData });
        _accessMutex.unlock();

        output = newCharacterVisualData;
        return true;
    }
}
const std::vector<DefaultSpellStorage> CharacterDatabaseCache::GetDefaultSpellStorageData()
{
    return _defaultSpellStorageCache;
}
const std::vector<DefaultSkillStorage> CharacterDatabaseCache::GetDefaultSkillStorageData()
{
    return _defaultSkillStorageCache;
}
const std::vector<DefaultSpawnStorage> CharacterDatabaseCache::GetDefaultSpawnStorageData()
{
    return _defaultSpawnStorageCache;
}