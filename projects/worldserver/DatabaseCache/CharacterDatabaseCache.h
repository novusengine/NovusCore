#pragma once
#include "BaseDatabaseCache.h"
#include <robin_hood.h>

// characters table in DB
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
        assert(!_isReadOnly);
    }
private:
    CharacterDatabaseCache* _cache;
    bool _isReadOnly;
};
// character_visual_data table in DB
struct CharacterVisualData
{
    CharacterVisualData(CharacterDatabaseCache* cache, bool isReadOnly) { _cache = cache; _isReadOnly = isReadOnly; }
    CharacterVisualData(CharacterVisualData& data, bool isReadOnly)
    {
        guid = data.guid;
        skin = data.skin;
        face = data.face;
        facialStyle = data.facialStyle;
        hairStyle = data.hairStyle;
        hairColor = data.hairColor;
        _cache = data._cache;
        _isReadOnly = isReadOnly;
    }

    u64 guid;
    u8 skin;
    u8 face;
    u8 facialStyle;
    u8 hairStyle;
    u8 hairColor;

    void UpdateCache()
    {
        assert(!_isReadOnly);
    }
private:
    CharacterDatabaseCache* _cache;
    bool _isReadOnly;
};
// character_spell_storage table in DB
struct CharacterSpellStorage
{
    CharacterSpellStorage(CharacterDatabaseCache* cache, bool isReadOnly) { _cache = cache; _isReadOnly = isReadOnly; }
    CharacterSpellStorage(CharacterSpellStorage& data, bool isReadOnly)
    {
        id = data.id;
        _isReadOnly = isReadOnly;
    }

    u32 id;

    void UpdateCache()
    {
        assert(!_isReadOnly);
    }
private:
    CharacterDatabaseCache* _cache;
    bool _isReadOnly;
};
// character_skill_storage table in DB
struct CharacterSkillStorage
{
    CharacterSkillStorage(CharacterDatabaseCache* cache, bool isReadOnly) { _cache = cache; _isReadOnly = isReadOnly; }
    CharacterSkillStorage(CharacterSkillStorage& data, bool isReadOnly)
    {
        id = data.id;
        value = data.value;
        maxValue = data.maxValue;
        _isReadOnly = isReadOnly;
    }

    u16 id;
    u16 value;
    u16 maxValue;

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
    CharacterData GetCharacterData(u64 guid);
    const CharacterData GetCharacterDataReadOnly(u64 guid);

    // Character Visual cache
    CharacterVisualData GetCharacterVisualData(u64 guid);
    const CharacterVisualData GetCharacterVisualDataReadOnly(u64 guid);

    // Character Spell Storage cache
    std::vector<CharacterSpellStorage> GetCharacterSpellStorage(u64 guid);
    const std::vector<CharacterSpellStorage> GetCharacterSpellStorageReadOnly(u64 guid);

    // Character Skill Storage cache
    std::vector<CharacterSkillStorage> GetCharacterSkillStorage(u64 guid);
    const std::vector<CharacterSkillStorage> GetCharacterSkillStorageReadOnly(u64 guid);

private:
    robin_hood::unordered_map<u64, CharacterData> _characterDataCache;
    robin_hood::unordered_map<u64, CharacterVisualData> _characterVisualDataCache;
    robin_hood::unordered_map<u64, std::vector<CharacterSpellStorage>> _characterSpellStorageCache;
    robin_hood::unordered_map<u64, std::vector<CharacterSkillStorage>> _characterSkillStorageCache;
};