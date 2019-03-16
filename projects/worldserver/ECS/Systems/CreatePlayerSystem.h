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
#include <Networking/ByteBuffer.h>
#include "../Message.h"

#include "../DatabaseCache/CharacterDatabaseCache.h"

#include "../Connections/NovusConnection.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/PositionComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/CreatePlayerQueueSingleton.h"

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

            CharacterDatabaseCache characterDatabaseCache;
            CharacterData characterData = characterDatabaseCache.GetCharacterData(playerGuid);

            // -8949.950195f, -132.492996f, 83.531197f, 0.f
            registry.assign<PlayerUpdateDataComponent>(entity);
            registry.assign<PositionComponent>(entity, characterData.mapId, characterData.coordinateX, characterData.coordinateY, characterData.coordinateZ, characterData.orientation);

            singleton.accountToEntityMap[u32(message.account)] = entity;
        }
    }
}