#pragma once
#include "BaseDatabaseCache.h"
#include <robin_hood.h>

// item_template table in DB
class DBCDatabaseCache;

struct MapData
{
    MapData() {}
    MapData(DBCDatabaseCache* cache) { _cache = cache; }
    MapData(const MapData& data)
    {
        id = data.id;
        internalName = data.internalName;
        instanceType = data.instanceType;
        flags = data.flags;
        name = data.name;
        expansion = data.expansion;
        maxPlayers = data.maxPlayers;
        _cache = data._cache;
    }

    u16 id;
    std::string internalName;
    u32 instanceType;
    u32 flags;
    std::string name;
    u32 expansion;
    u32 maxPlayers;

private:
    DBCDatabaseCache* _cache;
};

struct EmoteTextData
{
    EmoteTextData() {}
    EmoteTextData(DBCDatabaseCache* cache) { _cache = cache; }
    EmoteTextData(const EmoteTextData& data)
    {
        id = data.id;
        internalName = data.internalName;
        animationId = data.animationId;
    }

    u32 id;
    std::string internalName;
    u32 animationId;

private:
    DBCDatabaseCache* _cache;
};

#define SPELL_EFFECTS_COUNT 3
struct SpellData
{
    SpellData() {}
    SpellData(DBCDatabaseCache* cache) { _cache = cache; }
    SpellData(const SpellData& data)
    {
        Id = data.Id;
        SpellCategory = data.SpellCategory;
        DispelType = data.DispelType;
        Mechanic = data.Mechanic;
        Attributes = data.Attributes;
        AttributesExA = data.AttributesExA;
        AttributesExB = data.AttributesExB;
        AttributesExC = data.AttributesExC;
        AttributesExD = data.AttributesExD;
        AttributesExE = data.AttributesExE;
        AttributesExF = data.AttributesExF;
        AttributesExG = data.AttributesExG;
        StanceMask = data.StanceMask;
        StanceExcludeMask = data.StanceExcludeMask;
        Targets = data.Targets;
        TargetCreatureType = data.TargetCreatureType;
        SpellFocusObject = data.SpellFocusObject;
        FacingCasterFlags = data.FacingCasterFlags;
        CasterAuraState = data.CasterAuraState;
        TargetAuraState = data.TargetAuraState;
        CasterAuraStateNot = data.CasterAuraStateNot;
        TargetAuraStateNot = data.TargetAuraStateNot;
        CasterAuraSpell = data.CasterAuraSpell;
        TargetAuraSpell = data.TargetAuraSpell;
        ExcludeCasterAuraSpell = data.ExcludeCasterAuraSpell;
        ExcludeTargetAuraSpell = data.ExcludeTargetAuraSpell;
        CastingTimeIndex = data.CastingTimeIndex;
        RecoveryTime = data.RecoveryTime;
        CategoryRecoveryTime = data.CategoryRecoveryTime;
        InterruptFlags = data.InterruptFlags;
        AuraInterruptFlags = data.AuraInterruptFlags;
        ChannelInterruptFlags = data.ChannelInterruptFlags;
        ProcFlags = data.ProcFlags;
        ProcChance = data.ProcChance;
        ProcCharges = data.ProcCharges;
        MaxLevel = data.MaxLevel;
        BaseLevel = data.BaseLevel;
        SpellLevel = data.SpellLevel;
        DurationIndex = data.DurationIndex;
        PowerType = data.PowerType;
        ManaCost = data.ManaCost;
        ManaCostPerlevel = data.ManaCostPerlevel;
        ManaPerSecond = data.ManaPerSecond;
        ManaPerSecondPerLevel = data.ManaPerSecondPerLevel;
        RangeIndex = data.RangeIndex;
        Speed = data.Speed;
        ModalNextSpell = data.ModalNextSpell;
        StackAmount = data.StackAmount;
        Totem[0] = data.Totem[0];
        Totem[1] = data.Totem[1];
        Reagent[0] = data.Reagent[0];
        Reagent[1] = data.Reagent[1];
        Reagent[2] = data.Reagent[2];
        Reagent[3] = data.Reagent[3];
        Reagent[4] = data.Reagent[4];
        Reagent[5] = data.Reagent[5];
        Reagent[6] = data.Reagent[6];
        Reagent[7] = data.Reagent[7];
        ReagentCount[0] = data.ReagentCount[0];
        ReagentCount[1] = data.ReagentCount[1];
        ReagentCount[2] = data.ReagentCount[2];
        ReagentCount[3] = data.ReagentCount[3];
        ReagentCount[4] = data.ReagentCount[4];
        ReagentCount[5] = data.ReagentCount[5];
        ReagentCount[6] = data.ReagentCount[6];
        ReagentCount[7] = data.ReagentCount[7];
        EquippedItemClass = data.EquippedItemClass;
        EquippedItemSubClassMask = data.EquippedItemSubClassMask;
        EquippedItemInventoryTypeMask = data.EquippedItemInventoryTypeMask;
        Effect[0] = data.Effect[0];
        Effect[1] = data.Effect[1];
        Effect[2] = data.Effect[2];
        EffectDieSides[0] = data.EffectDieSides[0];
        EffectDieSides[1] = data.EffectDieSides[1];
        EffectDieSides[2] = data.EffectDieSides[2];
        EffectRealPointsPerLevel[0] = data.EffectRealPointsPerLevel[0];
        EffectRealPointsPerLevel[1] = data.EffectRealPointsPerLevel[1];
        EffectRealPointsPerLevel[2] = data.EffectRealPointsPerLevel[2];
        EffectBasePoints[0] = data.EffectBasePoints[0];
        EffectBasePoints[1] = data.EffectBasePoints[1];
        EffectBasePoints[2] = data.EffectBasePoints[2];
        EffectMechanic[0] = data.EffectMechanic[0];
        EffectMechanic[1] = data.EffectMechanic[1];
        EffectMechanic[2] = data.EffectMechanic[2];
        EffectImplicitTargetA[0] = data.EffectImplicitTargetA[0];
        EffectImplicitTargetA[1] = data.EffectImplicitTargetA[1];
        EffectImplicitTargetA[2] = data.EffectImplicitTargetA[2];
        EffectImplicitTargetB[0] = data.EffectImplicitTargetB[0];
        EffectImplicitTargetB[1] = data.EffectImplicitTargetB[1];
        EffectImplicitTargetB[2] = data.EffectImplicitTargetB[2];
        EffectRadiusIndex[0] = data.EffectRadiusIndex[0];
        EffectRadiusIndex[1] = data.EffectRadiusIndex[1];
        EffectRadiusIndex[2] = data.EffectRadiusIndex[2];
        EffectApplyAuraName[0] = data.EffectApplyAuraName[0];
        EffectApplyAuraName[1] = data.EffectApplyAuraName[1];
        EffectApplyAuraName[2] = data.EffectApplyAuraName[2];
        EffectAuraPeriod[0] = data.EffectAuraPeriod[0];
        EffectAuraPeriod[1] = data.EffectAuraPeriod[1];
        EffectAuraPeriod[2] = data.EffectAuraPeriod[2];
        EffectAmplitude[0] = data.EffectAmplitude[0];
        EffectAmplitude[1] = data.EffectAmplitude[1];
        EffectAmplitude[2] = data.EffectAmplitude[2];
        EffectChainTarget[0] = data.EffectChainTarget[0];
        EffectChainTarget[1] = data.EffectChainTarget[1];
        EffectChainTarget[2] = data.EffectChainTarget[2];
        EffectItemType[0] = data.EffectItemType[0];
        EffectItemType[1] = data.EffectItemType[1];
        EffectItemType[2] = data.EffectItemType[2];
        EffectMiscValueA[0] = data.EffectMiscValueA[0];
        EffectMiscValueA[1] = data.EffectMiscValueA[1];
        EffectMiscValueA[2] = data.EffectMiscValueA[2];
        EffectMiscValueB[0] = data.EffectMiscValueB[0];
        EffectMiscValueB[1] = data.EffectMiscValueB[1];
        EffectMiscValueB[2] = data.EffectMiscValueB[2];
        EffectTriggerSpell[0] = data.EffectTriggerSpell[0];
        EffectTriggerSpell[1] = data.EffectTriggerSpell[1];
        EffectTriggerSpell[2] = data.EffectTriggerSpell[2];
        EffectPointsPerComboPoint[0] = data.EffectPointsPerComboPoint[0];
        EffectPointsPerComboPoint[1] = data.EffectPointsPerComboPoint[1];
        EffectPointsPerComboPoint[2] = data.EffectPointsPerComboPoint[2];
        EffectSpellClassMaskA[0] = data.EffectSpellClassMaskA[0];
        EffectSpellClassMaskA[1] = data.EffectSpellClassMaskA[1];
        EffectSpellClassMaskA[2] = data.EffectSpellClassMaskA[2];
        EffectSpellClassMaskB[0] = data.EffectSpellClassMaskB[0];
        EffectSpellClassMaskB[1] = data.EffectSpellClassMaskB[1];
        EffectSpellClassMaskB[2] = data.EffectSpellClassMaskB[2];
        EffectSpellClassMaskC[0] = data.EffectSpellClassMaskC[0];
        EffectSpellClassMaskC[1] = data.EffectSpellClassMaskC[1];
        EffectSpellClassMaskC[2] = data.EffectSpellClassMaskC[2];
        SpellVisual[0] = data.SpellVisual[0];
        SpellVisual[1] = data.SpellVisual[1];
        SpellIconID = data.SpellIconID;
        ActiveIconID = data.ActiveIconID;
        SpellPriority = data.SpellPriority;
        SpellName = data.SpellName;
        SpellSubText = data.SpellSubText;
        ManaCostPercentage = data.ManaCostPercentage;
        StartRecoveryCategory = data.StartRecoveryCategory;
        StartRecoveryTime = data.StartRecoveryTime;
        MaxTargetLevel = data.MaxTargetLevel;
        SpellClassSet = data.SpellClassSet;
        SpellClassMask[0] = data.SpellClassMask[0];
        SpellClassMask[1] = data.SpellClassMask[1];
        SpellClassMask[2] = data.SpellClassMask[2];
        MaxTargets = data.MaxTargets;
        DefenseType = data.DefenseType;
        PreventionType = data.PreventionType;
        StanceBarOrder = data.StanceBarOrder;
        EffectChainAmplitude[0] = data.EffectChainAmplitude[0];
        EffectChainAmplitude[1] = data.EffectChainAmplitude[1];
        EffectChainAmplitude[2] = data.EffectChainAmplitude[2];
        MinFactionId = data.MinFactionId;
        MinReputation = data.MinReputation;
        RequiredAuraVision = data.RequiredAuraVision;
        TotemCategory[0] = data.TotemCategory[0];
        TotemCategory[1] = data.TotemCategory[1];
        RequiredAreaId = data.RequiredAreaId;
        SchoolMask = data.SchoolMask;
        RuneCostID = data.RuneCostID;
        SpellMissileID = data.SpellMissileID;
        PowerDisplayID = data.PowerDisplayID;
        EffectBonusMultiplier[0] = data.EffectBonusMultiplier[0];
        EffectBonusMultiplier[1] = data.EffectBonusMultiplier[1];
        EffectBonusMultiplier[2] = data.EffectBonusMultiplier[2];
        SpellDescriptionVariableID = data.SpellDescriptionVariableID;
        SpellDifficultyID = data.SpellDifficultyID;
    }

