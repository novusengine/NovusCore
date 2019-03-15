#pragma once
#include <NovusTypes.h>
#include <entt.hpp>
#include <Networking/ByteBuffer.h>
#include "../Message.h"

#include "../Connections/NovusConnection.h"
#include "../Components/SingletonComponent.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/PositionComponent.h"
#include "../Components/CreatePlayerQueueSingleton.h"

namespace CreatePlayerSystem
{
    void Update(entt::registry &registry)
    {
		SingletonComponent& singleton = registry.get<SingletonComponent>(0);
        CreatePlayerQueueSingleton& createPlayerQueue = registry.get<CreatePlayerQueueSingleton>(0);

        Message message;
        while (createPlayerQueue.newEntityQueue->try_dequeue(message))
        {
            u64 playerGuid = 0;
            message.packet.Read<u64>(playerGuid);

            u32 entity = registry.create();
            ConnectionComponent& connection = registry.assign<ConnectionComponent>(entity, u32(message.account), playerGuid, false);
            connection.packets.push_back({ u32(message.opcode), false, message.packet });

            registry.assign<PlayerUpdateDataComponent>(entity);
            registry.assign<PositionComponent>(entity, 0u, -8949.950195f, -132.492996f, 83.531197f, 0.f);

            singleton.accountToEntityMap[u32(message.account)] = entity;
        }
    }
}