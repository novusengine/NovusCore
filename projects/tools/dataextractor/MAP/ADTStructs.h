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
#include "../ByteBuffer.h"

#define ADT_CELLS_PER_GRID    16
#define ADT_CELL_SIZE         8
#define ADT_GRID_SIZE         (ADT_CELLS_PER_GRID*ADT_CELL_SIZE)

// ADT_CELL_SIZE (THIS REPRESENTS THE AMOUNT OF CELLS INSIDE A CHUNK; NOT THE SIZE OF A CELL) TY TC

#define NOVUSMAP_MVER_TOKEN 1297499474
#define NOVUSMAP_MHDR_TOKEN 1296581714
#define NOVUSMAP_MCIN_TOKEN 1296255310
#define NOVUSMAP_MCNK_TOKEN 1296256587
#define NOVUSMAP_MFBO_TOKEN 1296577103
#define NOVUSMAP_MH2O_TOKEN 1296577103
#define NOVUSMAP_MCVT_TOKEN 1296258644

enum LIQUID_ID
{
	LIQUID_ID_WATER = 0,
	LIQUID_ID_OCEAN = 1,
	LIQUID_ID_MAGMA = 2,
	LIQUID_ID_SLIME = 3
};
enum LIQUID_TYPE
{
	LIQUID_TYPE_NO_WATER = 0x00,
	LIQUID_TYPE_WATER = 0x01,
	LIQUID_TYPE_OCEAN = 0x02,
	LIQUID_TYPE_MAGMA = 0x04,
	LIQUID_TYPE_SLIME = 0x08,
	LIQUID_TYPE_DARK_WATER = 0x10
};

#define MAP_LIQUID_NO_TYPE    0x0001
#define MAP_LIQUID_NO_HEIGHT  0x0002

#pragma pack(push, 1)
struct MVER
{
	MVER() : token(0), size(0), version(0) { }

	u32 token;
	u32 size;
	u32 version;

	void Read(ByteBuffer& buffer)
	{
		buffer.Read<u32>(token);
		buffer.Read<u32>(size);
		buffer.Read<u32>(version);
	}
};

struct MCVT
{
	MCVT() : token(0), size(0), heightMap() { }

	u32 token;
	u32 size;
	f32 heightMap[(ADT_CELL_SIZE + 1) * (ADT_CELL_SIZE + 1) + ADT_CELL_SIZE * ADT_CELL_SIZE];

	bool Read(ByteBuffer& buffer, u32 offset)
	{
		token = buffer.ReadAt<u32>(offset);

		if (token == NOVUSMAP_MCVT_TOKEN)
		{
			size = buffer.ReadAt<u32>(offset + 0x4);

			for (u32 i = 0; i < 145; i++)
			{
				heightMap[i] = buffer.ReadAt<f32>(offset + 0x8 + (i * 4));
			}

			return true;
		}

		return false;
	}
};

struct MCLQ
{
	MCLQ() : liquid(), flags(), data() { }

	u32 token = 0;
	u32 size = 0;

	f32 heightMin = 0;
	f32 heightMax = 0;

	struct LIQUID
	{
		u32 light;
		f32 height;
	} liquid[ADT_CELL_SIZE + 1][ADT_CELL_SIZE + 1];

	// 1<<0 - ocean
	// 1<<1 - lava/slime
	// 1<<2 - water
	// 1<<6 - all water
	// 1<<7 - dark water
	// == 0x0F - not show liquid
	u8 flags[ADT_CELL_SIZE][ADT_CELL_SIZE];
	u8 data[84];
};

struct MCNK
{
	MCNK() : token(0), size(0), flags(0), ix(0), iy(0), layers(0), doodadRefs(0), offsetMcvt(0), offsetMcnr(0), offsetMcly(0),
		offsetMcrf(0), offsetMcal(0), sizeMcal(0), offsetMcsh(0), sizeMcsh(0), areaId(0), mapObjectReferences(0), holes(0),
		s(), predTex(0), effectDoodad(0), offsetMcse(0), soundEmitters(0), offsetMclq(0), sizeMclq(0), zPos(0), xPos(0), yPos(0),
		offsetMccv(0), props(0), effectId(0) { }

