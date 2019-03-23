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

    connector->Query("SELECT guid, spell FROM character_spell_storage;", resultSet); 
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            CharacterSpellStorage newCharacterSpellStorage(this);
            u64 guid = row[0].as<amy::sql_bigint_unsigned>();
            newCharacterSpellStorage.id = row[1].as<amy::sql_int_unsigned>();

            _accessMutex.lock();
            _characterSpellStorageCache[guid].push_back(newCharacterSpellStorage);
            _accessMutex.unlock();
        }
    }


    connector->Query("SELECT guid, skill, value, character_skill_storage.maxValue FROM character_skill_storage;", resultSet);
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            CharacterSkillStorage newCharacterSkillStorage(this);
            u64 guid = row[0].as<amy::sql_bigint_unsigned>();
            newCharacterSkillStorage.id = row[1].as<amy::sql_smallint_unsigned>();
            newCharacterSkillStorage.value = row[2].as<amy::sql_smallint_unsigned>();
            newCharacterSkillStorage.maxValue = row[3].as<amy::sql_smallint_unsigned>();

            _accessMutex.lock();
            _characterSkillStorageCache[guid].push_back(newCharacterSkillStorage);
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
bool CharacterDatabaseCache::GetCharacterSpellStorage(u64 guid, std::vector<CharacterSpellStorage>& output)
{
    auto cache = _characterSpellStorageCache.find(guid);
    if (cache != _characterSpellStorageCache.end())
    {
        _accessMutex.lock_shared();
        std::vector<CharacterSpellStorage> characterSpellStorageData = cache->second;
        _accessMutex.unlock_shared();

        output = characterSpellStorageData;
        return true;
    }
    else
    {
        // We don't have the character, so we load it
        std::shared_ptr<DatabaseConnector> connector;
        bool result = DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector);
        assert(result);

        PreparedStatement stmt("SELECT guid, spell FROM character_spell_storage WHERE guid = {u};");
        stmt.Bind(guid);

        amy::result_set resultSet;
        connector->Query(stmt, resultSet);

        if (resultSet.affected_rows() == 0)
            return false;

        CharacterSpellStorage newCharacterSpellStorage(this);
        u64 guid = resultSet[0][0].as<amy::sql_bigint_unsigned>();
        newCharacterSpellStorage.id = resultSet[0][1].as<amy::sql_int_unsigned>();

        _accessMutex.lock();
        _characterSpellStorageCache[guid].push_back(newCharacterSpellStorage);
        _accessMutex.unlock();

        output = _characterSpellStorageCache[guid];
        return true;
    }
}
bool CharacterDatabaseCache::GetCharacterSkillStorage(u64 guid, std::vector<CharacterSkillStorage>& output)
{
    auto cache = _characterSkillStorageCache.find(guid);
    if (cache != _characterSkillStorageCache.end())
    {
        _accessMutex.lock_shared();
        std::vector<CharacterSkillStorage> characterSkillStorageData = cache->second;
        _accessMutex.unlock_shared();

        output = characterSkillStorageData;
        return true;
    }
    else
    {
        // We don't have the character, so we load it
        std::shared_ptr<DatabaseConnector> connector;
        bool result = DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector);
        assert(result);

        PreparedStatement stmt("SELECT guid, skill, value, character_skill_storage.maxValue FROM character_skill_storage WHERE guid = {u};");
        stmt.Bind(guid);

        amy::result_set resultSet;
        connector->Query(stmt, resultSet);

        if (resultSet.affected_rows() == 0)
            return false;

        CharacterSkillStorage newCharacterSkillStorage(this);
        u64 guid = resultSet[0][0].as<amy::sql_bigint_unsigned>();
        newCharacterSkillStorage.id = resultSet[0][1].as<amy::sql_smallint_unsigned>();
        newCharacterSkillStorage.value = resultSet[0][2].as<amy::sql_smallint_unsigned>();
        newCharacterSkillStorage.maxValue = resultSet[0][3].as<amy::sql_smallint_unsigned>();

        _accessMutex.lock();
        _characterSkillStorageCache[guid].push_back(newCharacterSkillStorage);
        _accessMutex.unlock();

        output = _characterSkillStorageCache[guid];
        return true;
    }
}