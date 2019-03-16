#pragma once
#include "BaseDatabaseCache.h"

// Characters table in DB
class CharacterDatabaseCache;
struct CharacterData
{
    CharacterData(CharacterDatabaseCache* cache, bool isReadOnly) { _cache = cache; _isReadOnly = isReadOnly; }
    CharacterData(CharacterData& data, bool isReadOnly)
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
        _isReadOnly = isReadOnly;
    }

    u32 guid;
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
        assert(!_isReadOnly);
    }
private:
    CharacterDatabaseCache* _cache;
    bool _isReadOnly;
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
    CharacterData GetCharacterData(u32 guid);
    const CharacterData GetCharacterDataReadOnly(u32 guid);

private:
    std::unordered_map<u32, CharacterData> _characterDataCache;
};