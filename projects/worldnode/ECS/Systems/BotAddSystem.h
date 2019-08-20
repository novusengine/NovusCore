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
#include "../Components/PlayerVisualInfoComponent.h"
#include "../Components/UnitInfoComponent.h"
#include "../Components/AuraListComponent.h"
#include "../Components/ScriptDataStorageComponent.h"
#include "../Components/UnitStatsComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/GuidLookupSingleton.h"
#include "../Components/Singletons/BotCreateQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"
#include "../Components/Singletons/MapSingleton.h"

namespace BotAddSystem
{
void Update(entt::registry& registry)
{
    SingletonComponent& singleton = registry.ctx<SingletonComponent>();
    GuidLookupSingleton& guidLookupSingleton = registry.ctx<GuidLookupSingleton>();
    BotCreateQueueSingleton& createBotQueue = registry.ctx<BotCreateQueueSingleton>();
    CharacterDatabaseCacheSingleton& characterDatabase = registry.ctx<CharacterDatabaseCacheSingleton>();
    MapSingleton& mapSingleton = registry.ctx<MapSingleton>();

    BotCreateInfo botCreateInfo;
    while (createBotQueue.newBotQueue->try_dequeue(botCreateInfo))
    {
        CharacterInfo characterInfo = botCreateInfo.characterInfo;
        
        u32 entityId = registry.create();
        u32 accountId = characterInfo.account;
        ObjectGuid charGuid(characterInfo.guid);

        registry.assign<PlayerConnectionComponent>(entityId, entityId, accountId, charGuid, botCreateInfo.socket);
        registry.assign<PlayerInitializeComponent>(entityId, entityId, accountId, charGuid, botCreateInfo.socket);

        registry.assign<PlayerFieldDataComponent>(entityId);
        registry.assign<PlayerUpdateDataComponent>(entityId);

        u8 visualData = 0;
        registry.assign<PlayerVisualInfoComponent>(entityId, visualData, visualData, visualData, visualData, visualData);
        registry.assign<UnitInfoComponent>(entityId, characterInfo.name, characterInfo.race, characterInfo.gender, characterInfo.classId, characterInfo.level);

        PlayerPositionComponent& playerPositionComponent = registry.assign<PlayerPositionComponent>(entityId, characterInfo.mapId);
        playerPositionComponent.movementData.position = characterInfo.position;
        playerPositionComponent.movementData.orientation = characterInfo.orientation;
        Vector2 position = Vector2(playerPositionComponent.movementData.position.x, playerPositionComponent.movementData.position.y);

        UnitStatsComponent& unitStatsComponent = registry.assign<UnitStatsComponent>(entityId);
        unitStatsComponent.baseHealth = 1337.0f;
        unitStatsComponent.currentHealth = 1337.0f;
        unitStatsComponent.maxHealth = 1337.0f;

        for (int i = 0; i < POWER_COUNT; i++)
        {
            // TODO: Add some kind of class-mask so all classes don't use all resources
            switch (i)
            {
            case POWER_RAGE:
                unitStatsComponent.basePower[i] = 0.0f;
                unitStatsComponent.currentPower[i] = 0.0f;
                unitStatsComponent.maxPower[i] = 100.0f;
                break;
            default: // TODO: Handle other special types of resources
                unitStatsComponent.basePower[i] = 100.0f;
                unitStatsComponent.currentPower[i] = 100.0f;
                unitStatsComponent.maxPower[i] = 100.0f;
                break;
            }
        }

        for (int i = 0; i < STAT_COUNT; i++)
        {
            unitStatsComponent.baseStat[i] = 100;
            unitStatsComponent.currentStat[i] = 100;
        }

        for (int i = 0; i < RESISTANCE_COUNT; i++)
        {
            unitStatsComponent.baseResistance[i] = 100;
            unitStatsComponent.currentResistance[i] = 100;
        }

        u16 adtId = 0;
        if (mapSingleton.maps[playerPositionComponent.mapId].GetAdtIdFromWorldPosition(position, adtId))
        {
            playerPositionComponent.adtId = adtId;
            mapSingleton.maps[playerPositionComponent.mapId].playersInAdts[adtId].push_back(entityId);
        }

        registry.assign<PlayerSpellStorageComponent>(entityId);
        registry.assign<PlayerSkillStorageComponent>(entityId);

        registry.assign<ScriptDataStorageComponent>(entityId);
        registry.assign<AuraListComponent>(entityId, entityId, characterInfo.guid);

        singleton.accountToEntityMap[static_cast<u32>(characterInfo.account)] = entityId;
        guidLookupSingleton.playerToEntityMap[characterInfo.guid] = entityId;
    }
}
} // namespace BotAddSystem
