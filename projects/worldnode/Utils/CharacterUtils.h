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
#include <Networking/ByteBuffer.h>
#include <Networking/Opcode/Opcode.h>
#include <Utils/StringUtils.h>
#include "MapUtils.h"
#include "../DatabaseCache/CharacterDatabaseCache.h"
#include "../Connections/WorldConnection.h"

#include "../ECS/Components/Singletons/SingletonComponent.h"
#include "../ECS/Components/PlayerConnectionComponent.h"
#include "../ECS/Components/PlayerUpdateDataComponent.h"
#include "../ECS/Components/PlayerPositionComponent.h"

namespace CharacterUtils
{
inline void GetDisplayIdFromRace(const CharacterInfo characterData, u32& displayId)
{
    switch (characterData.race)
    {
    // Human
    case 1:
    {
        displayId = 49 + characterData.gender;
        break;
    }
    // Orc
    case 2:
    {
        displayId = 51 + characterData.gender;
        break;
    }
    // Dwarf
    case 3:
    {
        displayId = 53 + characterData.gender;
        break;
    }
    // Night Elf
    case 4:
    {
        displayId = 55 + characterData.gender;
        break;
    }
    // Undead
    case 5:
    {
        displayId = 57 + characterData.gender;
        break;
    }
    // Tauren
    case 6:
    {
        displayId = 59 + characterData.gender;
        break;
    }
    // Gnome
    case 7:
    {
        displayId = 1563 + characterData.gender;
        break;
    }
    // Gnome
    case 8:
    {
        displayId = 1478 + characterData.gender;
        break;
    }
    // Blood Elf
    case 10:
    {
        displayId = 15476 - characterData.gender;
        break;
    }
    // Draenie
    case 11:
    {
        displayId = 16125 + characterData.gender;
        break;
    }
    }
}
inline u8 GetLastMovementTimeIndexFromOpcode(u16 opcode)
{
    u8 opcodeIndex = 0;
    switch (opcode)
    {
    case Opcode::MSG_MOVE_STOP:
    {
        opcodeIndex = 0;
        break;
    }
    case Opcode::MSG_MOVE_STOP_STRAFE:
    {
        opcodeIndex = 1;
        break;
    }
    case Opcode::MSG_MOVE_STOP_TURN:
    {
        opcodeIndex = 2;
        break;
    }
    case Opcode::MSG_MOVE_STOP_PITCH:
    {
        opcodeIndex = 3;
        break;
    }
    case Opcode::MSG_MOVE_START_FORWARD:
    {
        opcodeIndex = 4;
        break;
    }
    case Opcode::MSG_MOVE_START_BACKWARD:
    {
        opcodeIndex = 5;
        break;
    }
    case Opcode::MSG_MOVE_START_STRAFE_LEFT:
    {
        opcodeIndex = 6;
        break;
    }
    case Opcode::MSG_MOVE_START_STRAFE_RIGHT:
    {
        opcodeIndex = 7;
        break;
    }
    case Opcode::MSG_MOVE_START_TURN_LEFT:
    {
        opcodeIndex = 8;
        break;
    }
    case Opcode::MSG_MOVE_START_TURN_RIGHT:
    {
        opcodeIndex = 9;
        break;
    }
    case Opcode::MSG_MOVE_START_PITCH_UP:
    {
        opcodeIndex = 10;
        break;
    }
    case Opcode::MSG_MOVE_START_PITCH_DOWN:
    {
        opcodeIndex = 11;
        break;
    }
    case Opcode::MSG_MOVE_START_ASCEND:
    {
        opcodeIndex = 12;
        break;
    }
    case Opcode::MSG_MOVE_STOP_ASCEND:
    {
        opcodeIndex = 13;
        break;
    }
    case Opcode::MSG_MOVE_START_DESCEND:
    {
        opcodeIndex = 14;
        break;
    }
    case Opcode::MSG_MOVE_START_SWIM:
    {
        opcodeIndex = 15;
        break;
    }
    case Opcode::MSG_MOVE_STOP_SWIM:
    {
        opcodeIndex = 16;
        break;
    }
    case Opcode::MSG_MOVE_FALL_LAND:
    {
        opcodeIndex = 17;
        break;
    }
    case Opcode::CMSG_MOVE_FALL_RESET:
    {
        opcodeIndex = 18;
        break;
    }
    case Opcode::MSG_MOVE_JUMP:
    {
        opcodeIndex = 19;
        break;
    }
    case Opcode::MSG_MOVE_SET_FACING:
    {
        opcodeIndex = 20;
        break;
    }
    case Opcode::MSG_MOVE_SET_PITCH:
    {
        opcodeIndex = 21;
        break;
    }
    case Opcode::MSG_MOVE_SET_RUN_MODE:
    {
        opcodeIndex = 22;
        break;
    }
    case Opcode::MSG_MOVE_SET_WALK_MODE:
    {
        opcodeIndex = 23;
        break;
    }
    case Opcode::CMSG_MOVE_SET_FLY:
    {
        opcodeIndex = 24;
        break;
    }
    case Opcode::CMSG_MOVE_CHNG_TRANSPORT:
    {
        opcodeIndex = 25;
        break;
    }
    case Opcode::MSG_MOVE_HEARTBEAT:
    {
        opcodeIndex = 26;
        break;
    }
    }

    return opcodeIndex;
}

inline void BuildSpeedChangePacket(u64 characterGuid, f32 speed, Opcode opcode, std::shared_ptr<ByteBuffer> buffer)
{
    buffer->PutGuid(characterGuid);
    buffer->PutU32(0);

    /* Convert speed to a multiplicative of base speed */
    if (opcode == Opcode::SMSG_FORCE_WALK_SPEED_CHANGE)
    {
        speed *= 2.5f;
    }
    else if (opcode == Opcode::SMSG_FORCE_RUN_SPEED_CHANGE)
    {
        // Write extra bit added in 2.1.0
        buffer->PutU8(1);
        speed *= 7.1111f;
    }
    else if (opcode == Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE)
    {
        speed *= 4.5f;
    }
    else if (opcode == Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE)
    {
        speed *= 4.722222f;
    }
    else if (opcode == Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE)
    {
        speed *= 2.5f;
    }
    else if (opcode == Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE)
    {
        speed *= 7.1111f;
    }
    else if (opcode == Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE)
    {
        speed *= 4.5f;
    }

    buffer->PutF32(speed);
}
inline void BuildFlyModePacket(u64 characterGuid, std::shared_ptr<ByteBuffer> buffer)
{
    buffer->PutGuid(characterGuid);
    buffer->PutU32(0); // Unk
}

template <typename... Args>
inline std::shared_ptr<ByteBuffer> BuildNotificationPacket(std::string message, Args... args)
{
    char str[256];
    i32 length = StringUtils::FormatString(str, sizeof(str), message.c_str(), args...);

    std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<512>();
    buffer->PutU8(0x00);  // CHAT_MSG_SYSTEM
    buffer->PutI32(0x00); // LANG_UNIVERSAL
    buffer->PutU64(0);
    buffer->PutU32(0); // Chat Flag (??)

    // This is based on chatType
    buffer->PutU64(0); // Receiver (0) for none

    buffer->PutU32(static_cast<u32>(length) + 1);
    buffer->PutString(str);
    buffer->PutU8(0); // Chat Tag

    return buffer;
}

// This function will force the clients position to match what we have in our PlayerPositionComponent, use this after serverside movement
inline void InvalidatePosition(entt::registry* registry, u32 entityId)
{
    SingletonComponent& singletonData = registry->ctx<SingletonComponent>();
    PlayerConnectionComponent& playerConnection = registry->get<PlayerConnectionComponent>(entityId);
    PlayerPositionComponent& playerPositionData = registry->get<PlayerPositionComponent>(entityId);

    std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<42>();
    buffer->PutGuid(playerConnection.characterGuid);
    buffer->PutU32(0); // Teleport Count

    // Movement
    buffer->PutU32(0);
    buffer->PutU16(0);
    buffer->PutU32(static_cast<u32>(singletonData.lifeTimeInMS));

    buffer->Put<Vector3>(playerPositionData.position);
    buffer->PutF32(playerPositionData.orientation);
    buffer->PutU32(0);

    playerConnection.socket->SendPacket(buffer.get(), Opcode::MSG_MOVE_TELEPORT_ACK);
}

inline void SendPacketToGridPlayers(entt::registry* registry, u32 entityId, std::shared_ptr<ByteBuffer> buffer, u16 opcode, bool excludeSelf = false)
{
    PlayerPositionComponent& playerPositionData = registry->get<PlayerPositionComponent>(entityId);
    MapsUtils::SendPacketToGridPlayers(registry, playerPositionData.mapId, playerPositionData.adtId, buffer, opcode, excludeSelf ? entityId : 0);
}
} // namespace CharacterUtils
