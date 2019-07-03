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
    connector->Query("SELECT characters.guid, characters.account, characters.name, characters.race, characters.gender, characters.class, characters.level, characters.mapId, characters.zoneId, characters.coordinate_x, characters.coordinate_y, characters.coordinate_z, characters.orientation, characters.online, character_visual_data.skin, character_visual_data.face, character_visual_data.facial_style, character_visual_data.hair_style, character_visual_data.hair_color FROM characters INNER JOIN character_visual_data ON characters.guid = character_visual_data.guid;", resultSet);

    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            CharacterInfo newCharacterInfo(this);
            newCharacterInfo.guid = row[0].GetU64();
            newCharacterInfo.account = row[1].GetU32();
            newCharacterInfo.name = row[2].GetString();
            newCharacterInfo.race = row[3].GetU8();
            newCharacterInfo.gender = row[4].GetU8();
            newCharacterInfo.classId = row[5].GetU8();
            newCharacterInfo.level = row[6].GetU8();
            newCharacterInfo.mapId = row[7].GetU32();
            newCharacterInfo.zoneId = row[8].GetU32();
            newCharacterInfo.position.x = row[9].GetF32();
            newCharacterInfo.position.y = row[10].GetF32();
            newCharacterInfo.position.z = row[11].GetF32();
            newCharacterInfo.orientation = row[12].GetF32();
            newCharacterInfo.online = row[13].GetU8();

            CharacterVisualData newCharacterVisualData(this);
            newCharacterVisualData.guid = newCharacterInfo.guid;
            newCharacterVisualData.skin = row[14].GetU8();
            newCharacterVisualData.face = row[15].GetU8();
            newCharacterVisualData.facialStyle = row[16].GetU8();
            newCharacterVisualData.hairStyle = row[17].GetU8();
            newCharacterVisualData.hairColor = row[18].GetU8();

            _accessMutex.lock();
            _characterInfoCache[newCharacterInfo.guid] = newCharacterInfo;
            _characterVisualDataCache[newCharacterInfo.guid] = newCharacterVisualData;
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
            _characterDataCache.insert({newCharacterData.guid, newCharacterData});
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
            _characterSpellStorageCache[guid][newCharacterSpellStorage.id] = newCharacterSpellStorage;
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
            _characterSkillStorageCache[guid][newCharacterSkillStorage.id] = newCharacterSkillStorage;
            _accessMutex.unlock();
        }
    }

    connector->Query("SELECT lowGuid, itemEntry, bagSlot, bagPosition, characterGuid FROM character_items;", resultSet);
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            CharacterItemData newCharacterItemData(this);
            newCharacterItemData.lowGuid = row[0].GetU32();
            newCharacterItemData.itemEntry = row[1].GetU32();
            newCharacterItemData.bagSlot = row[2].GetU8();
            newCharacterItemData.bagPosition = row[3].GetU32();
            newCharacterItemData.characterGuid = row[4].GetU32();

            _accessMutex.lock();
            _characteritemDataCache[newCharacterItemData.characterGuid][newCharacterItemData.lowGuid] = newCharacterItemData;
            _accessMutex.unlock();
        }
    }
}
void CharacterDatabaseCache::LoadAsync()
{
}

