#pragma once
#include <NovusTypes.h>
#include <vector>
#include <Networking/ByteBuffer.h>

struct PositionComponent 
{
	u32 mapId;
	f32 x;
	f32 y;
	f32 z;
	f32 orientation;
    f32 oldx;
    f32 oldy;
    f32 oldz;
    f32 oldorientation;

    std::vector<PositionUpdateData> positionUpdateData;
};