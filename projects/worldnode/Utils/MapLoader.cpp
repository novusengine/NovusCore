#include "MapLoader.h"
#include <Networking/DataStore.h>
#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>
#include <filesystem>

#include "../ECS/Components/Singletons/MapSingleton.h"
#include "../ECS/Components/Singletons/DBCDatabaseCacheSingleton.h"

bool MapLoader::Load(entt::registry& registry)
{
    //size_t test = sizeof(NovusAdt);
    std::filesystem::path absolutePath = std::filesystem::absolute("maps");
    if (!std::filesystem::is_directory(absolutePath)) { NC_LOG_ERROR("Failed to find maps folder"); return false; }

	MapSingleton& mapSingleton = registry.set<MapSingleton>();
	DBCDatabaseCacheSingleton& dbcCache = registry.ctx<DBCDatabaseCacheSingleton>();

    size_t loadedAdts = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(absolutePath))
    {
        auto file = std::filesystem::path(entry.path());
		if (file.extension() != ".nmap") continue;
        
        FileReader adtFile(entry.path().string(), file.filename().string());
        if (!adtFile.Open()) { NC_LOG_ERROR("Failed to load all maps"); return false; }
            
        NovusAdt adt;
        if (!ExtractAdtInfo(adtFile, adt)) { NC_LOG_ERROR("Failed to load all maps"); return false; }

		std::vector<std::string> splitName = StringUtils::SplitString(file.filename().string(), '_');
		size_t numberOfSplits = splitName.size();

		std::string mapInternalName = splitName[0];
		MapData mapData;
		if (!dbcCache.cache->GetMapDataFromInternalName(mapInternalName, mapData))
			continue;
		
		u16 mapId = mapData.id;
		
		if (mapSingleton.maps.find(mapId) == mapSingleton.maps.end())
		{
			mapSingleton.maps[mapId].id = mapId;
			mapSingleton.maps[mapId].mapName = mapData.name;
		}

		u16 x = std::stoi(splitName[numberOfSplits - 2]);
		u16 y = std::stoi(splitName[numberOfSplits - 1]);

		int chunkId = x + (y * blockStride);
		mapSingleton.maps[mapId].adts[chunkId] = adt;

		loadedAdts++;
    }

    if (loadedAdts == 0) { NC_LOG_ERROR("0 maps found in (%s)", absolutePath.string().c_str()); return false; }

    NC_LOG_SUCCESS("Loaded %u ADTs", loadedAdts);
    return true;
}

bool MapLoader::ExtractAdtInfo(FileReader& reader, NovusAdt& adt)
{
    DataStore buffer(nullptr, reader.Length());
    reader.Read(buffer, buffer.Size);

    buffer.Get<NovusAdtHeader>(adt.adtHeader);

    buffer.Get<NovusAreaHeader>(adt.areaHeader);
    if (adt.areaHeader.hasSubArea)
    {	
        buffer.Get<NovusAdtAreaIds>(adt.areaIds);
    }

    buffer.Get<NovusHeightHeader>(adt.heightHeader);
    if (adt.heightHeader.hasHeightBox)
    {
        buffer.Get<NovusAdtHeightLimit>(adt.heightLimit);
    }

    return true;
}