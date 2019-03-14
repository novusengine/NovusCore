#pragma once
#include <NovusTypes.h>

#include <Networking/ByteBuffer.h>
#include "../Connections/NovusConnection.h"
#include <vector>

struct PlayerUpdateDataComponent
{
    PlayerUpdateDataComponent() : updateMask(PLAYER_END), playerFields(PLAYER_END * 4) { }

    UpdateMask<1344> updateMask;
    Common::ByteBuffer playerFields;
    std::vector<u64> visibleGuids;
};