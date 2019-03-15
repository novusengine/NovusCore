#pragma once
#include <NovusTypes.h>

#include <Networking/ByteBuffer.h>
#include "../Connections/NovusConnection.h"
#include <vector>

struct PositionUpdateData
{
    u16 opcode;
    u32 movementFlags;
    u16 movementFlagsExtra;
    u32 gameTime;
    f32 x;
    f32 y;
    f32 z;
    f32 orientation;
    u32 fallTime;
};

struct PlayerUpdateDataComponent
{
    PlayerUpdateDataComponent() : updateMask(PLAYER_END), playerFields(PLAYER_END * 4) { }

    UpdateMask<1344> updateMask;
    Common::ByteBuffer playerFields;
    std::vector<u32> visibleGuids;
    std::vector<PositionUpdateData> positionUpdateData;
};