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
            newCharacterData.guid = row[0].GetU64();
            newCharacterData.account = row[1].GetU32();
            newCharacterData.name = row[2].GetString();
            newCharacterData.race = row[3].GetU8();
            newCharacterData.gender = row[4].GetU8();
            newCharacterData.classId = row[5].GetU8();
            newCharacterData.level = row[6].GetU8();
            newCharacterData.mapId = row[7].GetU32();
            newCharacterData.zoneId = row[8].GetU32();
            newCharacterData.coordinateX = row[9].GetF32();
            newCharacterData.coordinateY = row[10].GetF32();
            newCharacterData.coordinateZ = row[11].GetF32();
            newCharacterData.orientation = row[12].GetF32();

            CharacterVisualData newCharacterVisualData(this);
            newCharacterVisualData.guid = newCharacterData.guid;
            newCharacterVisualData.skin = row[13].GetU8();
            newCharacterVisualData.face = row[14].GetU8();
            newCharacterVisualData.facialStyle = row[15].GetU8();
            newCharacterVisualData.hairStyle = row[16].GetU8();
            newCharacterVisualData.hairColor = row[17].GetU8();

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
            u64 guid = row[0].GetU64();
            newCharacterSpellStorage.id = row[1].GetU32();

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
            u64 guid = row[0].GetU64();
            newCharacterSkillStorage.id = row[1].GetU16();
            newCharacterSkillStorage.value = row[2].GetU16();
            newCharacterSkillStorage.maxValue = row[3].GetU16();

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
        amy::row resultRow = resultSet[0];

        newCharacterData.guid = resultRow[0].GetU32();
        newCharacterData.account = resultRow[1].GetU32();

        newCharacterData.name = resultRow[2].GetString();

        newCharacterData.race = resultRow[3].GetU8();
        newCharacterData.gender = resultRow[4].GetU8();
        newCharacterData.classId = resultRow[5].GetU8();
        newCharacterData.level = resultRow[6].GetU8();
        newCharacterData.mapId = resultRow[7].GetU32();
        newCharacterData.zoneId = resultRow[8].GetU32();
        newCharacterData.coordinateX = resultRow[9].GetF32();
        newCharacterData.coordinateY = resultRow[10].GetF32();
        newCharacterData.coordinateZ = resultRow[11].GetF32();
        newCharacterData.orientation = resultRow[12].GetF32();

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
        amy::row resultRow = resultSet[0];

        newCharacterVisualData.guid = resultRow[0].GetU32();
        newCharacterVisualData.skin = resultRow[1].GetU8();
        newCharacterVisualData.face = resultRow[2].GetU8();
        newCharacterVisualData.facialStyle = resultRow[3].GetU8();
        newCharacterVisualData.hairStyle = resultRow[4].GetU8();
        newCharacterVisualData.hairColor = resultRow[5].GetU8();

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

        _accessMutex.lock();
        for (auto row : resultSet)
        {
            CharacterSpellStorage newCharacterSpellStorage(this);
            u64 guid = row[0].GetU64();
            newCharacterSpellStorage.id = row[1].GetU32();

            _characterSpellStorageCache[guid].push_back(newCharacterSpellStorage);
        }
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

        _accessMutex.lock();
        for (auto row : resultSet)
        {
            CharacterSkillStorage newCharacterSkillStorage(this);
            u64 guid = row[0].GetU64();
            newCharacterSkillStorage.id = row[1].GetU16();
            newCharacterSkillStorage.value = row[2].GetU16();
            newCharacterSkillStorage.maxValue = row[3].GetU16();

            _characterSkillStorageCache[guid].push_back(newCharacterSkillStorage);
        }
        _accessMutex.unlock();

        output = _characterSkillStorageCache[guid];
        return true;
    }
}