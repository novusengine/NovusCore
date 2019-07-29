#include "SpellLoader.h"
#include <Networking/ByteBuffer.h>
#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>
#include <filesystem>

#include "../ECS/Components/Singletons/DBCDatabaseCacheSingleton.h"
#include "../Scripting/AuraHooks.h"

SpellLoader* SpellLoader::spellLoader = nullptr;

bool SpellLoader::Load(entt::registry& registry)
{
    DBCDatabaseCacheSingleton& dbcCache = registry.ctx<DBCDatabaseCacheSingleton>();
    auto& spellDatas = dbcCache.cache->GetSpellDatas();
    auto& effectMap = AuraEffectHooks::GetEffectMap();

    u32 biggestEffectId = 0;
    for (auto& spellData : spellDatas)
    {
        for (i32 i = 0; i < 3; i++)
        {
            u32 auraApplyName = spellData.second.EffectApplyAuraName[i];
            if (auraApplyName > biggestEffectId)
                biggestEffectId = auraApplyName;

            auto itr = effectMap.find(auraApplyName);
            if (itr != effectMap.end())
                continue;

            effectMap[auraApplyName] = std::array<std::vector<asIScriptFunction*>, AuraEffectHooks::Hooks::COUNT>();
        }
    }
    _auraEffectCount = biggestEffectId;

    NC_LOG_SUCCESS("Loaded %u aura effects", biggestEffectId);
    return true;
}