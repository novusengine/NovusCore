#include "DBCDatabaseCache.h"
#include <Database/DatabaseConnector.h>
#include <Database/PreparedStatement.h>

DBCDatabaseCache::DBCDatabaseCache()
{
}
DBCDatabaseCache::~DBCDatabaseCache()
{
}

void DBCDatabaseCache::Load()
{
    std::shared_ptr<DatabaseConnector> connector;
    bool result = DatabaseConnector::Borrow(DATABASE_TYPE::DBC, connector);
    assert(result);
    if (!result)
        return;

    amy::result_set resultSet;

    /* Map Data */
    connector->Query("SELECT * FROM map;", resultSet);
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            MapData mapData(this);
            mapData.id = row[0].GetU16();
            mapData.internalName = row[1].GetString();
            mapData.instanceType = row[2].GetU32();
            mapData.flags = row[3].GetU32();
            mapData.name = row[4].GetString();
            mapData.expansion = row[5].GetU32();
            mapData.maxPlayers = row[6].GetU32();

            _mapDataCache[mapData.id] = mapData;
        }
    }

    /* EmoteText Data */
    connector->Query("SELECT * FROM emotes_text;", resultSet);
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            EmoteTextData emoteTextData(this);
            emoteTextData.id = row[0].GetU32();
            emoteTextData.internalName = row[1].GetString();
            emoteTextData.animationId = row[2].GetU32();

            _emoteTextDataCache[emoteTextData.id] = emoteTextData;
        }
    }

    /* Spells Data */
    connector->Query("SELECT * FROM spells;", resultSet);
    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            SpellData spellData(this);
            spellData.Id = row[0].GetU32();
            spellData.SpellCategory = row[1].GetU32();
            spellData.DispelType = row[2].GetU32();
            spellData.Mechanic = row[3].GetU32();
            spellData.Attributes = row[4].GetU32();
            spellData.AttributesExA = row[5].GetU32();
            spellData.AttributesExB = row[6].GetU32();
            spellData.AttributesExC = row[7].GetU32();
            spellData.AttributesExD = row[8].GetU32();
            spellData.AttributesExE = row[9].GetU32();
            spellData.AttributesExF = row[10].GetU32();
            spellData.AttributesExG = row[11].GetU32();
            spellData.StanceMask = row[12].GetU64();
            spellData.StanceExcludeMask = row[13].GetU64();
            spellData.Targets = row[14].GetU32();
            spellData.TargetCreatureType = row[15].GetU32();
            spellData.SpellFocusObject = row[16].GetU32();
            spellData.FacingCasterFlags = row[17].GetU32();
            spellData.CasterAuraState = row[18].GetU32();
            spellData.TargetAuraState = row[19].GetU32();
            spellData.CasterAuraStateNot = row[20].GetU32();
            spellData.TargetAuraStateNot = row[21].GetU32();
            spellData.CasterAuraSpell = row[22].GetU32();
            spellData.TargetAuraSpell = row[23].GetU32();
            spellData.ExcludeCasterAuraSpell = row[24].GetU32();
            spellData.ExcludeTargetAuraSpell = row[25].GetU32();
            spellData.CastingTimeIndex = row[26].GetU32();
            spellData.RecoveryTime = row[27].GetU32();
            spellData.CategoryRecoveryTime = row[28].GetU32();
            spellData.InterruptFlags = row[29].GetU32();
            spellData.AuraInterruptFlags = row[30].GetU32();
            spellData.ChannelInterruptFlags = row[31].GetU32();
            spellData.ProcFlags = row[32].GetU32();
            spellData.ProcChance = row[33].GetU32();
            spellData.ProcCharges = row[34].GetU32();
            spellData.MaxLevel = row[35].GetU32();
            spellData.BaseLevel = row[36].GetU32();
            spellData.SpellLevel = row[37].GetU32();
            spellData.DurationIndex = row[38].GetU32();
            spellData.PowerType = row[39].GetI32();
            spellData.ManaCost = row[40].GetU32();
            spellData.ManaCostPerlevel = row[41].GetU32();
            spellData.ManaPerSecond = row[42].GetU32();
            spellData.ManaPerSecondPerLevel = row[43].GetU32();
            spellData.RangeIndex = row[44].GetU32();
            spellData.Speed = row[45].GetF32();
            spellData.ModalNextSpell = row[46].GetU32();
            spellData.StackAmount = row[47].GetU32();
            spellData.Totem[0] = row[48].GetU32();
            spellData.Totem[1] = row[49].GetU32();
            spellData.Reagent[0] = row[50].GetI32();
            spellData.Reagent[1] = row[51].GetI32();
            spellData.Reagent[2] = row[52].GetI32();
            spellData.Reagent[3] = row[53].GetI32();
            spellData.Reagent[4] = row[54].GetI32();
            spellData.Reagent[5] = row[55].GetI32();
            spellData.Reagent[6] = row[56].GetI32();
            spellData.Reagent[7] = row[57].GetI32();
            spellData.ReagentCount[0] = row[58].GetI32();
            spellData.ReagentCount[1] = row[59].GetI32();
            spellData.ReagentCount[2] = row[60].GetI32();
            spellData.ReagentCount[3] = row[61].GetI32();
            spellData.ReagentCount[4] = row[62].GetI32();
            spellData.ReagentCount[5] = row[63].GetI32();
            spellData.ReagentCount[6] = row[64].GetI32();
            spellData.ReagentCount[7] = row[65].GetI32();
            spellData.EquippedItemClass = row[66].GetI32();
            spellData.EquippedItemSubClassMask = row[67].GetI32();
            spellData.EquippedItemInventoryTypeMask = row[68].GetI32();
            spellData.Effect[0] = row[69].GetU32();
            spellData.Effect[1] = row[70].GetU32();
            spellData.Effect[2] = row[71].GetU32();
            spellData.EffectDieSides[0] = row[72].GetI32();
            spellData.EffectDieSides[1] = row[73].GetI32();
            spellData.EffectDieSides[2] = row[74].GetI32();
            spellData.EffectRealPointsPerLevel[0] = row[75].GetF32();
            spellData.EffectRealPointsPerLevel[1] = row[76].GetF32();
            spellData.EffectRealPointsPerLevel[2] = row[77].GetF32();
            spellData.EffectBasePoints[0] = row[78].GetI32();
            spellData.EffectBasePoints[1] = row[79].GetI32();
            spellData.EffectBasePoints[2] = row[80].GetI32();
            spellData.EffectMechanic[0] = row[81].GetU32();
            spellData.EffectMechanic[1] = row[82].GetU32();
            spellData.EffectMechanic[2] = row[83].GetU32();
            spellData.EffectImplicitTargetA[0] = row[84].GetU32();
            spellData.EffectImplicitTargetA[1] = row[85].GetU32();
            spellData.EffectImplicitTargetA[2] = row[86].GetU32();
            spellData.EffectImplicitTargetB[0] = row[87].GetU32();
            spellData.EffectImplicitTargetB[1] = row[88].GetU32();
            spellData.EffectImplicitTargetB[2] = row[89].GetU32();
            spellData.EffectRadiusIndex[0] = row[90].GetU32();
            spellData.EffectRadiusIndex[1] = row[91].GetU32();
            spellData.EffectRadiusIndex[2] = row[92].GetU32();
            spellData.EffectApplyAuraName[0] = row[93].GetU32();
            spellData.EffectApplyAuraName[1] = row[94].GetU32();
            spellData.EffectApplyAuraName[2] = row[95].GetU32();
            spellData.EffectAuraPeriod[0] = row[96].GetU32();
            spellData.EffectAuraPeriod[1] = row[97].GetU32();
            spellData.EffectAuraPeriod[2] = row[98].GetU32();
            spellData.EffectAmplitude[0] = row[99].GetF32();
            spellData.EffectAmplitude[1] = row[100].GetF32();
            spellData.EffectAmplitude[2] = row[101].GetF32();
            spellData.EffectChainTarget[0] = row[102].GetU32();
            spellData.EffectChainTarget[1] = row[103].GetU32();
            spellData.EffectChainTarget[2] = row[104].GetU32();
            spellData.EffectItemType[0] = row[105].GetU32();
            spellData.EffectItemType[1] = row[106].GetU32();
            spellData.EffectItemType[2] = row[107].GetU32();
            spellData.EffectMiscValueA[0] = row[108].GetI32();
            spellData.EffectMiscValueA[1] = row[109].GetI32();
            spellData.EffectMiscValueA[2] = row[110].GetI32();
            spellData.EffectMiscValueB[0] = row[111].GetI32();
            spellData.EffectMiscValueB[1] = row[112].GetI32();
            spellData.EffectMiscValueB[2] = row[113].GetI32();
            spellData.EffectTriggerSpell[0] = row[114].GetU32();
            spellData.EffectTriggerSpell[1] = row[115].GetU32();
            spellData.EffectTriggerSpell[2] = row[116].GetU32();
            spellData.EffectPointsPerComboPoint[0] = row[117].GetF32();
            spellData.EffectPointsPerComboPoint[1] = row[118].GetF32();
            spellData.EffectPointsPerComboPoint[2] = row[119].GetF32();
            spellData.EffectSpellClassMaskA[0] = row[120].GetU32();
            spellData.EffectSpellClassMaskA[1] = row[121].GetU32();
            spellData.EffectSpellClassMaskA[2] = row[122].GetU32();
            spellData.EffectSpellClassMaskB[0] = row[123].GetU32();
            spellData.EffectSpellClassMaskB[1] = row[124].GetU32();
            spellData.EffectSpellClassMaskB[2] = row[125].GetU32();
            spellData.EffectSpellClassMaskC[0] = row[126].GetU32();
            spellData.EffectSpellClassMaskC[1] = row[127].GetU32();
            spellData.EffectSpellClassMaskC[2] = row[128].GetU32();
            spellData.SpellVisual[0] = row[129].GetU32();
            spellData.SpellVisual[1] = row[130].GetU32();
            spellData.SpellIconID = row[131].GetU32();
            spellData.ActiveIconID = row[132].GetU32();
            spellData.SpellPriority = row[133].GetU32();
            spellData.SpellName = row[134].GetString();
            spellData.SpellSubText = row[135].GetString();
            spellData.ManaCostPercentage = row[136].GetU32();
            spellData.StartRecoveryCategory = row[137].GetU32();
            spellData.StartRecoveryTime = row[138].GetU32();
            spellData.MaxTargetLevel = row[139].GetU32();
            spellData.SpellClassSet = row[140].GetU32();
            spellData.SpellClassMask[0] = row[141].GetU32();
            spellData.SpellClassMask[1] = row[142].GetU32();
            spellData.SpellClassMask[2] = row[143].GetU32();
            spellData.MaxTargets = row[144].GetU32();
            spellData.DefenseType = row[145].GetU32();
            spellData.PreventionType = row[146].GetU32();
            spellData.StanceBarOrder = row[147].GetU32();
            spellData.EffectChainAmplitude[0] = row[148].GetF32();
            spellData.EffectChainAmplitude[1] = row[149].GetF32();
            spellData.EffectChainAmplitude[2] = row[150].GetF32();
            spellData.MinFactionId = row[151].GetU32();
            spellData.MinReputation = row[152].GetU32();
            spellData.RequiredAuraVision = row[153].GetU32();
            spellData.TotemCategory[0] = row[154].GetU32();
            spellData.TotemCategory[1] = row[155].GetU32();
            spellData.RequiredAreaId = row[156].GetI32();
            spellData.SchoolMask = row[157].GetU32();
            spellData.RuneCostID = row[158].GetU32();
            spellData.SpellMissileID = row[159].GetU32();
            spellData.PowerDisplayID = row[160].GetI32();
            spellData.EffectBonusMultiplier[0] = row[161].GetF32();
            spellData.EffectBonusMultiplier[1] = row[162].GetF32();
            spellData.EffectBonusMultiplier[2] = row[163].GetF32();
            spellData.SpellDescriptionVariableID = row[164].GetU32();
            spellData.SpellDifficultyID = row[165].GetU32();

            _spellDataCache[spellData.Id] = spellData;
        }
    }

}
void DBCDatabaseCache::LoadAsync()
{
}

