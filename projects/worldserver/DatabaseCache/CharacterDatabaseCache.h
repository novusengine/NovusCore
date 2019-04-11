#pragma once
#include "BaseDatabaseCache.h"
#include <robin_hood.h>

// characters table in DB
class CharacterDatabaseCache;
struct CharacterData
{
    CharacterData() { }
    CharacterData(CharacterDatabaseCache* cache) { _cache = cache; }
    CharacterData(const CharacterData& data)
    {
        guid = data.guid;
        account = data.account;
        name = data.name;
        race = data.race;
        gender = data.gender;
        classId = data.classId;
        level = data.level;
        mapId = data.mapId;
        zoneId = data.zoneId;
        coordinateX = data.coordinateX;
        coordinateY = data.coordinateY;
        coordinateZ = data.coordinateZ;
        orientation = data.orientation;
        _cache = data._cache;
    }

    u64 guid;
    u32 account;
    std::string name;
    u8 race;
    u8 gender;
    u8 classId;
    u8 level;
    u32 mapId;
    u32 zoneId;
    f32 coordinateX;
    f32 coordinateY;
    f32 coordinateZ;
    f32 orientation;

    void UpdateCache(u64 characterGuid);
private:
    CharacterDatabaseCache* _cache;
};
// character_visual_data table in DB
struct CharacterVisualData
{
    CharacterVisualData() { }
    CharacterVisualData(CharacterDatabaseCache* cache) { _cache = cache; }
    CharacterVisualData(const CharacterVisualData& data)
    {
        guid = data.guid;
        skin = data.skin;
        face = data.face;
        facialStyle = data.facialStyle;
        hairStyle = data.hairStyle;
        hairColor = data.hairColor;
        _cache = data._cache;
    }

    u64 guid;
    u8 skin;
    u8 face;
    u8 facialStyle;
    u8 hairStyle;
    u8 hairColor;

    void UpdateCache(u64 characterGuid)
    {
        //_cache->_characterVisualDataCache[characterGuid] = *this;
    }
private:
    CharacterDatabaseCache* _cache;
};
// character_spell_storage table in DB
struct CharacterSpellStorage
{
    CharacterSpellStorage() { }
    CharacterSpellStorage(CharacterDatabaseCache* cache) { _cache = cache; }
    CharacterSpellStorage(const CharacterSpellStorage& data)
    {
        id = data.id;
        _cache = data._cache;
    }

    u32 id;

    void UpdateCache(u64 characterGuid)
    {
        //_cache->_characterSpellStorageCache[characterGuid][id] = *this;
    }
    void EraseCache(u64 characterGuid)
    {
        //_cache->_characterSpellStorageCache[characterGuid].erase(id);
    }
private:
    CharacterDatabaseCache* _cache;
};
// character_skill_storage table in DB
struct CharacterSkillStorage
{
    CharacterSkillStorage() { }
    CharacterSkillStorage(CharacterDatabaseCache* cache) { _cache = cache; }
    CharacterSkillStorage(const CharacterSkillStorage& data)
    {
        id = data.id;
        value = data.value;
        maxValue = data.maxValue;
        _cache = data._cache;
    }

    u16 id;
    u16 value;
    u16 maxValue;

    void UpdateCache(u64 characterGuid)
    {
        //_cache->_characterSkillStorageCache[characterGuid][id] = *this;
    }
    void EraseCache(u64 characterGuid)
    {
        //_cache->_characterSkillStorageCache[characterGuid].erase(id);
    }
private:
    CharacterDatabaseCache* _cache;
};

class CharacterDatabaseCache : BaseDatabaseCache
{
public:
    CharacterDatabaseCache();
    ~CharacterDatabaseCache();

    void Load() override;
    void LoadAsync() override;
    void Save() override;
    void SaveAsync() override;
    
    void SaveAndUnloadCharacter(u64 characterGuid);
    void SaveCharacter(u64 characterGuid);
    void UnloadCharacter(u64 characterGuid);

    // Character cache
    bool GetCharacterData(u64 characterGuid, CharacterData& output);

    // Character Visual cache
    bool GetCharacterVisualData(u64 characterGuid, CharacterVisualData& output);

    // Character Spell Storage cache
    bool GetCharacterSpellStorage(u64 characterGuid, robin_hood::unordered_map<u32, CharacterSpellStorage>& output);

    // Character Skill Storage cache
    bool GetCharacterSkillStorage(u64 characterGuid, robin_hood::unordered_map<u32, CharacterSkillStorage>& output);

private:
    friend CharacterData;
    friend CharacterVisualData;
    friend CharacterSpellStorage;
    friend CharacterSkillStorage;

    robin_hood::unordered_map<u64, CharacterData> _characterDataCache; // Character Guid
    robin_hood::unordered_map<u64, CharacterVisualData> _characterVisualDataCache; // Character Guid
    robin_hood::unordered_map<u64, robin_hood::unordered_map<u32, CharacterSpellStorage>> _characterSpellStorageCache; // Character Guid, Spell Id
    robin_hood::unordered_map<u64, robin_hood::unordered_map<u32, CharacterSkillStorage>> _characterSkillStorageCache; // Character Guid, Skill Id
};