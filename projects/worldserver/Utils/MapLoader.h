#pragma once
#include <NovusTypes.h>
#include <Utils/FileReader.h>
#include <vector>

#define NOVUSADT_TOKEN 1313685840
#define NOVUSADT_VERSION 808464433

#define ADT_CELLS_PER_GRID    16
#define ADT_CELL_SIZE         8
#define ADT_GRID_SIZE         (ADT_CELLS_PER_GRID*ADT_CELL_SIZE)

#pragma pack(push, 1)
struct NovusHoleHeader
{
    NovusHoleHeader() : hasHoleData(0) { }

    u8 hasHoleData;
};

struct NovusLiquidHeader
{
    NovusLiquidHeader() : hasLiquidData(0), hasMultipleLiquidTypes(0), offsetX(0), offsetY(0), width(0), height(0), level(0) { }

    u8 hasLiquidData;
    u8 hasMultipleLiquidTypes;
    u8 offsetX;
    u8 offsetY;
    u8 width;
    u8 height;
    u8 liquidFlags;
    u16 liquidEntry;
    f32 level;
};

struct NovusHeightHeader
{
    NovusHeightHeader() : hasHeightBox(0), gridHeight(0), gridMaxHeight(0) { }

    u8 hasHeightBox;
    f32 gridHeight;
    f32 gridMaxHeight;

    f32 V8[ADT_GRID_SIZE][ADT_GRID_SIZE];
    f32 V9[ADT_GRID_SIZE + 1][ADT_GRID_SIZE + 1];
};

struct NovusAreaHeader
{
    NovusAreaHeader() : hasSubArea(0), areaId(0) { }

    u8 hasSubArea;
    u16 areaId;
};

struct NovusAdtHeader
{
    NovusAdtHeader() : token(0), version(0), areaSize(0), heightSize(0), liquidSize(0), holeSize(0) { }

    u32 token;
    u32 version;
    u32 areaSize;
    u32 heightSize;
    u32 liquidSize;
    u32 holeSize;
};

struct NovusAdtAreaIds
{
    NovusAdtAreaIds() : ids() { }

    u16 ids[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];
};

struct NovusAdtHeightLimit
{
    NovusAdtHeightLimit() : limitBoxMax(), limitBoxMin() { }

    i16 limitBoxMax[3][3];
    i16 limitBoxMin[3][3];
};

struct NovusAdtLiquidData
{
    NovusAdtLiquidData() : liquidEntry(), liquidFlags() { }

    u16 liquidEntry[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];
    u8  liquidFlags[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];
};

struct NovusAdtLiquidHeight
{
    NovusAdtLiquidHeight() : liquidHeight() { }

    f32 liquidHeight[ADT_GRID_SIZE + 1][ADT_GRID_SIZE + 1];
};

struct NovusAdtHolesData
{
    NovusAdtHolesData() : holes() { }

    u16 holes[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];
};
struct NovusAdt
{
    NovusAdtHeader adtHeader;
    NovusAreaHeader areaHeader;
    NovusHeightHeader heightHeader;
    NovusLiquidHeader liquidHeader;
    NovusHoleHeader holeHeader;

    NovusAdtAreaIds areaIds;
    NovusAdtHeightLimit heightLimit;
    NovusAdtLiquidData liquidData;
    NovusAdtLiquidHeight liquidHeight; 
    NovusAdtHolesData holesData;
};
#pragma pack(pop)

class MapLoader
{
public:
    MapLoader() { }
    bool Load();

private:
    bool ExtractAdtInfo(FileReader& reader, NovusAdt& adt);
    std::vector<NovusAdt> _adts;
};