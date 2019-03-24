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
#include "../Message.h"

#include "../Connections/NovusConnection.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/DeletePlayerQueueSingleton.h"

namespace DeletePlayerSystem
{
    void Update(entt::registry &registry)
    {
		SingletonComponent& singleton = registry.get<SingletonComponent>(0);
        DeletePlayerQueueSingleton& deletePlayerQueue = registry.get<DeletePlayerQueueSingleton>(0);
        NovusConnection& novusConnection = *singleton.connection;
        
        Common::ByteBuffer buildPacket;
        std::vector<u64> deletedEntities;
        UpdateData updateData;

        ExpiredPlayerData expiredPlayerData;
        while (deletePlayerQueue.expiredEntityQueue->try_dequeue(expiredPlayerData))
        {
            auto itr = singleton.accountToEntityMap.find(expiredPlayerData.account);
            if (itr != singleton.accountToEntityMap.end())
            {
                updateData.AddGuid(expiredPlayerData.guid);
                deletedEntities.push_back(expiredPlayerData.guid);
                registry.destroy(itr->second);
                singleton.accountToEntityMap.erase(expiredPlayerData.account);
            }
        }

        if (!updateData.IsEmpty())
        {
            u16 buildOpcode = 0;
            updateData.Build(buildPacket, buildOpcode);

            auto view = registry.view<ConnectionComponent, PlayerUpdateDataComponent>();
            view.each([&novusConnection, &buildPacket, &deletedEntities, buildOpcode](const auto, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData)
            {
                NovusHeader novusHeader;
                novusHeader.CreateForwardHeader(clientConnection.accountGuid, buildOpcode, buildPacket.GetActualSize());
                novusConnection.SendPacket(novusHeader.BuildHeaderPacket(buildPacket));

                for (u64 guid : deletedEntities)
                {
                    auto position = std::find(clientUpdateData.visibleGuids.begin(), clientUpdateData.visibleGuids.end(), guid);
                    if (position != clientUpdateData.visibleGuids.end())
                    {
                        clientUpdateData.visibleGuids.erase(position);
                    }
                }
            });
        }
    }
}