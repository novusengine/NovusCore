/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/
#pragma once
#include <NovusTypes.h>
#include <Networking/ByteBuffer.h>
#include "ADTStructs.h"
#include "../MPQ/MPQFile.h"

#define NOVUSMAP_TOKEN 1313685840
#define NOVUSMAP_VERSION 808464433

#pragma pack(push, 1)
struct NovusHoleHeader
{
    NovusHoleHeader() : hasHoleData(false) {}

    u8 hasHoleData;
};

struct NovusLiquidHeader
{
    NovusLiquidHeader() : hasLiquidData(false), hasMultipleLiquidTypes(false), offsetX(255), offsetY(255), width(0), height(0), level(20000) {}

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
    NovusHeightHeader() : hasHeightBox(false), gridHeight(20000), gridMaxHeight(-20000) {}

    u8 hasHeightBox;
    f32 gridHeight;
    f32 gridMaxHeight;
};

struct NovusAreaHeader
{
    NovusAreaHeader() : hasSubArea(false), areaId(0) {}

    u8 hasSubArea;
    u16 areaId;
};

struct NovusAdtHeader
{
    NovusAdtHeader() : token(NOVUSMAP_TOKEN), version(NOVUSMAP_VERSION) {}

    u32 token;
    u32 version;
};
#pragma pack(pop)

class ADT
{
public:
    ADT(MPQFile& file, std::string fileName, std::string filePath);
    void Convert();
    u8 GetLiquidIdFromType(u16 type);

    MVER mver;
    MHDR mhdr;
    MCIN mcin;
    MH2O mh2o;
    MFBO mfbo;
private:
    // Grid Data Storage
    u16 areaIds[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];
    f32 heightMap[ADT_CELLS_PER_GRID * ADT_CELLS_PER_GRID][(ADT_CELL_SIZE + 1) * (ADT_CELL_SIZE + 1) + ADT_CELL_SIZE * ADT_CELL_SIZE];

    i16 heightBoxMax[3][3];
    i16 heightBoxMin[3][3];

    MPQFile & _file;
    std::string _fileName;
    std::string _filePath;
};