#include "SpellLoader.h"
#include <Networking/ByteBuffer.h>
#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>
#include <filesystem>

#include "../ECS/Components/Singletons/DBCDatabaseCacheSingleton.h"

SpellLoader* SpellLoader::spellLoader = nullptr;

bool SpellLoader::Load(entt::registry& registry)
{
    DBCDatabaseCacheSingleton& dbcCache = registry.ctx<DBCDatabaseCacheSingleton>();
    auto& spellDatas = dbcCache.cache->GetSpellDatas();

    u32 biggestEffectId = 0;
    for (auto& spellData : spellDatas)
    {
        if (spellData.second.EffectApplyAuraName[0] > biggestEffectId)
            biggestEffectId = spellData.second.EffectApplyAuraName[0];
        if (spellData.second.EffectApplyAuraName[1] > biggestEffectId)
            biggestEffectId = spellData.second.EffectApplyAuraName[1];
        if (spellData.second.EffectApplyAuraName[2] > biggestEffectId)
            biggestEffectId = spellData.second.EffectApplyAuraName[2];
    }
    _auraEffectCount = biggestEffectId;

    NC_LOG_SUCCESS("Loaded %u aura effects", biggestEffectId);
    return true;
}