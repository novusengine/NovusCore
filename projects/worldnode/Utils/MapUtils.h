#pragma once
#include <NovusTypes.h>
#include <Networking/ByteBuffer.h>
#include <entt.hpp>
#include "../Connections/WorldConnection.h"
#include "../Game/NovusMap.h"

#include "../ECS/Components/PlayerConnectionComponent.h"
#include "../ECS/Components/Singletons/MapSingleton.h"

namespace MapsUtils
{
inline void SendPacketToGridPlayers(entt::registry* registry, u32 mapId, u16 adtId, std::shared_ptr<ByteBuffer> buffer, u16 opcode, u32 excludeEntityId = 0)
{
    MapSingleton& mapSingleton = registry->ctx<MapSingleton>();

    std::vector<u32>& playerList = mapSingleton.maps[mapId].playersInAdts[adtId];
    for (u32 entityId : playerList)
    {
        if (entityId == excludeEntityId)
            continue;

        PlayerConnectionComponent& currentConnection = registry->get<PlayerConnectionComponent>(entityId);
        currentConnection.socket->SendPacket(buffer.get(), opcode);
    }
}
} // namespace MapsUtils