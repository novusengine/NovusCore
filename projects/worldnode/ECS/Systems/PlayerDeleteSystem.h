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
#include <Networking/DataStore.h>
#include "../../Message.h"

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerDeleteQueueSingleton.h"

namespace PlayerDeleteSystem
{
    void Update(entt::registry &registry)
    {
		SingletonComponent& singleton = registry.ctx<SingletonComponent>();
        PlayerDeleteQueueSingleton& deletePlayerQueue = registry.ctx<PlayerDeleteQueueSingleton>();
        MapSingleton& mapSingleton = registry.ctx<MapSingleton>();

        std::shared_ptr<DataStore> buildPacket = DataStore::Borrow<4096>();
        std::vector<u64> deletedEntities;
        UpdateData updateData;

        ExpiredPlayerData expiredPlayerData;
        while (deletePlayerQueue.expiredEntityQueue->try_dequeue(expiredPlayerData))
        {
            // Remove player from current ADT
            PlayerPositionComponent& positionComponent = registry.get<PlayerPositionComponent>(expiredPlayerData.entityGuid);
            u32 mapId = positionComponent.mapId;
            u16 adtId = positionComponent.adtId;

            if (adtId != INVALID_ADT)
            {
                std::vector<u32>& playerList = mapSingleton.maps[mapId].playersInAdts[adtId];
                auto iterator = std::find(playerList.begin(), playerList.end(), expiredPlayerData.entityGuid);
                assert(iterator != playerList.end());
                playerList.erase(iterator);
            }

            updateData.AddGuid(expiredPlayerData.characterGuid);
            deletedEntities.push_back(expiredPlayerData.characterGuid);
            registry.destroy(expiredPlayerData.entityGuid);
            singleton.accountToEntityMap.erase(expiredPlayerData.accountGuid);
        }

        if (!updateData.IsEmpty())
        {
            u16 buildOpcode = 0;
            updateData.Build(buildPacket.get(), buildOpcode);

            auto view = registry.view<PlayerConnectionComponent, PlayerUpdateDataComponent>();
            view.each([&buildPacket, &deletedEntities, buildOpcode](const auto, PlayerConnectionComponent& playerConnection, PlayerUpdateDataComponent& playerUpdateData)
            {
                playerConnection.socket->SendPacket(buildPacket.get(), buildOpcode);

                for (u64 guid : deletedEntities)
                {
                    auto position = std::find(playerUpdateData.visibleGuids.begin(), playerUpdateData.visibleGuids.end(), guid);
                    if (position != playerUpdateData.visibleGuids.end())
                    {
                        playerUpdateData.visibleGuids.erase(position);
                    }
                }
            });
        }
    }
}
