#include "SpellLoader.h"
#include <Networking/ByteBuffer.h>
#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>
#include <filesystem>

#include "../ECS/Components/Singletons/DBCDatabaseCacheSingleton.h"
#include "../Scripting/AuraHooks.h"
#include "../Scripting/SpellHooks.h"

SpellLoader* SpellLoader::spellLoader = nullptr;

bool SpellLoader::Load(entt::registry& registry)
{
    DBCDatabaseCacheSingleton& dbcCache = registry.ctx<DBCDatabaseCacheSingleton>();
    auto& spellDatas = dbcCache.cache->GetSpellDatas();
    auto& auraEffectMap = AuraEffectHooks::GetEffectMap();
    auto& spellEffectMap = SpellEffectHooks::GetEffectMap();

    u32 numSpellEffects = 0;
    u32 numAuraEffects = 0;
    for (auto& spellData : spellDatas)
    {
        for (i32 i = 0; i < 3; i++)
        {
            u32 spellEffect = spellData.second.Effect[i];
            auto spellItr = spellEffectMap.find(spellEffect);
            if (spellItr == spellEffectMap.end())
            {
                spellEffectMap[spellEffect] = std::array<std::vector<asIScriptFunction*>, SpellEffectHooks::Hooks::COUNT>();
                numSpellEffects++;
            }

            u32 auraApplyName = spellData.second.EffectApplyAuraName[i];
            auto auraItr = auraEffectMap.find(auraApplyName);
            if (auraItr == auraEffectMap.end())
            {
                auraEffectMap[auraApplyName] = std::array<std::vector<asIScriptFunction*>, AuraEffectHooks::Hooks::COUNT>();
                numAuraEffects++;
            }
        }
    }

    NC_LOG_SUCCESS("Loaded %u spell effects", numSpellEffects);
    NC_LOG_SUCCESS("Loaded %u aura effects", numAuraEffects);
    return true;
}