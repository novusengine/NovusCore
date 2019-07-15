/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/
#pragma once
#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>
#include <sstream>
#include "../MPQ/MPQHandler.h"
#include "DBCReader.h"
#include "DBCStructures.h"

namespace DBCLoader
{
bool LoadMap(MPQHandler& handler, std::string& sqlOutput, std::vector<std::string>& adtLocationOutput)
{
    MPQFile file;
    if (handler.GetFile("DBFilesClient\\Map.dbc", file))
    {
        NC_LOG_MESSAGE("Loading Map.dbc...");
        if (DBCReader* dbcReader = DBCReader::GetReader())
        {
            if (dbcReader->Load(file.Buffer) == 0)
            {
                u32 rows = dbcReader->GetNumRows();
                if (rows == 0)
                    return false;

                std::stringstream ss;
                ss << "DELETE FROM map;" << std::endl
                   << "INSERT INTO map(id, internalName, instanceType, flags, name, expansion, maxPlayers) VALUES";

                for (u32 i = 0; i < rows; i++)
                {
                    auto row = dbcReader->GetRow(i);
                    //u32 fieldCount = dbcReader->GetNumFields();
                    DBCMap map;
                    map.Id = row.GetUInt32(0);
                    map.InternalName = StringUtils::EscapeString(row.GetString(row.GetUInt32(1)));
                    map.InstanceType = row.GetUInt32(2);
                    map.Flags = row.GetUInt32(3);
                    map.Name = StringUtils::EscapeString(row.GetString(row.GetUInt32(5)));
                    map.Expansion = row.GetUInt32(63);
                    map.MaxPlayers = row.GetUInt32(65);

                    // MapFlag 2 & 16, seem to be exclusive to Test / Development Maps
                    if ((map.Flags & 2) == 0 && (map.Flags & 16) == 0)
                        adtLocationOutput.push_back(row.GetString(row.GetUInt32(1)));

                    if (i != 0)
                        ss << ", ";

                    ss << "(" << map.Id << ", '" << map.InternalName << "', " << map.InstanceType << ", " << map.Flags << ", '" << map.Name << "', " << map.Expansion << ", " << map.MaxPlayers << ")";
                }

                ss << ";" << std::endl;
                sqlOutput += ss.str();
            }
        }
    }
    else
    {
        NC_LOG_ERROR("Failed to load Map.dbc");
        return false;
    }

    return true;
}

bool LoadEmotesText(MPQHandler& handler, std::string& sqlOutput)
{
    MPQFile file;
    if (handler.GetFile("DBFilesClient\\EmotesText.dbc", file))
    {
        NC_LOG_MESSAGE("Loading EmotesText.dbc...");

        if (DBCReader* dbcReader = DBCReader::GetReader())
        {
            if (dbcReader->Load(file.Buffer) == 0)
            {
                u32 rows = dbcReader->GetNumRows();
                if (rows == 0)
                    return false;

                std::stringstream ss;
                ss << "DELETE FROM emotes_text;" << std::endl
                   << "INSERT INTO emotes_text(id, internalName, animationId) VALUES";

                for (u32 i = 0; i < rows; i++)
                {
                    auto row = dbcReader->GetRow(i);

                    DBCEmotesText emoteText;
                    emoteText.Id = row.GetUInt32(0);
                    emoteText.InternalName = row.GetString(row.GetUInt32(1));
                    emoteText.AnimationId = row.GetUInt32(2);

                    if (i != 0)
                        ss << ", ";

                    ss << "(" << emoteText.Id << ", '" << emoteText.InternalName << "', " << emoteText.AnimationId << ")";
                }

                ss << ";" << std::endl;
                sqlOutput += ss.str();
            }
        }
    }
    else
    {
        NC_LOG_ERROR("Failed to load EmotesText.dbc");
        return false;
    }

    return true;
}

std::string GetNameFromUnknownLocale(DBCReader::DBCRow& row, u32 index, u32 num)
{
    for (int i = 0; i < num; i++)
    {
        u32 rowIndex = index + i;
        u32 nameIndex = row.GetUInt32(rowIndex);

        if (nameIndex != 0)
        {
            return row.GetString(nameIndex);
        }
    }

    return "UNKNOWN";
}

bool LoadSpell(MPQHandler& handler, std::string& sqlOutput)
{
    MPQFile file;
    if (handler.GetFile("DBFilesClient\\Spell.dbc", file))
    {
        NC_LOG_MESSAGE("Loading Spell.dbc...");

        if (DBCReader* dbcReader = DBCReader::GetReader())
        {
            if (dbcReader->Load(file.Buffer) == 0)
            {
                u32 rows = dbcReader->GetNumRows();
                if (rows == 0)
                    return false;

                std::stringstream ss;
                ss << "DELETE FROM spells;" << std::endl
                   << "INSERT INTO spells(id, spellCategory, dispelType, mechanic, attributes, attributesExA, attributesExB, attributesExC, attributesExD, attributesExE, attributesExF, attributesExG, stances, excludedStances, targets,"
                      "targetCreatureType, spellFocusObject, facingCasterFlags, casterAuraState, targetAuraState, casterAuraStateNot, targetAuraStateNot, casterAuraSpell, targetAuraSpell, excludeCasterAuraSpell, excludeTargetAuraSpell,"
                      "castingTimeIndex, recoveryTime, categoryRecoveryTime, interruptFlags, auraInterruptFlags, channelInterruptFlags, procFlags, procChance, procCharges, maxLevel, baseLevel, spellLevel, durationIndex, powerType, manaCost,"
                      "manaCostPerlevel, manaPerSecond, manaPerSecondPerLevel, rangeIndex, speed, modalNextSpell, stackAmount, totem1, totem2, reagent1, reagent2, reagent3, reagent4, reagent5, reagent6, reagent7, reagent8, reagentCount1,"
                      "reagentCount2, reagentCount3, reagentCount4, reagentCount5, reagentCount6, reagentCount7, reagentCount8, equippedItemClass, equippedItemSubClassMask, equippedItemInventoryTypeMask, effect1, effect2, effect3,"
                      "effectDieSides1, effectDieSides2, effectDieSides3, effectRealPointsPerLevel1, effectRealPointsPerLevel2, effectRealPointsPerLevel3, effectBasePoints1, effectBasePoints2, effectBasePoints3,"
                      "effectMechanic1, effectMechanic2, effectMechanic3, effectImplicitTargetA1, effectImplicitTargetA2, effectImplicitTargetA3, effectImplicitTargetB1, effectImplicitTargetB2, effectImplicitTargetB3,"
                      "effectRadiusIndex1, effectRadiusIndex2, effectRadiusIndex3, effectApplyAuraName1, effectApplyAuraName2, effectApplyAuraName3, effectAuraPeriod1, effectAuraPeriod2, effectAuraPeriod3,"
                      "effectAmplitude1, effectAmplitude2, effectAmplitude3, effectChainTarget1, effectChainTarget2, effectChainTarget3, effectItemType1, effectItemType2, effectItemType3, effectMiscValueA1, effectMiscValueA2, effectMiscValueA3,"
                      "effectMiscValueB1, effectMiscValueB2, effectMiscValueB3, effectTriggerSpell1, effectTriggerSpell2, effectTriggerSpell3, effectPointsPerComboPoint1, effectPointsPerComboPoint2, effectPointsPerComboPoint3,"
                      "effectSpellClassMaskA1, effectSpellClassMaskA2, effectSpellClassMaskA3, effectSpellClassMaskB1, effectSpellClassMaskB2, effectSpellClassMaskB3, effectSpellClassMaskC1, effectSpellClassMaskC2, effectSpellClassMaskC3,"
                      "spellVisual1, spellVisual2, spellIconID, activeIconID, spellPriority, spellName, spellSubText, manaCostPercentage, startRecoveryCategory, startRecoveryTime, maxTargetLevel, spellClassSet,"
                      "spellClassMask1, spellClassMask2, spellClassMask3, maxTargets, defenseType, preventionType, stanceBarOrder, effectChainAmplitude0, effectChainAmplitude1, effectChainAmplitude2, minFactionId, minReputation, requiredAuraVision,"
                      "totemCategory1, totemCategory2, requiredAreaId, schoolMask, runeCostID, spellMissileID, powerDisplayID, effectBonusMultiplier1, effectBonusMultiplier2, effectBonusMultiplier3, spellDescriptionVariableID, spellDifficultyID"
                      ") VALUES";

                DBCSpell spell;
                for (u32 i = 0; i < rows; i++)
                {
                    auto row = dbcReader->GetRow(i);
                    spell.Id = row.GetUInt32(0);
                    spell.SpellCategory = row.GetUInt32(1);
                    spell.DispelType = row.GetUInt32(2);
                    spell.Mechanic = row.GetUInt32(3);
                    spell.Attributes = row.GetUInt32(4);
                    spell.AttributesExA = row.GetUInt32(5);
                    spell.AttributesExB = row.GetUInt32(6);
                    spell.AttributesExC = row.GetUInt32(7);
                    spell.AttributesExD = row.GetUInt32(8);
                    spell.AttributesExE = row.GetUInt32(9);
                    spell.AttributesExF = row.GetUInt32(10);
                    spell.AttributesExG = row.GetUInt32(11);
                    spell.Stances = row.GetUInt64(12);         // This is 8 bytes wide, so skip 13
                    spell.ExcludedStances = row.GetUInt64(14); // This is 8 bytes wide, so skip 15
                    spell.Targets = row.GetUInt32(16);
                    spell.TargetCreatureType = row.GetUInt32(17);
                    spell.SpellFocusObject = row.GetUInt32(18);
                    spell.FacingCasterFlags = row.GetUInt32(19);
                    spell.CasterAuraState = row.GetUInt32(20);
                    spell.TargetAuraState = row.GetUInt32(21);
                    spell.CasterAuraStateNot = row.GetUInt32(22);
                    spell.TargetAuraStateNot = row.GetUInt32(23);
                    spell.CasterAuraSpell = row.GetUInt32(24);
                    spell.TargetAuraSpell = row.GetUInt32(25);
                    spell.ExcludeCasterAuraSpell = row.GetUInt32(26);
                    spell.ExcludeTargetAuraSpell = row.GetUInt32(27);
                    spell.CastingTimeIndex = row.GetUInt32(28);
                    spell.RecoveryTime = row.GetUInt32(29);
                    spell.CategoryRecoveryTime = row.GetUInt32(30);
                    spell.InterruptFlags = row.GetUInt32(31);
                    spell.AuraInterruptFlags = row.GetUInt32(32);
                    spell.ChannelInterruptFlags = row.GetUInt32(33);
                    spell.ProcFlags = row.GetUInt32(34);
                    spell.ProcChance = row.GetUInt32(35);
                    spell.ProcCharges = row.GetUInt32(36);
                    spell.MaxLevel = row.GetUInt32(37);
                    spell.BaseLevel = row.GetUInt32(38);
                    spell.SpellLevel = row.GetUInt32(39);
                    spell.DurationIndex = row.GetUInt32(40);
                    spell.PowerType = row.GetInt32(41);
                    spell.ManaCost = row.GetUInt32(42);
                    spell.ManaCostPerlevel = row.GetUInt32(43);
                    spell.ManaPerSecond = row.GetUInt32(44);
                    spell.ManaPerSecondPerLevel = row.GetUInt32(45);
                    spell.RangeIndex = row.GetUInt32(46);
                    spell.Speed = row.GetFloat(47);
                    spell.ModalNextSpell = row.GetUInt32(48);
                    spell.StackAmount = row.GetUInt32(49);
                    spell.Totem[0] = row.GetUInt32(50);
                    spell.Totem[1] = row.GetUInt32(51);
                    spell.Reagent[0] = row.GetInt32(52);
                    spell.Reagent[1] = row.GetInt32(53);
                    spell.Reagent[2] = row.GetInt32(54);
                    spell.Reagent[3] = row.GetInt32(55);
                    spell.Reagent[4] = row.GetInt32(56);
                    spell.Reagent[5] = row.GetInt32(57);
                    spell.Reagent[6] = row.GetInt32(58);
                    spell.Reagent[7] = row.GetInt32(59);
                    spell.ReagentCount[0] = row.GetInt32(60);
                    spell.ReagentCount[1] = row.GetInt32(61);
                    spell.ReagentCount[2] = row.GetInt32(62);
                    spell.ReagentCount[3] = row.GetInt32(63);
                    spell.ReagentCount[4] = row.GetInt32(64);
                    spell.ReagentCount[5] = row.GetInt32(65);
                    spell.ReagentCount[6] = row.GetInt32(66);
                    spell.ReagentCount[7] = row.GetInt32(67);
                    spell.EquippedItemClass = row.GetInt32(68);
                    spell.EquippedItemSubClassMask = row.GetInt32(69);
                    spell.EquippedItemInventoryTypeMask = row.GetInt32(70);
                    spell.Effect[0] = row.GetUInt32(71);
                    spell.Effect[1] = row.GetUInt32(72);
                    spell.Effect[2] = row.GetUInt32(73);
                    spell.EffectDieSides[0] = row.GetInt32(74);
                    spell.EffectDieSides[1] = row.GetInt32(75);
                    spell.EffectDieSides[2] = row.GetInt32(76);
                    spell.EffectRealPointsPerLevel[0] = row.GetFloat(77);
                    spell.EffectRealPointsPerLevel[1] = row.GetFloat(78);
                    spell.EffectRealPointsPerLevel[2] = row.GetFloat(79);
                    spell.EffectBasePoints[0] = row.GetInt32(80);
                    spell.EffectBasePoints[1] = row.GetInt32(81);
                    spell.EffectBasePoints[2] = row.GetInt32(82);
                    spell.EffectMechanic[0] = row.GetUInt32(83);
                    spell.EffectMechanic[1] = row.GetUInt32(84);
                    spell.EffectMechanic[2] = row.GetUInt32(85);
                    spell.EffectImplicitTargetA[0] = row.GetUInt32(86);
                    spell.EffectImplicitTargetA[1] = row.GetUInt32(87);
                    spell.EffectImplicitTargetA[2] = row.GetUInt32(88);
                    spell.EffectImplicitTargetB[0] = row.GetUInt32(89);
                    spell.EffectImplicitTargetB[1] = row.GetUInt32(90);
                    spell.EffectImplicitTargetB[2] = row.GetUInt32(91);
                    spell.EffectRadiusIndex[0] = row.GetUInt32(92);
                    spell.EffectRadiusIndex[1] = row.GetUInt32(93);
                    spell.EffectRadiusIndex[2] = row.GetUInt32(94);
                    spell.EffectApplyAuraName[0] = row.GetUInt32(95);
                    spell.EffectApplyAuraName[1] = row.GetUInt32(96);
                    spell.EffectApplyAuraName[2] = row.GetUInt32(97);
                    spell.EffectAuraPeriod[0] = row.GetUInt32(98);
                    spell.EffectAuraPeriod[1] = row.GetUInt32(99);
                    spell.EffectAuraPeriod[2] = row.GetUInt32(100);
                    spell.EffectAmplitude[0] = row.GetFloat(101);
                    spell.EffectAmplitude[1] = row.GetFloat(102);
                    spell.EffectAmplitude[2] = row.GetFloat(103);
                    spell.EffectChainTarget[0] = row.GetUInt32(104);
                    spell.EffectChainTarget[1] = row.GetUInt32(105);
                    spell.EffectChainTarget[2] = row.GetUInt32(106);
                    spell.EffectItemType[0] = row.GetUInt32(107);
                    spell.EffectItemType[1] = row.GetUInt32(108);
                    spell.EffectItemType[2] = row.GetUInt32(109);
                    spell.EffectMiscValueA[0] = row.GetInt32(110);
                    spell.EffectMiscValueA[1] = row.GetInt32(111);
                    spell.EffectMiscValueA[2] = row.GetInt32(112);
                    spell.EffectMiscValueB[0] = row.GetInt32(113);
                    spell.EffectMiscValueB[1] = row.GetInt32(114);
                    spell.EffectMiscValueB[2] = row.GetInt32(115);
                    spell.EffectTriggerSpell[0] = row.GetUInt32(116);
                    spell.EffectTriggerSpell[1] = row.GetUInt32(117);
                    spell.EffectTriggerSpell[2] = row.GetUInt32(118);
                    spell.EffectPointsPerComboPoint[0] = row.GetFloat(119);
                    spell.EffectPointsPerComboPoint[1] = row.GetFloat(120);
                    spell.EffectPointsPerComboPoint[2] = row.GetFloat(121);
                    spell.EffectSpellClassMaskA[0] = row.GetUInt32(122);
                    spell.EffectSpellClassMaskA[1] = row.GetUInt32(123);
                    spell.EffectSpellClassMaskA[2] = row.GetUInt32(124);
                    spell.EffectSpellClassMaskB[0] = row.GetUInt32(125);
                    spell.EffectSpellClassMaskB[1] = row.GetUInt32(126);
                    spell.EffectSpellClassMaskB[2] = row.GetUInt32(127);
                    spell.EffectSpellClassMaskC[0] = row.GetUInt32(128);
                    spell.EffectSpellClassMaskC[1] = row.GetUInt32(129);
                    spell.EffectSpellClassMaskC[2] = row.GetUInt32(130);
                    spell.SpellVisual[0] = row.GetUInt32(131);
                    spell.SpellVisual[1] = row.GetUInt32(132);
                    spell.SpellIconID = row.GetUInt32(133);
                    spell.ActiveIconID = row.GetUInt32(134);
                    spell.SpellPriority = row.GetUInt32(135);
                    spell.SpellName = GetNameFromUnknownLocale(row, 136, 16);    // Skip 152 for SpellNameFlag
                    spell.SpellSubText = GetNameFromUnknownLocale(row, 153, 16); // Skip 169 for RankFlags
                    // Skip 170 - 203 for unnecessary text stuff that we don't need
                    spell.ManaCostPercentage = row.GetUInt32(204);
                    spell.StartRecoveryCategory = row.GetUInt32(205);
                    spell.StartRecoveryTime = row.GetUInt32(206);
                    spell.MaxTargetLevel = row.GetUInt32(207);
                    spell.SpellClassSet = row.GetUInt32(208);
                    spell.SpellClassMask[0] = row.GetUInt32(209);
                    spell.SpellClassMask[1] = row.GetUInt32(210);
                    spell.SpellClassMask[2] = row.GetUInt32(211);
                    spell.MaxTargets = row.GetUInt32(212);
                    spell.DefenseType = row.GetUInt32(213);
                    spell.PreventionType = row.GetUInt32(214);
                    spell.StanceBarOrder = row.GetUInt32(215);
                    spell.EffectChainAmplitude[0] = row.GetFloat(216);
                    spell.EffectChainAmplitude[1] = row.GetFloat(217);
                    spell.EffectChainAmplitude[2] = row.GetFloat(218);
                    spell.MinFactionId = row.GetUInt32(219);
                    spell.MinReputation = row.GetUInt32(220);
                    spell.RequiredAuraVision = row.GetUInt32(221);
                    spell.TotemCategory[0] = row.GetUInt32(222);
                    spell.TotemCategory[1] = row.GetUInt32(223);
                    spell.RequiredAreaId = row.GetInt32(224);
                    spell.SchoolMask = row.GetUInt32(225);
                    spell.RuneCostID = row.GetUInt32(226);
                    spell.SpellMissileID = row.GetUInt32(227);
                    spell.PowerDisplayID = row.GetInt32(228);
                    spell.EffectBonusMultiplier[0] = row.GetFloat(229);
                    spell.EffectBonusMultiplier[1] = row.GetFloat(230);
                    spell.EffectBonusMultiplier[2] = row.GetFloat(231);
                    spell.SpellDescriptionVariableID = row.GetUInt32(232);
                    spell.SpellDifficultyID = row.GetUInt32(233);

                    if (i != 0)
                        ss << ", ";

                     ss << "(" << spell.Id << ", " << spell.SpellCategory << ", " << spell.DispelType << ", " << spell.Mechanic << ", " << spell.Attributes << ", " << spell.AttributesExA << ", " << spell.AttributesExB
                       << ", " << spell.AttributesExC << ", " << spell.AttributesExD << ", " << spell.AttributesExE << ", " << spell.AttributesExF << ", " << spell.AttributesExG << ", " << spell.Stances << ", " << spell.ExcludedStances
                       << ", " << spell.Targets << ", " << spell.TargetCreatureType << ", " << spell.SpellFocusObject << ", " << spell.FacingCasterFlags << ", " << spell.CasterAuraState << ", " << spell.TargetAuraState
                       << ", " << spell.CasterAuraStateNot << ", " << spell.TargetAuraStateNot << ", " << spell.CasterAuraSpell << ", " << spell.TargetAuraSpell << ", " << spell.ExcludeCasterAuraSpell << ", " << spell.ExcludeTargetAuraSpell
                       << ", " << spell.CastingTimeIndex << ", " << spell.RecoveryTime << ", " << spell.CategoryRecoveryTime << ", " << spell.InterruptFlags << ", " << spell.AuraInterruptFlags << ", " << spell.ChannelInterruptFlags
                       << ", " << spell.ProcFlags << ", " << spell.ProcChance << ", " << spell.ProcCharges << ", " << spell.MaxLevel << ", " << spell.BaseLevel << ", " << spell.SpellLevel << ", " << spell.DurationIndex << ", " << spell.PowerType
                       << ", " << spell.ManaCost << ", " << spell.ManaCostPerlevel << ", " << spell.ManaPerSecond << ", " << spell.ManaPerSecondPerLevel << ", " << spell.RangeIndex << ", " << spell.Speed << ", " << spell.ModalNextSpell
                       << ", " << spell.StackAmount << ", " << spell.Totem[0] << ", " << spell.Totem[1] << ", " << spell.Reagent[0] << ", " << spell.Reagent[1] << ", " << spell.Reagent[2] << ", " << spell.Reagent[3] << ", " << spell.Reagent[4]
                       << ", " << spell.Reagent[5] << ", " << spell.Reagent[6] << ", " << spell.Reagent[7] << ", " << spell.ReagentCount[0] << ", " << spell.ReagentCount[1] << ", " << spell.ReagentCount[2] << ", " << spell.ReagentCount[3]
                       << ", " << spell.ReagentCount[4] << ", " << spell.ReagentCount[5] << ", " << spell.ReagentCount[6] << ", " << spell.ReagentCount[7] << ", " << spell.EquippedItemClass << ", " << spell.EquippedItemSubClassMask
                       << ", " << spell.EquippedItemInventoryTypeMask << ", " << spell.Effect[0] << ", " << spell.Effect[1] << ", " << spell.Effect[2] << ", " << spell.EffectDieSides[0] << ", " << spell.EffectDieSides[1]
                       << ", " << spell.EffectDieSides[2] << ", " << spell.EffectRealPointsPerLevel[0] << ", " << spell.EffectRealPointsPerLevel[1] << ", " << spell.EffectRealPointsPerLevel[2] << ", " << spell.EffectBasePoints[0]
                       << ", " << spell.EffectBasePoints[1] << ", " << spell.EffectBasePoints[2] << ", " << spell.EffectMechanic[0] << ", " << spell.EffectMechanic[1] << ", " << spell.EffectMechanic[2] << ", " << spell.EffectImplicitTargetA[0]
                       << ", " << spell.EffectImplicitTargetA[1] << ", " << spell.EffectImplicitTargetA[2] << ", " << spell.EffectImplicitTargetB[0] << ", " << spell.EffectImplicitTargetB[1] << ", " << spell.EffectImplicitTargetB[2]
                       << ", " << spell.EffectRadiusIndex[0] << ", " << spell.EffectRadiusIndex[1] << ", " << spell.EffectRadiusIndex[2] << ", " << spell.EffectApplyAuraName[0] << ", " << spell.EffectApplyAuraName[1] << ", " << spell.EffectApplyAuraName[2]
                       << ", " << spell.EffectAuraPeriod[0] << ", " << spell.EffectAuraPeriod[1] << ", " << spell.EffectAuraPeriod[2] << ", " << spell.EffectAmplitude[0] << ", " << spell.EffectAmplitude[1] << ", " << spell.EffectAmplitude[2]
                       << ", " << spell.EffectChainTarget[0] << ", " << spell.EffectChainTarget[1] << ", " << spell.EffectChainTarget[2] << ", " << spell.EffectItemType[0] << ", " << spell.EffectItemType[1] << ", " << spell.EffectItemType[2]
                       << ", " << spell.EffectMiscValueA[0] << ", " << spell.EffectMiscValueA[1] << ", " << spell.EffectMiscValueA[2] << ", " << spell.EffectMiscValueB[0] << ", " << spell.EffectMiscValueB[1] << ", " << spell.EffectMiscValueB[2]
                       << ", " << spell.EffectTriggerSpell[0] << ", " << spell.EffectTriggerSpell[1] << ", " << spell.EffectTriggerSpell[2] << ", " << spell.EffectPointsPerComboPoint[0] << ", " << spell.EffectPointsPerComboPoint[1]
                       << ", " << spell.EffectPointsPerComboPoint[2] << ", " << spell.EffectSpellClassMaskA[0] << ", " << spell.EffectSpellClassMaskA[1] << ", " << spell.EffectSpellClassMaskA[2] << ", " << spell.EffectSpellClassMaskB[0]
                       << ", " << spell.EffectSpellClassMaskB[1] << ", " << spell.EffectSpellClassMaskB[2] << ", " << spell.EffectSpellClassMaskC[0] << ", " << spell.EffectSpellClassMaskC[1] << ", " << spell.EffectSpellClassMaskC[2]
                       << ", " << spell.SpellVisual[0] << ", " << spell.SpellVisual[1] << ", " << spell.SpellIconID << ", " << spell.ActiveIconID << ", " << spell.SpellPriority << ", '" << spell.SpellName << "', '" << spell.SpellSubText
                       << "', " << spell.ManaCostPercentage << ", " << spell.StartRecoveryCategory << ", " << spell.StartRecoveryTime << ", " << spell.MaxTargetLevel << ", " << spell.SpellClassSet << ", " << spell.SpellClassMask[0]
                       << ", " << spell.SpellClassMask[1] << ", " << spell.SpellClassMask[2] << ", " << spell.MaxTargets << ", " << spell.DefenseType << ", " << spell.PreventionType << ", " << spell.StanceBarOrder << ", " << spell.EffectChainAmplitude[0]
                       << ", " << spell.EffectChainAmplitude[1] << ", " << spell.EffectChainAmplitude[2] << ", " << spell.MinFactionId << ", " << spell.MinReputation << ", " << spell.RequiredAuraVision << ", " << spell.TotemCategory[0]
                       << ", " << spell.TotemCategory[1] << ", " << spell.RequiredAreaId << ", " << spell.SchoolMask << ", " << spell.RuneCostID << ", " << spell.SpellMissileID << ", " << spell.PowerDisplayID << ", " << spell.EffectBonusMultiplier[0]
                       << ", " << spell.EffectBonusMultiplier[1] << ", " << spell.EffectBonusMultiplier[2] << ", " << spell.SpellDescriptionVariableID << ", " << spell.SpellDifficultyID << ")";
                }

                ss << ";" << std::endl;
                sqlOutput += ss.str();
            }
        }
    }
    else
    {
        NC_LOG_ERROR("Failed to load Spell.dbc");
        return false;
    }
    return true;
}
} // namespace DBCLoader
