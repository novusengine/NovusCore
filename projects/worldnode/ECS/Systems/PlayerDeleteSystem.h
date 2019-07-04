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
#include <entt.hpp>
#include <vector>
#include <Networking/ByteBuffer.h>
#include "../../Message.h"

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/GuidLookupSingleton.h"
#include "../Components/Singletons/PlayerDeleteQueueSingleton.h"

namespace PlayerDeleteSystem
{
void Update(entt::registry& registry)
{
    SingletonComponent& singleton = registry.ctx<SingletonComponent>();
    GuidLookupSingleton& guidLookupSingleton = registry.ctx<GuidLookupSingleton>();
    PlayerDeleteQueueSingleton& deletePlayerQueue = registry.ctx<PlayerDeleteQueueSingleton>();
    MapSingleton& mapSingleton = registry.ctx<MapSingleton>();

    std::shared_ptr<ByteBuffer> buildPacket = ByteBuffer::Borrow<4096>();

    ExpiredPlayerData expiredPlayerData;
    while (deletePlayerQueue.expiredEntityQueue->try_dequeue(expiredPlayerData))
    {
        u64 characterGuid = expiredPlayerData.characterGuid;

        PlayerPositionComponent& positionComponent = registry.get<PlayerPositionComponent>(expiredPlayerData.entityId);
        u32 mapId = positionComponent.mapId;
        u16 adtId = positionComponent.adtId;

        // Remove player from current ADT
        if (adtId != INVALID_ADT)
        {
            std::vector<u32>& playerList = mapSingleton.maps[mapId].playersInAdts[adtId];
            auto iterator = std::find(playerList.begin(), playerList.end(), expiredPlayerData.entityId);
            assert(iterator != playerList.end());
            playerList.erase(iterator);

            for (u32 entity : playerList)
            {
                PlayerConnectionComponent& currentConnection = registry.get<PlayerConnectionComponent>(entity);
                PlayerUpdateDataComponent& currentUpdateData = registry.get<PlayerUpdateDataComponent>(entity);
                PlayerFieldDataComponent& currentFieldData = registry.get<PlayerFieldDataComponent>(entity);

                auto iterator = std::find(currentUpdateData.visibleGuids.begin(), currentUpdateData.visibleGuids.end(), characterGuid);
                if (iterator != currentUpdateData.visibleGuids.end())
                {
                    currentUpdateData.visibleGuids.erase(iterator);
                    currentFieldData.updateData.AddInvalidGuid(characterGuid);
                }
            }
        }

        singleton.accountToEntityMap.erase(expiredPlayerData.accountGuid);
        guidLookupSingleton.playerToEntityMap.erase(expiredPlayerData.entityId);
        registry.destroy(expiredPlayerData.entityId);
    }
}
} // namespace PlayerDeleteSystem
