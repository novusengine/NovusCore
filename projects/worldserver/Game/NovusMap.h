/*
	MIT License

	Copyright (c) 2018-2019 NovusCore

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
#pragma once
#include <NovusTypes.h>
#include <robin_hood.h>
#include <Math/Vector2.h>

#define NOVUSADT_TOKEN 1313685840
#define NOVUSADT_VERSION 808464433

const u32 blockStride = 64;

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

	f32 heightData[ADT_CELLS_PER_GRID * ADT_CELLS_PER_GRID][(ADT_CELL_SIZE + 1) * (ADT_CELL_SIZE + 1) + ADT_CELL_SIZE * ADT_CELL_SIZE];
};

struct NovusAreaHeader
{
	NovusAreaHeader() : hasSubArea(0), areaId(0) { }

	u8 hasSubArea;
	u16 areaId;
};

struct NovusAdtHeader
{
	NovusAdtHeader() : token(0), version(0) { }

	u32 token;
	u32 version;
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
	NovusAdt() {};
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

struct NovusMap
{
	NovusMap(){}
	u16 id;
	std::string mapName;
	robin_hood::unordered_map<u16, NovusAdt> adts;

	f32 GetHeight(Vector2& pos);
};