#pragma once
#include "BaseDatabaseCache.h"
#include <vector>
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

    void UpdateCache()
    {
    }
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

    void UpdateCache()
    {
    }
private:
    CharacterDatabaseCache* _cache;
};
// character_spell_storage table in DB
struct DefaultSpellStorage
{
    DefaultSpellStorage() { }
    DefaultSpellStorage(CharacterDatabaseCache* cache) { _cache = cache; }
    DefaultSpellStorage(const DefaultSpellStorage& data)
    {
        raceMask = data.raceMask;
        classMask = data.classMask;
        id = data.id;
        _cache = data._cache;
    }

    i16 raceMask;
    i16 classMask;
    u32 id;

    void UpdateCache()
    {
    }
private:
    CharacterDatabaseCache* _cache;
};
// character_skill_storage table in DB
struct DefaultSkillStorage
{
    DefaultSkillStorage() { }
    DefaultSkillStorage(CharacterDatabaseCache* cache) { _cache = cache; }
    DefaultSkillStorage(const DefaultSkillStorage& data)
    {
        raceMask = data.raceMask;
        classMask = data.classMask;
        id = data.id;
        value = data.value;
        maxValue = data.maxValue;
        _cache = data._cache;
    }

    i16 raceMask;
    i16 classMask;
    u16 id;
    u16 value;
    u16 maxValue;

    void UpdateCache()
    {
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

    // Character cache
    bool GetCharacterData(u64 guid, CharacterData& output);

    // Character Visual cache
    bool GetCharacterVisualData(u64 guid, CharacterVisualData& output);

    const std::vector<DefaultSpellStorage> GetDefaultSpellStorageData();
    const std::vector<DefaultSkillStorage> GetDefaultSkillStorageData();

private:
    robin_hood::unordered_map<u64, CharacterData> _characterDataCache;
    robin_hood::unordered_map<u64, CharacterVisualData> _characterVisualDataCache;
    std::vector<DefaultSpellStorage> _defaultSpellStorageCache;
    std::vector<DefaultSkillStorage> _defaultSkillStorageCache;
};