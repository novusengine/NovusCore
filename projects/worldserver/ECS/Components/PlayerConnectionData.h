#pragma once
#include <NovusTypes.h>

#include <Networking/ByteBuffer.h>
#include "../Connections/NovusConnection.h"

struct PlayerConnectionData
{
    PlayerConnectionData() : updateMask(PLAYER_END), playerFields(PLAYER_END * 4) { }

    UpdateMask<1344> updateMask;
    Common::ByteBuffer playerFields;
};