void CharacterDatabaseCache::Save()
{
    DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, [this](std::shared_ptr<DatabaseConnector>& connector) {
        for (auto itr : _characterInfoCache)
        {
            CharacterInfo characterInfo = itr.second;
            PreparedStatement characterBaseData("UPDATE characters set mapId={u}, zoneId={u}, coordinate_x={f}, coordinate_y={f}, coordinate_z={f}, orientation={f} WHERE guid={u};");
            characterBaseData.Bind(characterInfo.mapId);
            characterBaseData.Bind(characterInfo.zoneId);
            characterBaseData.Bind(characterInfo.position.x);
            characterBaseData.Bind(characterInfo.position.y);
            characterBaseData.Bind(characterInfo.position.z);
            characterBaseData.Bind(characterInfo.orientation);
            characterBaseData.Bind(characterInfo.guid);

            connector->Execute(characterBaseData);
        }

        for (auto itr : _characterVisualDataCache)
        {
            CharacterVisualData characterVisualData = itr.second;
            PreparedStatement characterBaseVisualData("UPDATE character_visual_data set skin={u}, face={u}, facial_style={u}, hair_style={u}, hair_color={u} WHERE guid={u};");
            characterBaseVisualData.Bind(characterVisualData.skin);
            characterBaseVisualData.Bind(characterVisualData.face);
            characterBaseVisualData.Bind(characterVisualData.facialStyle);
            characterBaseVisualData.Bind(characterVisualData.hairStyle);
            characterBaseVisualData.Bind(characterVisualData.hairColor);
            characterBaseVisualData.Bind(characterVisualData.guid);

            connector->Execute(characterBaseVisualData);
        }

        for (auto itr : _characterSpellStorageCache)
        {
            std::stringstream ss;
            bool first = true;

            ss << "DELETE FROM character_spell_storage WHERE guid={u} AND spell NOT IN (";
            for (auto itr2 : itr.second)
            {
                CharacterSpellStorage characterSpellStorage = itr2.second;
                PreparedStatement characterSpellStorageData("INSERT IGNORE INTO character_spell_storage(guid, spell) VALUES({u}, {u});");
                characterSpellStorageData.Bind(itr.first);
                characterSpellStorageData.Bind(characterSpellStorage.id);

                connector->Execute(characterSpellStorageData);

                if (!first)
                {
                    ss << ", " << characterSpellStorage.id;
                }
                else
                {
                    ss << characterSpellStorage.id;
                    first = false;
                }
            }
            ss << ");";

            PreparedStatement characterSpellStorageData(ss.str());
            characterSpellStorageData.Bind(itr.first);
            connector->Execute(characterSpellStorageData);
        }

        for (auto itr : _characterSkillStorageCache)
        {
            std::stringstream ss;
            bool first = true;

            ss << "DELETE FROM character_skill_storage WHERE guid={u} AND skill NOT IN (";
            for (auto itr2 : itr.second)
            {
                CharacterSkillStorage characterSkillStorage = itr2.second;
                PreparedStatement characterSkillStorageData("INSERT INTO character_skill_storage(guid, skill, value, character_skill_storage.maxValue) VALUES({u}, {u}, {u}, {u}) ON DUPLICATE KEY UPDATE value={u}, character_skill_storage.maxValue={u};");
                characterSkillStorageData.Bind(itr.first);
                characterSkillStorageData.Bind(characterSkillStorage.id);
                characterSkillStorageData.Bind(characterSkillStorage.value);
                characterSkillStorageData.Bind(characterSkillStorage.maxValue);
                characterSkillStorageData.Bind(characterSkillStorage.value);
                characterSkillStorageData.Bind(characterSkillStorage.maxValue);

                connector->Execute(characterSkillStorageData);

                if (!first)
                {
                    ss << ", " << characterSkillStorage.id;
                }
                else
                {
                    ss << characterSkillStorage.id;
                    first = false;
                }
            }
            ss << ");";

            PreparedStatement characterSkillStorageData(ss.str());
            characterSkillStorageData.Bind(itr.first);
            connector->Execute(characterSkillStorageData);
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
    DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, [this, characterGuid](std::shared_ptr<DatabaseConnector>& connector) {
        CharacterInfo characterInfo = _characterInfoCache[characterGuid];
        PreparedStatement characterBaseData("UPDATE characters SET level={u}, mapId={u}, zoneId={u}, coordinate_x={f}, coordinate_y={f}, coordinate_z={f}, orientation={f}, online={u} WHERE guid={u};");
        characterBaseData.Bind(characterInfo.level);
        characterBaseData.Bind(characterInfo.mapId);
        characterBaseData.Bind(characterInfo.zoneId);
        characterBaseData.Bind(characterInfo.position.x);
        characterBaseData.Bind(characterInfo.position.y);
        characterBaseData.Bind(characterInfo.position.z);
        characterBaseData.Bind(characterInfo.orientation);
        characterBaseData.Bind(characterInfo.online);
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

        // Save Spells
        {
            std::stringstream ss;
            bool first = true;

            ss << "DELETE FROM character_spell_storage WHERE guid={u} AND spell NOT IN (";
            for (auto itr : _characterSpellStorageCache[characterGuid])
            {
                CharacterSpellStorage characterSpellStorage = itr.second;
                PreparedStatement characterSpellStorageData("INSERT IGNORE INTO character_spell_storage(guid, spell) VALUES({u}, {u});");
                characterSpellStorageData.Bind(characterGuid);
                characterSpellStorageData.Bind(characterSpellStorage.id);
                connector->Execute(characterSpellStorageData);

                if (!first)
                {
                    ss << ", " << characterSpellStorage.id;
                }
                else
                {
                    ss << characterSpellStorage.id;
                    first = false;
                }
            }
            ss << ");";

            PreparedStatement characterSpellStorageData(ss.str());
            characterSpellStorageData.Bind(characterGuid);
            connector->Execute(characterSpellStorageData);
        }

        // Save Skills
        {
            std::stringstream ss;
            bool first = true;

            ss << "DELETE FROM character_skill_storage WHERE guid={u} AND skill NOT IN (";
            for (auto itr2 : _characterSkillStorageCache[characterGuid])
            {
                CharacterSkillStorage characterSkillStorage = itr2.second;
                PreparedStatement characterSkillStorageData("INSERT INTO character_skill_storage(guid, skill, value, character_skill_storage.maxValue) VALUES({u}, {u}, {u}, {u}) ON DUPLICATE KEY UPDATE value={u}, character_skill_storage.maxValue={u};");
                characterSkillStorageData.Bind(characterGuid);
                characterSkillStorageData.Bind(characterSkillStorage.id);
                characterSkillStorageData.Bind(characterSkillStorage.value);
                characterSkillStorageData.Bind(characterSkillStorage.maxValue);
                characterSkillStorageData.Bind(characterSkillStorage.value);
                characterSkillStorageData.Bind(characterSkillStorage.maxValue);
                connector->Execute(characterSkillStorageData);

                if (!first)
                {
                    ss << ", " << characterSkillStorage.id;
                }
                else
                {
                    ss << characterSkillStorage.id;
                    first = false;
                }
            }
            ss << ");";

            PreparedStatement characterSkillStorageData(ss.str());
            characterSkillStorageData.Bind(characterGuid);
            connector->Execute(characterSkillStorageData);
        }
    });
}
void CharacterDatabaseCache::UnloadCharacter(u64 characterGuid)
{
    _characterInfoCache.erase(characterGuid);
    _characterVisualDataCache.erase(characterGuid);
    _characterSpellStorageCache.erase(characterGuid);
    _characterSkillStorageCache.erase(characterGuid);
    _characterDataCache.erase(characterGuid);
}

bool CharacterDatabaseCache::GetCharacterInfo(u64 characterGuid, CharacterInfo& output)
{
    auto cache = _characterInfoCache.find(characterGuid);
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
        stmt.Bind(characterGuid);

        amy::result_set resultSet;
        connector->Query(stmt, resultSet);

        if (resultSet.affected_rows() == 0)
            return false;

        CharacterInfo newCharacterInfo(this);
        amy::row resultRow = resultSet[0];

        newCharacterInfo.guid = resultRow[0].GetU32();
        newCharacterInfo.account = resultRow[1].GetU32();

        newCharacterInfo.name = resultRow[2].GetString();

        newCharacterInfo.race = resultRow[3].GetU8();
        newCharacterInfo.gender = resultRow[4].GetU8();
        newCharacterInfo.classId = resultRow[5].GetU8();
        newCharacterInfo.level = resultRow[6].GetU8();
        newCharacterInfo.mapId = resultRow[7].GetU32();
        newCharacterInfo.zoneId = resultRow[8].GetU32();
        newCharacterInfo.position.x = resultRow[9].GetF32();
        newCharacterInfo.position.y = resultRow[10].GetF32();
        newCharacterInfo.position.z = resultRow[11].GetF32();
        newCharacterInfo.orientation = resultRow[12].GetF32();

        _accessMutex.lock();
        _characterInfoCache[characterGuid] = newCharacterInfo;
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
    _characterDataCache.insert({characterGuid, newCharacterData});
    _accessMutex.unlock();

    output = newCharacterData;
    return true;
}
bool CharacterDatabaseCache::GetCharacterVisualData(u64 characterGuid, CharacterVisualData& output)
{
    auto cache = _characterVisualDataCache.find(characterGuid);
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
        stmt.Bind(characterGuid);

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
        _characterVisualDataCache[characterGuid] = newCharacterVisualData;
        _accessMutex.unlock();

        output = newCharacterVisualData;
        return true;
    }
}
bool CharacterDatabaseCache::GetCharacterSpellStorage(u64 characterGuid, robin_hood::unordered_map<u32, CharacterSpellStorage>& output)
{
    auto cache = _characterSpellStorageCache.find(characterGuid);
    if (cache != _characterSpellStorageCache.end())
    {
        _accessMutex.lock_shared();
        robin_hood::unordered_map<u32, CharacterSpellStorage> characterSpellStorageData = cache->second;
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
        if (!result)
            return false;

        PreparedStatement stmt("SELECT guid, spell FROM character_spell_storage WHERE guid = {u};");
        stmt.Bind(characterGuid);

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

            _characterSpellStorageCache[guid][newCharacterSpellStorage.id] = newCharacterSpellStorage;
        }
        _accessMutex.unlock();

        output = _characterSpellStorageCache[characterGuid];
        return true;
    }
}
bool CharacterDatabaseCache::GetCharacterSkillStorage(u64 characterGuid, robin_hood::unordered_map<u32, CharacterSkillStorage>& output)
{
    auto cache = _characterSkillStorageCache.find(characterGuid);
    if (cache != _characterSkillStorageCache.end())
    {
        _accessMutex.lock_shared();
        robin_hood::unordered_map<u32, CharacterSkillStorage> characterSkillStorageData = cache->second;
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
        if (!result)
            return false;

        PreparedStatement stmt("SELECT guid, skill, value, character_skill_storage.maxValue FROM character_skill_storage WHERE guid = {u};");
        stmt.Bind(characterGuid);

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

            _characterSkillStorageCache[guid][newCharacterSkillStorage.id] = newCharacterSkillStorage;
        }
        _accessMutex.unlock();

        output = _characterSkillStorageCache[characterGuid];
        return true;
    }
}
bool CharacterDatabaseCache::GetCharacterItemData(u64 characterGuid, robin_hood::unordered_map<u32, CharacterItemData>& output)
{
    auto cache = _characteritemDataCache.find(characterGuid);
    if (cache != _characteritemDataCache.end())
    {
        _accessMutex.lock_shared();
        robin_hood::unordered_map<u32, CharacterItemData> characterItemData = cache->second;
        _accessMutex.unlock_shared();

        output = characterItemData;
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

        PreparedStatement stmt("SELECT lowGuid, itemEntry, bagSlot, bagPosition, characterGuid FROM character_items WHERE characterGuid = {u};");
        stmt.Bind(characterGuid);

        amy::result_set resultSet;
        connector->Query(stmt, resultSet);

        if (resultSet.affected_rows() == 0)
            return false;

        _accessMutex.lock();
        for (auto row : resultSet)
        {
            CharacterItemData newCharacterItemData(this);
            newCharacterItemData.lowGuid = row[0].GetU32();
            newCharacterItemData.itemEntry = row[1].GetU32();
            newCharacterItemData.bagSlot = row[2].GetU8();
            newCharacterItemData.bagPosition = row[3].GetU32();
            newCharacterItemData.characterGuid = row[4].GetU32();

            _characteritemDataCache[newCharacterItemData.characterGuid][newCharacterItemData.lowGuid] = newCharacterItemData;
        }
        _accessMutex.unlock();

        output = _characteritemDataCache[characterGuid];
        return true;
    }
}

void CharacterInfo::UpdateCache(u64 characterGuid)
{
    _cache->_characterInfoCache[characterGuid] = *this;
}
void CharacterData::UpdateCache()
{
    loaded = true;
    _cache->_characterDataCache[guid][type] = *this;
    _cache->Save();
}