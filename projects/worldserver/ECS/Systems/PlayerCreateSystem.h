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
#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerPositionComponent.h"
#include "../Components/PlayerSpellStorageComponent.h"
#include "../Components/PlayerSkillStorageComponent.h"
#include "../Components/PlayerInitializeComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerCreateQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"

namespace PlayerCreateSystem
{
    void Update(entt::registry &registry)
    {
		SingletonComponent& singleton = registry.ctx<SingletonComponent>();
        PlayerCreateQueueSingleton& createPlayerQueue = registry.ctx<PlayerCreateQueueSingleton>();
        CharacterDatabaseCacheSingleton& characterDatabase = registry.ctx<CharacterDatabaseCacheSingleton>();

        Message message;
        while (createPlayerQueue.newEntityQueue->try_dequeue(message))
        {
            u64 playerGuid = 0;
            message.packet.Read<u64>(playerGuid); 
            
            CharacterData characterData;
            if (characterDatabase.cache->GetCharacterData(playerGuid, characterData))
            {
                u32 entity = registry.create();
                registry.assign<PlayerConnectionComponent>(entity, entity, u32(message.account), playerGuid);
                registry.assign<PlayerInitializeComponent>(entity, u32(message.account), playerGuid);

                // -8949.950195f, -132.492996f, 83.531197f, 0.f
                registry.assign<PlayerUpdateDataComponent>(entity);
                /* 8.53332f is the amount of yards a player can move over a duration of 1.2 seconds with base speed,
                this should be updated in the future to include the player's actual speed upon login, such as the player being mounted up from a previous session */
                registry.assign<PlayerPositionComponent>(entity, characterData.mapId, characterData.coordinateX, characterData.coordinateY, characterData.coordinateZ, characterData.orientation, characterData.coordinateX, characterData.coordinateY, characterData.coordinateZ, characterData.orientation, 8.53332f);
                registry.assign<PlayerSpellStorageComponent>(entity);
                registry.assign<PlayerSkillStorageComponent>(entity);

                singleton.accountToEntityMap[u32(message.account)] = entity;
            }
        }
    }
}