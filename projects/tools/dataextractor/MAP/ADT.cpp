#include "ADT.h"
#include <fstream>

ADT::ADT(MPQFile& file, std::string fileName, std::string filePath) : _file(file), _fileName(fileName), _filePath(filePath)
{
    memset(areaIds, 0, sizeof(areaIds));
    memset(heightMap, 0, sizeof(heightMap));
}

void ADT::Convert()
{
    mver.Read(_file.Buffer);
    assert(mver.token == NOVUSMAP_MVER_TOKEN && mver.version == 18);

    mhdr.Read(_file.Buffer);
    assert(mhdr.token == NOVUSMAP_MHDR_TOKEN);

    if (!mcin.Read(_file.Buffer, mhdr.offsetMcin + 0x14))
    {
        return;
    }

    bool hasWater = false;
    if (mhdr.offsetMh2o)
    {
        if (mh2o.Read(_file.Buffer, mhdr.offsetMh2o + 0x14))
        {
            hasWater = true;
        }
        else
        {
            hasWater = false;
        }
    }

    bool hasHeightBox = false;
    if (mhdr.flags & 1)
    {
        if (!mfbo.Read(_file.Buffer, mhdr.offsetMfbo + 0x14))
        {
            hasHeightBox = true;
        }
        else
        {
            hasHeightBox = false;
        }
    }

    NovusAdtHeader adtHeader;
    NovusAreaHeader areaHeader;
    NovusHeightHeader heightHeader;

    // Read all chunks
    for (u32 i = 0; i < 256; i++)
    {
        u32 y = i / 16;
        u32 x = i % 16;

        MCNK mcnk;
        if (mcnk.Read(_file.Buffer, mcin.chunks[y][x]))
        {
            /* Handle AreaId */
            areaIds[y][x] = mcnk.areaId;
            if (areaHeader.areaId == 0)
            {
                areaHeader.areaId = mcnk.areaId;
            }
            else
            {
                if (!areaHeader.hasSubArea && areaHeader.areaId != mcnk.areaId)
                {
                    areaHeader.hasSubArea = true;
                }
            }

            /* Handle Heightmap */
            MCVT mcvt;
            if (mcvt.Read(_file.Buffer, mcnk.offsetMcvt + mcin.chunks[y][x]))
            {
                for (u32 j = 0; j < 145; j++)
                {
                    f32 height = mcnk.yPos + mcvt.heightMap[j];
                    heightMap[i][j] = height;

                    if (heightHeader.gridHeight > height)
                        heightHeader.gridHeight = height;
                    if (heightHeader.gridMaxHeight < height)
                        heightHeader.gridMaxHeight = height;
                }
            }
            else
            {
                for (u32 j = 0; j < 145; j++)
                {
                    f32 height = mcnk.yPos;
                    heightMap[i][j] = height;

                    if (heightHeader.gridHeight > height)
                        heightHeader.gridHeight = height;
                    if (heightHeader.gridMaxHeight < height)
                        heightHeader.gridMaxHeight = height;
                }
            }
        }
    }

    std::ofstream output(_filePath + "/" + _fileName, std::ofstream::out | std::ofstream::binary);
    if (!output)
    {
        printf("Failed to create map file. Check admin permissions\n");
        return;
    }

    // Write adtHeader, AreaHeader
    output.write(reinterpret_cast<char const*>(&adtHeader), sizeof(adtHeader));
    output.write(reinterpret_cast<char const*>(&areaHeader), sizeof(areaHeader));

    // Write AreaIds if area has sub area
    if (areaHeader.hasSubArea)
        output.write(reinterpret_cast<char const*>(&areaIds), sizeof(areaIds));

    // Write HeightHeader, Height Maps
    output.write(reinterpret_cast<char const*>(&heightHeader), sizeof(heightHeader));
    output.write(reinterpret_cast<char const*>(&heightMap), sizeof(heightMap));

    // Write Height Boxes if needed (HeightLimitBox)
    if (heightHeader.hasHeightBox)
    {
        output.write(reinterpret_cast<char*>(heightBoxMax), sizeof(heightBoxMax));
        output.write(reinterpret_cast<char*>(heightBoxMin), sizeof(heightBoxMin));
    }

    output.close();
}

u8 ADT::GetLiquidIdFromType(u16 type)
{
    switch (type)
    {
    case 1:
    case 5:
    case 9:
    case 13:
    case 17:
    case 41:
    case 61:
    case 81:
    case 181:
    {
        return 0;
    }
    case 2:
    case 6:
    case 10:
    case 14:
    case 100:
    {
        return 1;
    }
    case 3:
    case 7:
    case 11:
    case 15:
    case 19:
    case 121:
    case 141:
    {
        return 2;
    }
    case 4:
    case 8:
    case 12:
    case 20:
    case 21:
    {
        return 3;
    }
    default:
    {
        return 4;
    }
    }
}