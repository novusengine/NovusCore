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

enum PowerType
{
    POWER_HEALTH = -2, // This is strictly used for spells
    POWER_MANA = 0,
    POWER_RAGE,
    POWER_FOCUS,
    POWER_ENERGY,
    POWER_HAPPINESS,
    POWER_RUNE,
    POWER_RUNIC_POWER,
    POWER_COUNT = 7
};

enum StatType
{
    STAT_STRENGTH,
    STAT_AGILITY,
    STAT_STAMINA,
    STAT_INTELLECT,
    STAT_SPIRIT,
    STAT_COUNT
};

enum ResistanceType
{
    RESISTANCE_NORMAL,
    RESISTANCE_HOLY,
    RESISTANCE_FIRE,
    RESISTANCE_NATURE,
    RESISTANCE_FROST,
    RESISTANCE_SHADOW,
    RESISTANCE_ARCANE,
    RESISTANCE_COUNT
};

struct UnitStatsComponent
{
    f32 baseHealth;
    f32 currentHealth;
    f32 maxHealth;
    bool healthIsDirty;

    f32 basePower[POWER_COUNT];
    f32 currentPower[POWER_COUNT];
    f32 maxPower[POWER_COUNT];
    bool powerIsDirty[POWER_COUNT];

    i32 baseStat[STAT_COUNT];
    i32 currentStat[STAT_COUNT];

    i32 baseResistance[RESISTANCE_COUNT];
    i32 currentResistance[RESISTANCE_COUNT];
};