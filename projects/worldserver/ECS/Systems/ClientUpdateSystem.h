#pragma once
#include <NovusTypes.h>
#include <entt.hpp>

#include "../Components/PositionComponent.h"
#include "../Components/UnitStatusComponent.h"
#include "../Components/SingletonComponent.h"

namespace ClientUpdateSystem
{
	void Update(entt::registry &registry) 
    {
        SingletonComponent& singleton = registry.get<SingletonComponent>(0);
        NovusConnection& novusConnection = *singleton.connection;

        auto view = registry.view<ConnectionComponent, PlayerUpdateDataComponent, PositionComponent>();
        auto subView = registry.view<ConnectionComponent>();

        view.each([&novusConnection, subView](const auto, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData, PositionComponent& clientPositionData)
        {
            if (clientPositionData.positionUpdateData.size() > 0)
            {
                subView.each([&novusConnection, clientConnection, clientPositionData](const auto, ConnectionComponent& connection)
                {
                    if (clientConnection.characterGuid != connection.characterGuid)
                    {
                        for (PositionUpdateData positionUpdate : clientPositionData.positionUpdateData)
                        {
                            NovusHeader packetHeader;
                            packetHeader.CreateForwardHeader(connection.accountGuid, positionUpdate.opcode, positionUpdate.data.size());

                            Common::ByteBuffer writtenData(packetHeader.size);
                            packetHeader.AddTo(writtenData);
                            writtenData.Append(positionUpdate.data);

                            novusConnection.SendPacket(writtenData);
                        }
                    }
                });

                // Clear position updates
                clientPositionData.positionUpdateData.clear();
            }
        });
	}
}
