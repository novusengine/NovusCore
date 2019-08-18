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
#include <entt.hpp>

// Forward declarations to avoid circular includes
struct CharacterInfo;
struct SingletonComponent;
struct PlayerConnectionComponent;
struct PlayerPositionComponent;
class CharacterUtils
{
public:
    static void GetDisplayIdFromRace(const CharacterInfo characterData, u32& displayId);
    static u8 GetLastMovementTimeIndexFromOpcode(u16 opcode);

    static void BuildSpeedChangePacket(u64 characterGuid, f32 speed, Opcode opcode, std::shared_ptr<ByteBuffer> buffer);
    static void BuildFlyModePacket(u64 characterGuid, std::shared_ptr<ByteBuffer> buffer);

    template <typename... Args>
    static std::shared_ptr<ByteBuffer> BuildNotificationPacket(std::string message, Args... args)
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
    static void InvalidatePosition(SingletonComponent* singleton, PlayerConnectionComponent* playerConnection, PlayerPositionComponent* playerPositionData);
    static void InvalidatePosition(entt::registry* registry, u32 entityId);

    static void SendPacketToGridPlayers(entt::registry* registry, u32 entityId, std::shared_ptr<ByteBuffer> buffer, u16 opcode, bool excludeSelf = false);

private:
    CharacterUtils() { }
};
