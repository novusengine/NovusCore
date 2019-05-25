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
            CharacterInfo newCharacterInfo(this);
            newCharacterInfo.guid = row[0].as<amy::sql_bigint_unsigned>();
            newCharacterInfo.account = row[1].as<amy::sql_int_unsigned>();
            newCharacterInfo.name = row[2].as<amy::sql_varchar>();
            newCharacterInfo.race = row[3].as<amy::sql_tinyint_unsigned>();
            newCharacterInfo.gender = row[4].as<amy::sql_tinyint_unsigned>();
            newCharacterInfo.classId = row[5].as<amy::sql_tinyint_unsigned>();
            newCharacterInfo.level = row[6].as<amy::sql_tinyint_unsigned>();
            newCharacterInfo.mapId = row[7].as<amy::sql_int_unsigned>();
            newCharacterInfo.zoneId = row[8].as<amy::sql_int_unsigned>();
            newCharacterInfo.coordinateX = row[9].as<amy::sql_float>();
            newCharacterInfo.coordinateY = row[10].as<amy::sql_float>();
            newCharacterInfo.coordinateZ = row[11].as<amy::sql_float>();
            newCharacterInfo.orientation = row[12].as<amy::sql_float>();

            CharacterVisualData newCharacterVisualData(this);
            newCharacterVisualData.guid = newCharacterInfo.guid;
            newCharacterVisualData.skin = row[13].as<amy::sql_tinyint_unsigned>();
            newCharacterVisualData.face = row[14].as<amy::sql_tinyint_unsigned>();
            newCharacterVisualData.facialStyle = row[15].as<amy::sql_tinyint_unsigned>();
            newCharacterVisualData.hairStyle = row[16].as<amy::sql_tinyint_unsigned>();
            newCharacterVisualData.hairColor = row[17].as<amy::sql_tinyint_unsigned>();

            _accessMutex.lock();
            _characterInfoCache.insert({ newCharacterInfo.guid, newCharacterInfo });
            _characterVisualDataCache.insert({ newCharacterVisualData.guid, newCharacterVisualData });
            _accessMutex.unlock();
        }
    }

    connector->Query("SELECT guid, type, timestamp, data FROM character_data", resultSet);
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            CharacterData newCharacterData(this);
            newCharacterData.guid = row[0].as<amy::sql_bigint_unsigned>();
            newCharacterData.type = row[1].as<amy::sql_int_unsigned>();
            newCharacterData.timestamp = row[2].as<amy::sql_int_unsigned>();
            newCharacterData.data = row[3].as<amy::sql_blob>();
            newCharacterData.loaded = true;

            _accessMutex.lock();
            _characterDataCache.insert({ newCharacterData.guid, newCharacterData });
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
    DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, [this](std::shared_ptr<DatabaseConnector> & connector)
    {
        // Save Character Data
        for (auto itr : _characterDataCache)
        {
            for (CharacterData characterData : itr.second)
            {
                if (characterData.loaded)
                {
                    PreparedStatement characterDataStmt("INSERT INTO character_data(guid, character_data.type, character_data.timestamp, character_data.data) VALUES({u}, {u}, {u}, {s}) ON DUPLICATE KEY UPDATE character_data.timestamp={u}, character_data.data={s};");
                    characterDataStmt.Bind(characterData.guid);
                    characterDataStmt.Bind(characterData.type);
                    characterDataStmt.Bind(characterData.timestamp);
                    characterDataStmt.Bind(characterData.data);
                    characterDataStmt.Bind(characterData.timestamp);
                    characterDataStmt.Bind(characterData.data);

                    connector->Execute(characterDataStmt);
                }
            }
        }
    });
}

void CharacterDatabaseCache::SaveAsync()
{
}

