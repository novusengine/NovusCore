#include "BotUtils.h"
#include "../ECS/Components/Singletons/BotCreateQueueSingleton.h"
#include "../Utils/ServiceLocator.h"

#include <Database/Cache/CharacterDatabaseCache.h>
#include <entt.hpp>

void BotUtils::CreateBot(u16 mapId, Vector3 position, f32 orientation)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    BotCreateQueueSingleton& botCreateQueueSingleton = registry->ctx<BotCreateQueueSingleton>();

    BotCreateInfo botCreateInfo;
    botCreateInfo.characterInfo.guid = botCreateQueueSingleton.GetNewGuidForBot();
    botCreateInfo.characterInfo.account = 0;
    botCreateInfo.characterInfo.name = "Bot1";
    botCreateInfo.characterInfo.race = 1;
    botCreateInfo.characterInfo.gender = 1;
    botCreateInfo.characterInfo.classId = 1;
    botCreateInfo.characterInfo.level = 1;
    botCreateInfo.characterInfo.mapId = mapId;
    botCreateInfo.characterInfo.position = position;
    botCreateInfo.characterInfo.orientation = orientation;
    botCreateInfo.characterInfo.online = 1;
    botCreateInfo.socket = new WorldConnection(nullptr, nullptr, true);


    botCreateQueueSingleton.newBotQueue->enqueue(botCreateInfo);
}