#include "DBCDatabaseCache.h"
#include <Database/DatabaseConnector.h>
#include <Database/PreparedStatement.h>

DBCDatabaseCache::DBCDatabaseCache()
{
}
DBCDatabaseCache::~DBCDatabaseCache()
{
}

void DBCDatabaseCache::Load()
{
    std::shared_ptr<DatabaseConnector> connector;
    bool result = DatabaseConnector::Borrow(DATABASE_TYPE::DBC, connector);
    assert(result);
    if (!result)
        return;

    amy::result_set resultSet;
    /* Map Data*/
    connector->Query("SELECT * FROM map;", resultSet);

    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            MapData mapData(this);
            mapData.id = row[0].GetU16();
            mapData.internalName = row[1].GetString();
            mapData.instanceType = row[2].GetU32();
            mapData.flags = row[3].GetU32();
            mapData.name = row[4].GetString();
            mapData.expansion = row[5].GetU32();
            mapData.maxPlayers = row[6].GetU32();
            
            _mapDataCache[mapData.id] = mapData;
        }
    }

    /* Emote Data*/
    connector->Query("SELECT * FROM emotes;", resultSet);
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            EmoteData emoteData(this);
            emoteData.id = row[0].GetU32();
            emoteData.internalName = row[1].GetString();
            emoteData.textEmoteId = row[2].GetU32();

            _emoteDataCache[emoteData.id] = emoteData;
        }
    }
}
void DBCDatabaseCache::LoadAsync()
{
}

void DBCDatabaseCache::Save()
{
}
void DBCDatabaseCache::SaveAsync()
{
}

bool DBCDatabaseCache::GetMapData(u16 mapId, MapData& output)
{
	auto cache = _mapDataCache.find(mapId);
	if (cache != _mapDataCache.end())
	{
		_accessMutex.lock_shared();
		MapData mapData = cache->second;
		_accessMutex.unlock_shared();

		output = mapData;
		return true;
	}

	return false;
}
bool DBCDatabaseCache::GetMapDataFromInternalName(std::string internalName, MapData& output)
{
	_accessMutex.lock_shared();
	for (auto mapData : _mapDataCache)
	{
		if (mapData.second.internalName == internalName)
		{
			output = mapData.second;
			return true;
		}
	}
	_accessMutex.unlock_shared();

	return false;
}


bool DBCDatabaseCache::GetEmoteDataFromTextEmoteId(u32 textEmoteId, EmoteData& output)
{
    _accessMutex.lock_shared();
    for (auto emoteData : _emoteDataCache)
    {
        if (emoteData.second.textEmoteId == textEmoteId)
        {
            output = emoteData.second;
            return true;
        }
    }
    _accessMutex.unlock_shared();

    return false;
}