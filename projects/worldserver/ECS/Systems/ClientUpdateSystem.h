#pragma once
#include <NovusTypes.h>
#include <entt.hpp>

#include "../Components/PositionComponent.h"
#include "../Components/UnitStatusComponent.h"
#include "../Components/SingletonComponent.h"
#include "../Components/PlayerUpdatesQueueSingleton.h"

namespace ClientUpdateSystem
{
	void Update(entt::registry &registry) 
    {
        SingletonComponent& singleton = registry.get<SingletonComponent>(0);
        PlayerUpdatesQueueSingleton& playerUpdatesQueue = registry.get<PlayerUpdatesQueueSingleton>(0);
        NovusConnection& novusConnection = *singleton.connection;

        auto view = registry.view<ConnectionComponent, PlayerUpdateDataComponent, PositionComponent>();

        view.each([&novusConnection, playerUpdatesQueue](const auto, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData, PositionComponent& clientPositionData)
        {
            NovusHeader novusHeader;
            for (PlayerUpdatePacket playerUpdatePacket : playerUpdatesQueue.playerUpdatePacketQueue)
            {
                if (playerUpdatePacket.updateType == PlayerUpdateDataSystem::UPDATETYPE_CREATE_OBJECT ||
                    playerUpdatePacket.updateType == PlayerUpdateDataSystem::UPDATETYPE_CREATE_OBJECT2 &&
                    playerUpdatePacket.characterGuid != clientConnection.characterGuid)
                {
                    if (std::find(clientUpdateData.visibleGuids.begin(), clientUpdateData.visibleGuids.end(), playerUpdatePacket.characterGuid) == clientUpdateData.visibleGuids.end())
                    {
                        novusHeader.CreateForwardHeader(clientConnection.accountGuid, playerUpdatePacket.opcode, playerUpdatePacket.data.size());

                        Common::ByteBuffer packet(novusHeader.size);
                        novusHeader.AddTo(packet);
                        packet.Append(playerUpdatePacket.data);

                        novusConnection.SendPacket(packet);
                        clientUpdateData.visibleGuids.push_back(playerUpdatePacket.characterGuid);
                    }
                }
            }

            for (MovementPacket movementPacket : playerUpdatesQueue.playerMovementPacketQueue)
            {
                if (clientConnection.characterGuid != movementPacket.characterGuid)
                {
                    novusHeader.CreateForwardHeader(clientConnection.accountGuid, movementPacket.opcode, movementPacket.data.size());

                    Common::ByteBuffer packet(novusHeader.size);
                    novusHeader.AddTo(packet);
                    packet.Append(movementPacket.data);

                    novusConnection.SendPacket(packet);
                }
            }
        });

        // Clear Queues
        playerUpdatesQueue.playerUpdatePacketQueue.clear();
        playerUpdatesQueue.playerMovementPacketQueue.clear();
	}
}