void CharacterDatabaseCache::SaveAndUnloadCharacter(u64 characterGuid)
{
    SaveCharacter(characterGuid);
    UnloadCharacter(characterGuid);
}
void CharacterDatabaseCache::SaveCharacter(u64 characterGuid)
{
    DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, [this, characterGuid](std::shared_ptr<DatabaseConnector> & connector)
    {
        CharacterInfo characterInfo = _characterInfoCache[characterGuid];
        PreparedStatement characterBaseData("UPDATE characters SET level={u}, mapId={u}, zoneId={u}, coordinate_x={f}, coordinate_y={f}, coordinate_z={f}, orientation={f} WHERE guid={u};");
        characterBaseData.Bind(characterInfo.level);
        characterBaseData.Bind(characterInfo.mapId);
        characterBaseData.Bind(characterInfo.zoneId);
        characterBaseData.Bind(characterInfo.coordinateX);
        characterBaseData.Bind(characterInfo.coordinateY);
        characterBaseData.Bind(characterInfo.coordinateZ);
        characterBaseData.Bind(characterInfo.orientation);
        characterBaseData.Bind(characterGuid);
        connector->Execute(characterBaseData);

        // Save Visual Data
        CharacterVisualData characterVisualData = _characterVisualDataCache[characterGuid];
        PreparedStatement characterBaseVisualData("UPDATE character_visual_data SET skin={u}, face={u}, facial_style={u}, hair_style={u}, hair_color={u} WHERE guid={u};");
        characterBaseVisualData.Bind(characterVisualData.skin);
        characterBaseVisualData.Bind(characterVisualData.face);
        characterBaseVisualData.Bind(characterVisualData.facialStyle);
        characterBaseVisualData.Bind(characterVisualData.hairStyle);
        characterBaseVisualData.Bind(characterVisualData.hairColor);
        characterBaseVisualData.Bind(characterGuid);
        connector->Execute(characterBaseVisualData);

        // Save Character Data
        for (CharacterData characterData : _characterDataCache[characterGuid])
        {
            if (characterData.loaded)
            {
                PreparedStatement characterDataStmt("INSERT INTO character_data(guid, character_data.type, character_data.timestamp, character_data.data) VALUES({u}, {u}, {u}, {s}) ON DUPLICATE KEY UPDATE character_data.timestamp={u}, character_data.data={s};");
                characterDataStmt.Bind(characterData.guid);
                characterDataStmt.Bind(characterData.type);
                characterDataStmt.Bind(characterData.timestamp);
                characterDataStmt.Bind(characterData.data);
                characterDataStmt.Bind(characterData.timestamp);
                characterDataStmt.Bind(characterData.data);

                connector->Execute(characterDataStmt);
            }
        }
    });
}
void CharacterDatabaseCache::UnloadCharacter(u64 characterGuid)
{
    _characterInfoCache.erase(characterGuid);
    _characterVisualDataCache.erase(characterGuid);
    _characterDataCache.erase(characterGuid);
}

bool CharacterDatabaseCache::GetCharacterInfo(u64 guid, CharacterInfo& output)
{
    auto cache = _characterInfoCache.find(guid);
    if (cache != _characterInfoCache.end())
    {
        _accessMutex.lock_shared();
        CharacterInfo characterInfo = cache->second;
        _accessMutex.unlock_shared();

        output = characterInfo;
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

        CharacterInfo newCharacterInfo(this);
        newCharacterInfo.guid = resultSet[0][0].as<amy::sql_int_unsigned>();
        newCharacterInfo.account = resultSet[0][1].as<amy::sql_int_unsigned>();

        newCharacterInfo.name = resultSet[0][2].as<amy::sql_varchar>();

        newCharacterInfo.race = resultSet[0][3].as<amy::sql_tinyint_unsigned>();
        newCharacterInfo.gender = resultSet[0][4].as<amy::sql_tinyint_unsigned>();
        newCharacterInfo.classId = resultSet[0][5].as<amy::sql_tinyint_unsigned>();
        newCharacterInfo.level = resultSet[0][6].as<amy::sql_tinyint_unsigned>();
        newCharacterInfo.mapId = resultSet[0][7].as<amy::sql_int_unsigned>();
        newCharacterInfo.zoneId = resultSet[0][8].as<amy::sql_int_unsigned>();
        newCharacterInfo.coordinateX = resultSet[0][9].as<amy::sql_float>();
        newCharacterInfo.coordinateY = resultSet[0][10].as<amy::sql_float>();
        newCharacterInfo.coordinateZ = resultSet[0][11].as<amy::sql_float>();
        newCharacterInfo.orientation = resultSet[0][12].as<amy::sql_float>();

        _accessMutex.lock();
        _characterInfoCache.insert({ guid, newCharacterInfo });
        _accessMutex.unlock();

        output = newCharacterInfo;
        return true;
    }
}
bool CharacterDatabaseCache::GetCharacterData(u64 characterGuid, u32 type, CharacterData& output)
{
    auto cache = _characterDataCache.find(characterGuid);
    if (cache != _characterDataCache.end())
    {
        CharacterData characterData = cache->second[type];
        if (characterData.loaded)
        {
            output = characterData;
            return true;
        }
    }

    std::shared_ptr<DatabaseConnector> connector;
    bool result = DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector);
    assert(result);
    if (!result)
        return false;

    PreparedStatement stmt("SELECT * FROM character_data WHERE guid = {u} AND type = {u};");
    stmt.Bind(characterGuid);
    stmt.Bind(type);

    amy::result_set resultSet;
    connector->Query(stmt, resultSet);

    CharacterData newCharacterData(this);
    if (resultSet.affected_rows() == 0)
    {
        newCharacterData.guid = characterGuid;
        newCharacterData.type = type;
        newCharacterData.timestamp = 0;
        newCharacterData.data = "";
    }
    else
    {
        newCharacterData.guid = resultSet[0][0].as<amy::sql_bigint_unsigned>();
        newCharacterData.type = resultSet[0][1].as<amy::sql_int_unsigned>();
        newCharacterData.timestamp = resultSet[0][2].as<amy::sql_int_unsigned>();
        newCharacterData.data = resultSet[0][3].as<amy::sql_blob>();
    }
    newCharacterData.loaded = true;

    _accessMutex.lock();
    _characterDataCache.insert({ characterGuid, newCharacterData });
    _accessMutex.unlock();

    output = newCharacterData;
    return true;
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

void CharacterData::UpdateCache()
{
    loaded = true;
    _cache->_characterDataCache[guid][type] = *this;
    _cache->Save();
}