    u32 Id;
    u32 SpellCategory;
    u32 DispelType;
    u32 Mechanic;
    u32 Attributes;
    u32 AttributesExA;
    u32 AttributesExB;
    u32 AttributesExC;
    u32 AttributesExD;
    u32 AttributesExE;
    u32 AttributesExF;
    u32 AttributesExG;
    u64 StanceMask;
    u64 StanceExcludeMask;
    u32 Targets;
    u32 TargetCreatureType;
    u32 SpellFocusObject;
    u32 FacingCasterFlags;
    u32 CasterAuraState;
    u32 TargetAuraState;
    u32 CasterAuraStateNot;
    u32 TargetAuraStateNot;
    u32 CasterAuraSpell;
    u32 TargetAuraSpell;
    u32 ExcludeCasterAuraSpell;
    u32 ExcludeTargetAuraSpell;
    u32 CastingTimeIndex;
    u32 RecoveryTime;
    u32 CategoryRecoveryTime;
    u32 InterruptFlags;
    u32 AuraInterruptFlags;
    u32 ChannelInterruptFlags;
    u32 ProcFlags;
    u32 ProcChance;
    u32 ProcCharges;
    u32 MaxLevel;
    u32 BaseLevel;
    u32 SpellLevel;
    u32 DurationIndex;
    i32 PowerType;
    u32 ManaCost;
    u32 ManaCostPerlevel;
    u32 ManaPerSecond;
    u32 ManaPerSecondPerLevel;
    u32 RangeIndex;
    f32 Speed;
    u32 ModalNextSpell;
    u32 StackAmount;
    u32 Totem[2];
    i32 Reagent[8];
    i32 ReagentCount[8];
    i32 EquippedItemClass;
    i32 EquippedItemSubClassMask;
    i32 EquippedItemInventoryTypeMask;
    u32 Effect[3];
    i32 EffectDieSides[3];
    f32 EffectRealPointsPerLevel[3];
    i32 EffectBasePoints[3];
    u32 EffectMechanic[3];
    u32 EffectImplicitTargetA[3];
    u32 EffectImplicitTargetB[3];
    u32 EffectRadiusIndex[3];
    u32 EffectApplyAuraName[3];
    u32 EffectAuraPeriod[3];
    f32 EffectAmplitude[3];
    u32 EffectChainTarget[3];
    u32 EffectItemType[3];
    i32 EffectMiscValueA[3];
    i32 EffectMiscValueB[3];
    u32 EffectTriggerSpell[3];
    f32 EffectPointsPerComboPoint[3];
    u32 EffectSpellClassMaskA[3];
    u32 EffectSpellClassMaskB[3];
    u32 EffectSpellClassMaskC[3];
    u32 SpellVisual[2];
    u32 SpellIconID;
    u32 ActiveIconID;
    u32 SpellPriority;
    std::string SpellName;
    std::string SpellSubText;
    u32 ManaCostPercentage;
    u32 StartRecoveryCategory;
    u32 StartRecoveryTime;
    u32 MaxTargetLevel;
    u32 SpellClassSet;
    u32 SpellClassMask[3];
    u32 MaxTargets;
    u32 DefenseType;
    u32 PreventionType;
    u32 StanceBarOrder;
    f32 EffectChainAmplitude[3];
    u32 MinFactionId;
    u32 MinReputation;
    u32 RequiredAuraVision;
    u32 TotemCategory[2];
    i32 RequiredAreaId;
    u32 SchoolMask;
    u32 RuneCostID;
    u32 SpellMissileID;
    i32 PowerDisplayID;
    f32 EffectBonusMultiplier[3];
    u32 SpellDescriptionVariableID;
    u32 SpellDifficultyID;

private:
    DBCDatabaseCache* _cache;
};

class DBCDatabaseCache : BaseDatabaseCache
{
public:
    DBCDatabaseCache();
    ~DBCDatabaseCache();

    void Load() override;
    void LoadAsync() override;
    void Save() override;
    void SaveAsync() override;

    // Map Data cache
    bool GetMapData(u16 mapId, MapData& output);
    bool GetMapDataFromInternalName(std::string mapInternalName, MapData& output);

    // Emote Data cache
    bool GetEmoteTextData(u32 emoteTextId, EmoteTextData& output);

    // Spell Data cache
    bool GetSpellData(u32 spellId, SpellData& output);

private:
    robin_hood::unordered_map<u32, MapData> _mapDataCache;
    robin_hood::unordered_map<u32, EmoteTextData> _emoteTextDataCache;
    robin_hood::unordered_map<u32, SpellData> _spellDataCache;
};