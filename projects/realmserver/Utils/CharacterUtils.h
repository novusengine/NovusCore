#pragma once
#include <NovusTypes.h>
#include <vector>
#include <string>

#include "../DatabaseCache/CharacterDatabaseCache.h"

namespace CharacterUtils
{
struct SpawnPosition
{
    u16 mapId;
    u16 zoneId;
    f32 coordinate_x;
    f32 coordinate_y;
    f32 coordinate_z;
    f32 orientation;
};

bool BuildDefaultSkillSQL(const std::vector<DefaultSkillStorage> defaultSkills, u64 charGuid, u8 charRace, u8 charClass, std::string& output)
{
    int count = 0;
    u8 raceBitIndex = charRace - 1;
    u8 classBitIndex = charClass - 1;
    std::stringstream ss;

    for (DefaultSkillStorage skillData : defaultSkills)
    {
        if (skillData.raceMask == -1 || (skillData.raceMask & (1 << raceBitIndex)))
        {
            if (skillData.classMask == -1 || (skillData.classMask & (1 << classBitIndex)))
            {
                if (count++ > 0)
                    ss << ",";

                ss << "(" << charGuid << ", " << skillData.id << ", " << skillData.value << ", " << skillData.maxValue << ")";
            }
        }
    }

    if (count == 0)
        return false;

    ss << ";";

    output = "INSERT INTO character_skill_storage(guid, skill, value, character_skill_storage.maxValue) VALUES";
    output += ss.str();

    return true;
}
bool BuildDefaultSpellSQL(const std::vector<DefaultSpellStorage> defaultSpells, u64 charGuid, u8 charRace, u8 charClass, std::string& output)
{
    int count = 0;
    u8 raceBitIndex = charRace - 1;
    u8 classBitIndex = charClass - 1;
    std::stringstream ss;

    for (DefaultSpellStorage spellData : defaultSpells)
    {
        if (spellData.raceMask == -1 || (spellData.raceMask & (1 << raceBitIndex)))
        {
            if (spellData.classMask == -1 || (spellData.classMask & (1 << classBitIndex)))
            {
                if (count++ > 0)
                    ss << ",";

                ss << "(" << charGuid << ", " << spellData.id << ")";
            }
        }
    }

    if (count == 0)
        return false;

    ss << ";";

    output = "INSERT INTO character_spell_storage(guid, spell) VALUES";
    output += ss.str();

    return true;
}
bool BuildGetDefaultSpawn(const std::vector<DefaultSpawnStorage> defaultSpawns, u8 charRace, u8 charClass, SpawnPosition& output)
{
    u8 raceBitIndex = charRace - 1;
    u8 classBitIndex = charClass - 1;

    for (DefaultSpawnStorage spawnData : defaultSpawns)
    {
        if (spawnData.raceMask == -1 || (spawnData.raceMask & (1 << raceBitIndex)))
        {
            if (spawnData.classMask == -1 || (spawnData.classMask & (1 << classBitIndex)))
            {
                output.mapId = spawnData.mapId;
                output.zoneId = spawnData.zoneId;
                output.coordinate_x = spawnData.coordinate_x;
                output.coordinate_y = spawnData.coordinate_y;
                output.coordinate_z = spawnData.coordinate_z;
                output.orientation = spawnData.orientation;

                return true;
            }
        }
    }

    return false;
}
} // namespace CharacterUtils