	u32 token;
	u32 size;
	u32 flags;
	u32 ix;
	u32 iy;
	u32 layers;
	u32 doodadRefs;
	u32 offsetMcvt;
	u32 offsetMcnr;
	u32 offsetMcly;
	u32 offsetMcrf;
	u32 offsetMcal;
	u32 sizeMcal;
	u32 offsetMcsh;
	u32 sizeMcsh;
	u32 areaId;
	u32 mapObjectReferences;
	u32 holes;
	u8 s[16];
	u32 predTex;
	u32 effectDoodad;
	u32 offsetMcse;
	u32 soundEmitters;
	u32 offsetMclq;
	u32 sizeMclq;
	f32 zPos;
	f32 xPos;
	f32 yPos;
	u32 offsetMccv;
	u32 props;
	u32 effectId;

	bool Read(ByteBuffer& buffer, u32 offset)
	{
		if (offset == 0)
			return false;

		token = buffer.ReadAt<u32>(offset);

		if (token == NOVUSMAP_MCNK_TOKEN)
		{
			size = buffer.ReadAt<u32>(offset + 0x4);
			flags = buffer.ReadAt<u32>(offset + 0x8);
			ix = buffer.ReadAt<u32>(offset + 0xC);
			iy = buffer.ReadAt<u32>(offset + 0x10);
			layers = buffer.ReadAt<u32>(offset + 0x14);
			doodadRefs = buffer.ReadAt<u32>(offset + 0x18);
			offsetMcvt = buffer.ReadAt<u32>(offset + 0x1C);        // height map
			offsetMcnr = buffer.ReadAt<u32>(offset + 0x20);        // Normal vectors for each vertex
			offsetMcly = buffer.ReadAt<u32>(offset + 0x24);        // Texture layer definitions
			offsetMcrf = buffer.ReadAt<u32>(offset + 0x28);        // A list of indices into the parent file's MDDF chunk
			offsetMcal = buffer.ReadAt<u32>(offset + 0x2C);        // Alpha maps for additional texture layers
			sizeMcal = buffer.ReadAt<u32>(offset + 0x30);
			offsetMcsh = buffer.ReadAt<u32>(offset + 0x34);        // Shadow map for static shadows on the terrain
			sizeMcsh = buffer.ReadAt<u32>(offset + 0x38);
			areaId = buffer.ReadAt<u32>(offset + 0x3C);
			mapObjectReferences = buffer.ReadAt<u32>(offset + 0x40);
			holes = buffer.ReadAt<u32>(offset + 0x44);

			for (u32 i = 1; i <= 16; i++)
			{
				s[i] = buffer.ReadAt<u8>(offset + 0x48 + i);
			}

			predTex = buffer.ReadAt<u32>(offset + 0x58);
			effectDoodad = buffer.ReadAt<u32>(offset + 0x5C);
			offsetMcse = buffer.ReadAt<u32>(offset + 0x60);
			soundEmitters = buffer.ReadAt<u32>(offset + 0x64);
			offsetMclq = buffer.ReadAt<u32>(offset + 0x68);         // Liqid level (old)
			sizeMclq = buffer.ReadAt<u32>(offset + 0x6C);         //
			zPos = buffer.ReadAt<f32>(offset + 0x70);
			xPos = buffer.ReadAt<f32>(offset + 0x74);
			yPos = buffer.ReadAt<f32>(offset + 0x78);
			offsetMccv = buffer.ReadAt<u32>(offset + 0x7C);         // offsColorValues in WotLK
			props = buffer.ReadAt<u32>(offset + 0x80);
			effectId = buffer.ReadAt<u32>(offset + 0x84);

			return true;
		}

		return false;
	}
};

struct MCIN
{
	MCIN() : token(0), size(0), chunks() { }

	u32 token;
	u32 size;
	u32 chunks[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];

	bool Read(ByteBuffer& buffer, u32 offset)
	{
		u32 baseAddress = offset;
		token = buffer.ReadAt<u32>(baseAddress);
		baseAddress += 0x04;

		if (token == NOVUSMAP_MCIN_TOKEN)
		{
			size = buffer.ReadAt<u32>(baseAddress);
			baseAddress += 0x04;

			for (u32 i = 0; i < 256; i++)
			{
				u32 y = i / 16;
				u32 x = i % 16;

				u32 chunkOffset = buffer.ReadAt<u32>(baseAddress);
				chunks[y][x] = chunkOffset < buffer.size() ? chunkOffset : 0;
				baseAddress += 0x10;
			}

			return true;
		}

		return false;
	}
};

struct MFBO
{
	MFBO() : token(0), size(0), max(), min() { }

	u32 token;
	u32 size;

	i16 max[9];
	i16 min[9];

