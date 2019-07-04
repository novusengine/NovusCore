#pragma once
#include "BaseDatabaseCache.h"
#include <vector>
#include <robin_hood.h>

// characters table in DB
class CharacterDatabaseCache;
struct CharacterInfo
{
    CharacterInfo() {}
    CharacterInfo(CharacterDatabaseCache* cache) { _cache = cache; }
    CharacterInfo(const CharacterInfo& data)
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
// character_data table in DB
struct CharacterData
{
    CharacterData() {}
    CharacterData(CharacterDatabaseCache* cache) { _cache = cache; }
    CharacterData(const CharacterData& inData)
    {
        guid = inData.guid;
        type = inData.type;
        timestamp = inData.timestamp;
        data = inData.data;
        loaded = inData.loaded;
        _cache = inData._cache;
    }

    u64 guid;
    u32 type;
    u32 timestamp;
    std::string data;

    bool loaded = false;
    void UpdateCache();

private:
    CharacterDatabaseCache* _cache;
};
// character_visual_data table in DB
struct CharacterVisualData
{
    CharacterVisualData() {}
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
// default_spells table in DB
struct DefaultSpellStorage
{
    DefaultSpellStorage() {}
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
// default_skills table in DB
struct DefaultSkillStorage
{
    DefaultSkillStorage() {}
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
// default_spawns table in DB
struct DefaultSpawnStorage
{
    DefaultSpawnStorage() {}
    DefaultSpawnStorage(CharacterDatabaseCache* cache) { _cache = cache; }
    DefaultSpawnStorage(const DefaultSpawnStorage& data)
    {
        raceMask = data.raceMask;
        classMask = data.classMask;
        mapId = data.mapId;
        zoneId = data.zoneId;
        coordinate_x = data.coordinate_x;
        coordinate_y = data.coordinate_y;
        coordinate_z = data.coordinate_z;
        orientation = data.orientation;
        _cache = data._cache;
    }

    i16 raceMask;
    i16 classMask;
    u16 mapId;
    u16 zoneId;
    f32 coordinate_x;
    f32 coordinate_y;
    f32 coordinate_z;
    f32 orientation;

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

    void SaveAndUnloadCharacter(u64 characterGuid);
    void SaveCharacter(u64 characterGuid);
    void UnloadCharacter(u64 characterGuid);

    // Character Info cache
    bool GetCharacterInfo(u64 guid, CharacterInfo& output);

    // Character Data cache
    bool GetCharacterData(u64 characterGuid, u32 type, CharacterData& output);

    // Character Visual cache
    bool GetCharacterVisualData(u64 guid, CharacterVisualData& output);

    const std::vector<DefaultSpellStorage> GetDefaultSpellStorageData();
    const std::vector<DefaultSkillStorage> GetDefaultSkillStorageData();
    const std::vector<DefaultSpawnStorage> GetDefaultSpawnStorageData();

private:
    friend CharacterData;

    robin_hood::unordered_map<u64, CharacterInfo> _characterInfoCache;             // Character Guid
    robin_hood::unordered_map<u64, CharacterData[8]> _characterDataCache;          // Character Guid, DataCache Type
    robin_hood::unordered_map<u64, CharacterVisualData> _characterVisualDataCache; // Character Guid
    std::vector<DefaultSpellStorage> _defaultSpellStorageCache;
    std::vector<DefaultSkillStorage> _defaultSkillStorageCache;
    std::vector<DefaultSpawnStorage> _defaultSpawnStorageCache;
};