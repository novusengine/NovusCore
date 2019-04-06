#pragma once
#include <NovusTypes.h>
#include <Utils/StringHash.h>
#include <../ECS/Components/Singletons/CommandDataSingleton.h>

#include "Commands_Character.h"

namespace Commands
{
    void LoadCommands(entt::registry& registry)
    {
        CommandDataSingleton& commandDataSingleton = registry.set<CommandDataSingleton>();
        Commands_Character::LoadCharacterCommands(registry, commandDataSingleton);
    }
}