	bool Read(ByteBuffer& buffer, u32 offset)
	{
		u32 baseAddress = offset;
		token = buffer.ReadAt<u32>(baseAddress);
		baseAddress += 0x04;

		if (token == NOVUSMAP_MFBO_TOKEN)
		{
			/* Add Token Size + Skip Size Field */
			size = buffer.ReadAt<u32>(baseAddress);
			baseAddress += 0x04;

			for (u32 i = 0; i < 9; i++)
			{
				max[i] = buffer.ReadAt<i16>(baseAddress);
				baseAddress += 0x02;
			}

			for (u32 i = 0; i < 9; i++)
			{
				min[i] = buffer.ReadAt<i16>(baseAddress);
				baseAddress += 0x02;
			}

			return true;
		}

		return false;
	}
};

struct MH2O
{
	MH2O() : token(0), size(0), liquidHeaders() { }

	u32 token;
	u32 size;

	struct LiquidHeader
	{
		u32 offsetInformation;
		u32 layers;
		u32 offsetRenderMask;

		static LiquidHeader Read(ByteBuffer& buffer, u32 offset)
		{
			LiquidHeader header;
			header.offsetInformation = buffer.ReadAt<u32>(offset);
			header.layers = buffer.ReadAt<u32>(offset + 0x4);
			header.offsetRenderMask = buffer.ReadAt<u32>(offset + 0x8);

			return header;
		}
	} liquidHeaders[ADT_CELLS_PER_GRID][ADT_CELLS_PER_GRID];

	bool Read(ByteBuffer& buffer, u32 offset)
	{
		u32 baseAddress = offset;
		token = buffer.ReadAt<u32>(baseAddress);
		baseAddress += 0x4;

		if (token == NOVUSMAP_MH2O_TOKEN)
		{
			size = buffer.ReadAt<u32>(baseAddress);
			baseAddress += 0x4;

			for (u32 i = 0; i < 256; i++)
			{
				u32 y = i / 16;
				u32 x = i % 16;

				liquidHeaders[y][x] = LiquidHeader::Read(buffer, baseAddress + (y * 16 + x) * sizeof(LiquidHeader));
			}

			return true;
		}

		return false;
	}
};

struct MHDR
{
	MHDR() : token(0), size(0), flags(0), offsetMcin(0), offsetMtex(0), offsetMmdx(0),
		offsetMmid(0), offsetMwmo(0), offsetMddf(0), offsetModf(0), offsetMfbo(0),
		offsetMh2o(0), offsetMtfx(0), pad4(0), pad5(0), pad6(0), pad7(0) { }

	enum MHDRFlags
	{
		mhdr_MFBO = 1,                // contains a MFBO chunk.
		mhdr_northrend = 2,           // is set for some northrend ones.
	};

	u32 token;
	u32 size;

	u32 flags;        // &1: MFBO, &2: unknown. in some Northrend ones.
	u32 offsetMcin;  //Positions of MCNK's
	u32 offsetMtex;  //List of all the textures used
	u32 offsetMmdx;  //List of all the md2's used
	u32 offsetMmid;  //Offsets into MMDX list for what each ID is
	u32 offsetMwmo;  //list of all the WMO's used
	u32 offsetMwid;  //Offsets into MWMO list for what each ID is
	u32 offsetMddf;  //Doodad Information
	u32 offsetModf;  //WMO Positioning Information
	u32 offsetMfbo;  // tbc, wotlk; only when flags&1
	u32 offsetMh2o;  // wotlk
	u32 offsetMtfx;  // wotlk
	u32 pad4;
	u32 pad5;
	u32 pad6;
	u32 pad7;

	void Read(ByteBuffer& buffer)
	{
		buffer.Read<u32>(token);
		buffer.Read<u32>(size);

		if (token == NOVUSMAP_MHDR_TOKEN)
		{
			buffer.Read<u32>(flags);
			buffer.Read<u32>(offsetMcin);
			buffer.Read<u32>(offsetMtex);
			buffer.Read<u32>(offsetMmdx);
			buffer.Read<u32>(offsetMmid);
			buffer.Read<u32>(offsetMwmo);
			buffer.Read<u32>(offsetMwid);
			buffer.Read<u32>(offsetMddf);
			buffer.Read<u32>(offsetModf);
			buffer.Read<u32>(offsetMfbo);
			buffer.Read<u32>(offsetMh2o);
			buffer.Read<u32>(offsetMtfx);
			buffer.Read<u32>(pad4);
			buffer.Read<u32>(pad5);
			buffer.Read<u32>(pad6);
			buffer.Read<u32>(pad7);
		}
	}
};
#pragma pack(pop)