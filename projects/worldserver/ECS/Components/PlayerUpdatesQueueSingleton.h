#pragma once
#include <NovusTypes.h>
#include <vector>
#include <Networking/ByteBuffer.h>

struct PlayerUpdatePacket
{
    u16 opcode;
    u32 characterGuid;
    u8 updateType;
    Common::ByteBuffer data;
};

struct MovementPacket
{
    u16 opcode;
    u32 characterGuid;
    Common::ByteBuffer data;
};

struct PlayerUpdatesQueueSingleton
{
    std::vector<PlayerUpdatePacket> playerUpdatePacketQueue;
    std::vector<MovementPacket> playerMovementPacketQueue;
};