#include "NovusMap.h"

const float mapSideHalfLength = 17066.0f;
const float adtSideLength = 533.33333f;
const float chunkSideLength = 33.33333f;
const float cellSideLength = 4.1666625f;
const u8 chunkStride = 16;
const u8 cellStride = 8;

f32 NovusMap::GetHeight(Vector2& pos)
{
    // This is translated to remap positions [-17066 .. 17066] to [0 ..  34132]
    // Flipping X and Y here is intended, a quirk of how the ADTs are stored I guess - Pursche
    Vector2 translatedPos = Vector2(mapSideHalfLength - pos.y, mapSideHalfLength - pos.x);

    u16 adtId;
    if (!GetAdtIdFromWorldPosition(pos, adtId))
        return 0.0f;

	NovusAdt& adt = adts[adtId];

	Vector2 adtRemainder = translatedPos % adtSideLength;
	Vector2 chunk = adtRemainder / chunkSideLength;
	u32 chunkID = Math::FloorToInt(chunk.x) + (Math::FloorToInt(chunk.y) * chunkStride);

	// We have to flip these coordinates, this is intentional
	Vector2 chunkRemainder = Vector2(Math::Modulus(adtRemainder.y, chunkSideLength), Math::Modulus(adtRemainder.x, chunkSideLength));

	Vector2 cellPos = chunkRemainder / cellSideLength;
	Vector2 cellRemainder = chunkRemainder % cellSideLength;

	// This is what our height data looks like
	// 0     1     2     3     4     5     6     7     8 
	//    9    10    11    12    13    14    15    16   
	// 17    18   19    20    21    22    23    24     25
	//    26    27    28    29    30    31   32    33   
	// 34    35    36    37    38    39    40   41     42
	//    43    44    45    46    47    48    49    50   
	// 51    52    53    54    55    56    57    58    59
	//    60    61    62    63    64    65    66    67   
	// 68    69    70    71    72    73    74    75    76
	//    77    78    79    80    81    82    83    84   
	// 85    86    87    88    89    90    91    92    93
	//    94    95    96    97    98    99    100   101  
	// 102   103   104   105   106   107   108   109   110
	//    111   112   113   114   115   116   117   118  
	// 119   120   121   122   123   124   125   126   127
	//    128   129   130   131   132   133   134   135  
	// 136   137   138   139   140   141   142   143   144

	// cellStride represents the length of the side of the "inner grid", uneven rows of this structure
	// We are going to need to find the "row stride" of this data, meaning the length of two rows, I.E this:
	// 0     1     2     3     4     5     6     7     8 
	//    9    10    11    12    13    14    15    16
	// This can also be described as cellStride + cellStride+1
	u16 rowStride = cellStride + cellStride + 1;

	// Using CellPos we need to build a square looking something like this depending on what cell we're on
	// TL     TR	
	//     C
	// BL     BR
	// TL = TopLeft, TR = TopRight, C = Center, BL = BottomLeft, BR = BottomRight

	// Lets start by finding the Top Left "Vertex"
	u16 topLeftVertex = (Math::FloorToInt(cellPos.x) * rowStride) + Math::FloorToInt(cellPos.y);

	// Top Right is always +1 from Top Left
	u16 topRightVertex = topLeftVertex + 1;

	// Bottom Left is a full rowStride from the Top Left vertex
	u16 bottomLeftVertex = topLeftVertex + rowStride;

	// Bottom Right is always +1 from Bottom Left
	u16 bottomRightVertex = bottomLeftVertex + 1;

	// Center is always + cellStride + 1 from Top Left
	u16 centerVertex = topLeftVertex + cellStride + 1;

	// The next step is to use the cellRemainder to figure out which of these triangles we are on: https://imgur.com/i9aHwus
	// When we know we set a, b, c, aHeight, bHeight and cHeight accordingly
	f32 halfCellSideLength = cellSideLength / 2.0f;

	// NOTE: Order of A, B and C is important, don't swap them around without understanding how it works
	Vector2 a = Vector2(halfCellSideLength, halfCellSideLength);
	f32 aHeight = adt.heightHeader.heightData[chunkID][centerVertex];
	Vector2 b = Vector2::Zero;
	f32 bHeight = 0.0f;
	Vector2 c = Vector2::Zero;
	f32 cHeight = 0.0f;
	Vector2 p = cellRemainder;

	if (Math::Abs(cellRemainder.x - halfCellSideLength) > Math::Abs(cellRemainder.y - halfCellSideLength))
	{
		if (cellRemainder.y > halfCellSideLength)
		{
			// East triangle consists of Center, TopRight and BottomRight
			b = Vector2(0.0f, cellSideLength);
			bHeight = adt.heightHeader.heightData[chunkID][topRightVertex];

			c = Vector2(cellSideLength, cellSideLength);
			cHeight = adt.heightHeader.heightData[chunkID][bottomRightVertex];
		}
		else
		{
			// West triangle consists of Center, BottomLeft and TopLeft
			b = Vector2(cellSideLength, 0.0f);
			bHeight = adt.heightHeader.heightData[chunkID][bottomLeftVertex];

			c = Vector2(0, 0);
			cHeight = adt.heightHeader.heightData[chunkID][topLeftVertex];
		}
	}
	else
	{
		if (cellRemainder.x < halfCellSideLength)
		{
			// North triangle consists of Center, TopLeft and TopRight
			b = Vector2(0, 0);
			bHeight = adt.heightHeader.heightData[chunkID][topLeftVertex];

			c = Vector2(0.0f, cellSideLength);
			cHeight = adt.heightHeader.heightData[chunkID][topRightVertex];
		}
		else
		{
			// South triangle consists of Center, BottomRight and BottomLeft
			b = Vector2(cellSideLength, cellSideLength);
			bHeight = adt.heightHeader.heightData[chunkID][bottomRightVertex];

			c = Vector2(cellSideLength, 0.0f);
			cHeight = adt.heightHeader.heightData[chunkID][bottomLeftVertex];
		}
	}

	// Finally we do standard barycentric triangle interpolation to get the actual height of the position
	f32 det = (b.y - c.y) * (a.x - c.x) + (c.x - b.x) * (a.y - c.y);
	f32 factorA = (b.y - c.y) * (p.x - c.x) + (c.x - b.x) * (p.y - c.y);
	f32 factorB = (c.y - a.y) * (p.x - c.x) + (a.x - c.x) * (p.y - c.y);
	f32 alpha = factorA / det;
	f32 beta = factorB / det;
	f32 gamma = 1.0f - alpha - beta;

	f32 height = aHeight * alpha + bHeight * beta + cHeight * gamma;

	return height;
}

bool NovusMap::GetAdtIdFromWorldPosition(Vector2& pos, u16& adtId)
{
    // This is translated to remap positions [-17066 .. 17066] to [0 ..  34132]
    // Flipping X and Y here is intended, a quirk of how the ADTs are stored I guess - Pursche
    Vector2 translatedPos = Vector2(mapSideHalfLength - pos.y, mapSideHalfLength - pos.x);

    Vector2 adtPos = translatedPos / adtSideLength;
    adtId = Math::FloorToInt(adtPos.x) + (Math::FloorToInt(adtPos.y) * blockStride);

    return adts.find(adtId) != adts.end();
}

void NovusMap::GetChunkPositionFromAdtId(u16 adtId, u16& x, u16& y)
{
    x = adtId % blockStride;
    y = adtId / blockStride;
}

bool NovusMap::GetAdtIdFromChunkPosition(u16 x, u16 y, u16& adtId)
{
    adtId = Math::FloorToInt(x) + (Math::FloorToInt(y) * blockStride);

    return adts.find(adtId) != adts.end();
}