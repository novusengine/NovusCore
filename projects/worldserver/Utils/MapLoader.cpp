#include "MapLoader.h"
#include <Networking/ByteBuffer.h>
#include <Utils\DebugHandler.h>
#include <filesystem>

bool MapLoader::Load()
{
    size_t test = sizeof(NovusAdt);
    std::filesystem::path absolutePath = std::filesystem::absolute("maps");
    if (!std::filesystem::is_directory(absolutePath)) { NC_LOG_ERROR("Failed to find maps folder"); return false; }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(absolutePath))
    {
        auto file = std::filesystem::path(entry.path());
        if (file.extension() == ".nmap")
        {
            FileReader adtFile(entry.path().string(), file.filename().string());
            if (!adtFile.Open()) { NC_LOG_ERROR("Failed to load all maps"); return false; }
            
            NovusAdt adt;
            if (!ExtractAdtInfo(adtFile, adt)) { NC_LOG_ERROR("Failed to load all maps"); return false; }

            _adts.push_back(adt);
        }
    }

    if (_adts.size() == 0) { NC_LOG_ERROR("0 maps found in maps directory"); return false; }

    NC_LOG_SUCCESS("Loaded %u ADTs", _adts.size());
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

    buffer.Read<NovusLiquidHeader>(adt.liquidHeader);
    if (adt.liquidHeader.hasLiquidData)
    {
        if (adt.liquidHeader.hasMultipleLiquidTypes)
        {
            buffer.Read<NovusAdtLiquidData>(adt.liquidData);
        }
        
        buffer.Read((void*)&adt.liquidHeight, 4 * adt.liquidHeader.width * adt.liquidHeader.height);
    }

    buffer.Read<NovusHoleHeader>(adt.holeHeader);
    if (adt.holeHeader.hasHoleData)
    {
        buffer.Read<NovusAdtHolesData>(adt.holesData);
    }

    return true;
}