#include "MapLoader.h"
#include <Networking/ByteBuffer.h>
#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>
#include <filesystem>

#include "../ECS/Components/Singletons/MapSingleton.h"

bool MapLoader::Load(entt::registry& registry)
{
    size_t test = sizeof(NovusAdt);
    std::filesystem::path absolutePath = std::filesystem::absolute("maps");
    if (!std::filesystem::is_directory(absolutePath))
    {
        NC_LOG_ERROR("Failed to find maps folder");
        return false;
    }

    MapSingleton& mapSingleton = registry.set<MapSingleton>();

    mapSingleton.maps[0].mapName = "Eastern Kingdoms";
    mapSingleton.maps[0].id = 0;

    size_t loadedAdts = 0;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(absolutePath))
    {
        auto file = std::filesystem::path(entry.path());
        if (file.extension() != ".nmap")
            continue;

        FileReader adtFile(entry.path().string(), file.filename().string());
        if (!adtFile.Open())
        {
            NC_LOG_ERROR("Failed to load all maps");
            return false;
        }

        NovusAdt adt;
        if (!ExtractAdtInfo(adtFile, adt))
        {
            NC_LOG_ERROR("Failed to load all maps");
            return false;
        }

        std::vector<std::string> splitName = StringUtils::SplitString(file.filename().string(), '_');
        size_t numberOfSplits = splitName.size();

        u16 x = std::stoi(splitName[numberOfSplits - 2]);
        u16 y = std::stoi(splitName[numberOfSplits - 1]);

        int id = x + (y * blockStride);
        // TODO: Find actual mapID instead of putting everything inside of map 0, but this requires DBC data that we haven't extracted yet.
        mapSingleton.maps[0].adts[id] = adt;
        loadedAdts++;
    }

    if (loadedAdts == 0)
    {
        NC_LOG_ERROR("0 maps found in maps directory");
        return false;
    }

    NC_LOG_SUCCESS("Loaded %u ADTs", loadedAdts);
    return true;
}

bool MapLoader::ExtractAdtInfo(FileReader& reader, NovusAdt& adt)
{
    Common::ByteBuffer buffer;
    buffer.Resize(reader.Length());
    reader.Read(buffer, reader.Length());

    buffer.Read<NovusAdtHeader>(adt.adtHeader);

    buffer.Read<NovusAreaHeader>(adt.areaHeader);
    if (adt.areaHeader.hasSubArea)
    {
        buffer.Read<NovusAdtAreaIds>(adt.areaIds);
    }

    buffer.Read<NovusHeightHeader>(adt.heightHeader);
    if (adt.heightHeader.hasHeightBox)
    {
        buffer.Read<NovusAdtHeightLimit>(adt.heightLimit);
    }

    return true;
}