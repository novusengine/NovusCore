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
#include <Database/Cache/CharacterDatabaseCache.h>

#include "../../Message.h"

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerFieldDataComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/PlayerPositionComponent.h"
#include "../Components/PlayerSpellStorageComponent.h"
#include "../Components/PlayerSkillStorageComponent.h"
#include "../Components/PlayerInitializeComponent.h"
#include "../Components/AuraListComponent.h"
#include "../Components/ScriptDataStorageComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/GuidLookupSingleton.h"
#include "../Components/Singletons/PlayerCreateQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"

namespace PlayerAddSystem
{
void Update(entt::registry& registry)
{
    SingletonComponent& singleton = registry.ctx<SingletonComponent>();
    GuidLookupSingleton& guidLookupSingleton = registry.ctx<GuidLookupSingleton>();
    PlayerCreateQueueSingleton& createPlayerQueue = registry.ctx<PlayerCreateQueueSingleton>();
    CharacterDatabaseCacheSingleton& characterDatabase = registry.ctx<CharacterDatabaseCacheSingleton>();
    MapSingleton& mapSingleton = registry.ctx<MapSingleton>();

    Message message;
    while (createPlayerQueue.newPlayerQueue->try_dequeue(message))
    {
        u64 characterGuid = 0;
        message.packet->Get<u64>(characterGuid);

        CharacterInfo characterInfo;
        if (characterDatabase.cache->GetCharacterInfo(characterGuid, characterInfo))
        {
            u32 entityId = registry.create();
            u32 accountId = static_cast<u32>(message.account);
            ObjectGuid charGuid(characterGuid);

            registry.assign<PlayerConnectionComponent>(entityId, entityId, accountId, charGuid, message.connection);
            registry.assign<PlayerInitializeComponent>(entityId, entityId, accountId, charGuid, message.connection);

            registry.assign<PlayerFieldDataComponent>(entityId);
            registry.assign<PlayerUpdateDataComponent>(entityId);

            PlayerPositionComponent& playerPositionComponent = registry.assign<PlayerPositionComponent>(entityId, characterInfo.mapId);
            playerPositionComponent.movementData.position = characterInfo.position;
            playerPositionComponent.movementData.orientation = characterInfo.orientation;
            Vector2 position = Vector2(playerPositionComponent.movementData.position.x, playerPositionComponent.movementData.position.y);

            u16 adtId = 0;
            if (mapSingleton.maps[playerPositionComponent.mapId].GetAdtIdFromWorldPosition(position, adtId))
            {
                playerPositionComponent.adtId = adtId;
                mapSingleton.maps[playerPositionComponent.mapId].playersInAdts[adtId].push_back(entityId);
            }

            registry.assign<PlayerSpellStorageComponent>(entityId);
            registry.assign<PlayerSkillStorageComponent>(entityId);

            registry.assign<ScriptDataStorageComponent>(entityId);
            registry.assign<AuraListComponent>(entityId, characterGuid);

            singleton.accountToEntityMap[static_cast<u32>(message.account)] = entityId;
            guidLookupSingleton.playerToEntityMap[characterGuid] = entityId;
        }
    }
}
} // namespace PlayerAddSystem
