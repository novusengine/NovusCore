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
#include <algorithm>
#include <NovusTypes.h>
#include <entt.hpp>
#include <Networking/ByteBuffer.h>

#include "../../NovusEnums.h"

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerFieldDataComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerUpdatesQueueSingleton.h"

#include <tracy/Tracy.hpp>

namespace PlayerUpdateSystem
{
void Update(entt::registry& registry)
{
    PlayerUpdatesQueueSingleton& playerUpdatesQueue = registry.ctx<PlayerUpdatesQueueSingleton>();
    MapSingleton& mapSingleton = registry.ctx<MapSingleton>();

    auto view = registry.view<PlayerConnectionComponent, PlayerPositionComponent, PlayerUpdateDataComponent>();
    view.each([&playerUpdatesQueue, &mapSingleton](const auto, PlayerConnectionComponent& playerConnection, PlayerPositionComponent& playerPositionData, PlayerUpdateDataComponent& playerUpdateData) {
        if (playerUpdateData.positionUpdateData.size())
        {
            ZoneScopedNC("PositionUpdate", tracy::Color::Yellow2) for (PositionUpdateData positionData : playerUpdateData.positionUpdateData)
            {
                MovementPacket movementPacket;
                movementPacket.opcode = positionData.opcode;
                movementPacket.characterGuid = playerConnection.characterGuid;

                movementPacket.data = ByteBuffer::Borrow<38>();
                movementPacket.data->PutGuid(movementPacket.characterGuid);
                movementPacket.data->PutU32(positionData.movementFlags);
                movementPacket.data->PutU16(positionData.movementFlagsExtra);
                movementPacket.data->PutU32(positionData.gameTime);
                movementPacket.data->Put<Vector3>(positionData.position);
                movementPacket.data->PutF32(positionData.orientation);
                movementPacket.data->PutU32(positionData.fallTime);

                playerUpdatesQueue.playerMovementPacketQueue.push_back(movementPacket);
            }

            // Figure out what ADT the player is in
            Vector2 position = Vector2(playerPositionData.position.x, playerPositionData.position.y);
            u32 mapId = playerPositionData.mapId;

            u16 adtId;
            if (mapSingleton.maps[mapId].GetAdtIdFromWorldPosition(position, adtId))
            {
                // If the ADT doesnt match the previous known ADT we need to update it.
                if (adtId != playerPositionData.adtId)
                {
                    u32 entityId = playerConnection.entityId;

                    std::vector<u32>& playerList = mapSingleton.maps[mapId].playersInAdts[playerPositionData.adtId];
                    if (playerPositionData.adtId != INVALID_ADT)
                    {
                        auto iterator = std::find(playerList.begin(), playerList.end(), entityId);
                        assert(iterator != playerList.end());
                        playerList.erase(iterator);

                        playerConnection.SendChatNotification("[DEBUG] Old ADT: %u has %u players", playerPositionData.adtId, playerList.size());
                    }

                    playerPositionData.adtId = adtId;

                    mapSingleton.maps[mapId].playersInAdts[adtId].push_back(entityId);
                    playerConnection.SendChatNotification("[DEBUG] New ADT: %u has %u players", adtId, mapSingleton.maps[mapId].playersInAdts[adtId].size());
                }
            }

            // Clear Position Updates
            playerUpdateData.positionUpdateData.clear();
        }

        if (playerUpdateData.chatUpdateData.size() > 0)
        {
            ZoneScopedNC("ChatUpdate", tracy::Color::Yellow2) for (ChatUpdateData chatData : playerUpdateData.chatUpdateData)
            {
                ChatPacket chatPacket;
                chatPacket.data = ByteBuffer::Borrow<286>();

                chatPacket.data->PutU8(chatData.chatType);
                chatPacket.data->PutI32(chatData.language);
                chatPacket.data->PutU64(chatData.sender);
                chatPacket.data->PutU32(0); // Chat Flag (??)

                // This is based on chatType
                chatPacket.data->PutU64(0); // Receiver (0) for none

                chatPacket.data->PutU32(static_cast<u32>(chatData.message.length()) + 1);
                chatPacket.data->PutString(chatData.message);
                chatPacket.data->PutU8(0); // Chat Tag

                playerUpdatesQueue.playerChatPacketQueue.push_back(chatPacket);
            }

            // Clear Chat Updates
            playerUpdateData.chatUpdateData.clear();
        }
    });
}
} // namespace PlayerUpdateSystem
