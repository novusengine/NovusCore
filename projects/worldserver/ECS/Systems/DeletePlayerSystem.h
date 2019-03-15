#pragma once
#include <NovusTypes.h>
#include <entt.hpp>
#include <vector>
#include <Networking/ByteBuffer.h>
#include "../Message.h"

#include "../Connections/NovusConnection.h"
#include "../Components/SingletonComponent.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/DeletePlayerQueueSingleton.h"
#include "../Components/PlayerUpdateDataComponent.h"

namespace DeletePlayerSystem
{
    void Update(entt::registry &registry)
    {
		SingletonComponent& singleton = registry.get<SingletonComponent>(0);
        DeletePlayerQueueSingleton& deletePlayerQueue = registry.get<DeletePlayerQueueSingleton>(0);
        NovusConnection& novusConnection = *singleton.connection;
        
        Common::ByteBuffer buildPacket;
        std::vector<u32> deletedEntities;
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
                Common::ByteBuffer sentPacket;
                NovusHeader novusHeader;
                novusHeader.CreateForwardHeader(clientConnection.accountGuid, buildOpcode, buildPacket.GetActualSize());
                novusHeader.AddTo(sentPacket);
                sentPacket.Append(buildPacket);

                novusConnection.SendPacket(sentPacket);

                for (u32 guid : deletedEntities)
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