#pragma once
#include "BaseDatabaseCache.h"
#include <robin_hood.h>

// item_template table in DB
class DBCDatabaseCache;

struct MapData
{
	MapData() { }
	MapData(DBCDatabaseCache* cache) { _cache = cache; }
	MapData(const MapData& data)
    {
        id = data.id;
        internalName = data.internalName;
		instanceType = data.instanceType;
		flags = data.flags;
		name = data.name;
		expansion = data.expansion;
		maxPlayers = data.maxPlayers;
        _cache = data._cache;
    }
   
    u16 id;
	std::string internalName;
	u32 instanceType;
	u32 flags;
	std::string name;
	u32 expansion;
	u32 maxPlayers;
private:
	DBCDatabaseCache* _cache;
};

struct EmoteData
{
    EmoteData() { }
    EmoteData(DBCDatabaseCache* cache) { _cache = cache; }
    EmoteData(const EmoteData& data)
    {
        id = data.id;
        internalName = data.internalName;
        textEmoteId = data.textEmoteId;
    }

    u32 id;
    std::string internalName;
    u32 textEmoteId;
private:
    DBCDatabaseCache* _cache;
};

class DBCDatabaseCache : BaseDatabaseCache
{
public:
	DBCDatabaseCache();
    ~DBCDatabaseCache();

    void Load() override;
    void LoadAsync() override;
    void Save() override;
    void SaveAsync() override;

    // Map Data cache
	bool GetMapData(u16 mapId, MapData& output);
	bool GetMapDataFromInternalName(std::string mapInternalName, MapData& output);

    bool GetEmoteDataFromTextEmoteId(u32 textEmoteId, EmoteData& output);

private:
    friend MapData;

    robin_hood::unordered_map<u32, MapData> _mapDataCache;

    robin_hood::unordered_map<u32, EmoteData> _emoteDataCache;
};