void DBCDatabaseCache::Save()
{
}
void DBCDatabaseCache::SaveAsync()
{
}

bool DBCDatabaseCache::GetMapData(u16 mapId, MapData& output)
{
    auto cache = _mapDataCache.find(mapId);
    if (cache != _mapDataCache.end())
    {
        _accessMutex.lock_shared();
        MapData mapData = cache->second;
        _accessMutex.unlock_shared();

        output = mapData;
        return true;
    }

    return false;
}
bool DBCDatabaseCache::GetMapDataFromInternalName(std::string internalName, MapData& output)
{
    _accessMutex.lock_shared();
    for (auto mapData : _mapDataCache)
    {
        if (mapData.second.internalName == internalName)
        {
            output = mapData.second;
            _accessMutex.unlock_shared();
            return true;
        }
    }
    _accessMutex.unlock_shared();

    return false;
}

bool DBCDatabaseCache::GetEmoteTextData(u32 emoteTextId, EmoteTextData& output)
{
    auto cache = _emoteTextDataCache.find(emoteTextId);
    if (cache != _emoteTextDataCache.end())
    {
        _accessMutex.lock_shared();
        output = cache->second;
        _accessMutex.unlock_shared();
        return true;
    }

    return false;
}
bool DBCDatabaseCache::GetSpellData(u32 spellId, SpellData& output)
{
    auto cache = _spellDataCache.find(spellId);
    if (cache != _spellDataCache.end())
    {
        _accessMutex.lock_shared();
        output = cache->second;
        _accessMutex.unlock_shared();
        return true;
    }

    return false;
}