#include "PlayerConnectionSystem.h"
#include <NovusTypes.h>
#include <Networking/Opcode/Opcode.h>
#include "../NovusEnums.h"
#include "../Utils/CharacterUtils.h"

#include "../DatabaseCache/CharacterDatabaseCache.h"

#include "../Components/PlayerFieldDataComponent.h"
#include "../Components/PlayerSpellStorageComponent.h"
#include "../Components/PlayerSkillStorageComponent.h"
#include "../Components/PlayerInitializeComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"

namespace PlayerInitializeSystem
{
void Update(entt::registry& registry)
{
    SingletonComponent& singleton = registry.ctx<SingletonComponent>();
    CharacterDatabaseCacheSingleton& characterDatabase = registry.ctx<CharacterDatabaseCacheSingleton>();

    auto view = registry.view<PlayerInitializeComponent, PlayerFieldDataComponent, PlayerPositionComponent, PlayerSpellStorageComponent, PlayerSkillStorageComponent>();
    view.each([&characterDatabase](const auto, PlayerInitializeComponent& playerInitializeData, PlayerFieldDataComponent& playerFieldData, PlayerPositionComponent& playerFPositionData, PlayerSpellStorageComponent& spellStorageData, PlayerSkillStorageComponent& skillStorageData) {
        /* Load Cached Data for character */
        robin_hood::unordered_map<u32, CharacterSpellStorage> characterSpellStorage;
        if (characterDatabase.cache->GetCharacterSpellStorage(playerInitializeData.characterGuid, characterSpellStorage))
        {
            for (auto itr : characterSpellStorage)
            {
                spellStorageData.spells.push_back(itr.first);
            }
        }

        robin_hood::unordered_map<u32, CharacterSkillStorage> characterSkillStorage;
        if (characterDatabase.cache->GetCharacterSkillStorage(playerInitializeData.characterGuid, characterSkillStorage))
        {
            for (auto itr : characterSkillStorage)
            {
                CharacterSkillStorage skill = itr.second;

                skillData newSkill;
                newSkill.id = skill.id;
                newSkill.value = skill.value;
                newSkill.maxValue = skill.maxValue;
                skillStorageData.skills.push_back(newSkill);
            }
        }

        for (i32 i = 0; i < MAX_MOVEMENT_OPCODES; i++)
        {
            playerFPositionData.lastMovementOpcodeTime[i] = 0;
        }

        /* Login Code Here */
        playerFieldData.ResetFields();

        /* SMSG_NEW_WORLD */
        /* SMSG_LOGIN_VERIFY_WORLD */
        Common::ByteBuffer verifyWorld;
        verifyWorld.Write<u32>(playerFPositionData.mapId);
        verifyWorld.Write<f32>(playerFPositionData.x);
        verifyWorld.Write<f32>(playerFPositionData.y);
        verifyWorld.Write<f32>(playerFPositionData.z);
        verifyWorld.Write<f32>(playerFPositionData.orientation);
        playerInitializeData.socket->SendPacket(verifyWorld, Common::Opcode::SMSG_LOGIN_VERIFY_WORLD);

        /* SMSG_ACCOUNT_DATA_TIMES */
        u32 mask = 0xEA;

        Common::ByteBuffer accountDataTimes;
        accountDataTimes.Write<u32>(static_cast<u32>(time(nullptr))); // Unix Time
        accountDataTimes.Write<u8>(1);                                // bitmask blocks count
        accountDataTimes.Write<u32>(mask);                            // PER_CHARACTER_CACHE_MASK

        for (u32 i = 0; i < 8; ++i)
        {
            if (mask & (1 << i))
                accountDataTimes.Write<u32>(0);
        }

        playerInitializeData.socket->SendPacket(accountDataTimes, Common::Opcode::SMSG_ACCOUNT_DATA_TIMES);

        /* SMSG_FEATURE_SYSTEM_STATUS */
        Common::ByteBuffer featureSystemStatus;
        featureSystemStatus.Write<u8>(2);
        featureSystemStatus.Write<u8>(0);
        playerInitializeData.socket->SendPacket(featureSystemStatus, Common::Opcode::SMSG_FEATURE_SYSTEM_STATUS);

        /* SMSG_MOTD */
        Common::ByteBuffer motd;
        motd.Write<u32>(0);
        motd.WriteString("Welcome to NovusCore.");
        playerInitializeData.socket->SendPacket(motd, Common::Opcode::SMSG_MOTD);

        /* SMSG_LEARNED_DANCE_MOVES */
        Common::ByteBuffer learnedDanceMoves;
        learnedDanceMoves.Write<u32>(0);
        learnedDanceMoves.Write<u32>(0);
        playerInitializeData.socket->SendPacket(learnedDanceMoves, Common::Opcode::SMSG_LEARNED_DANCE_MOVES);

        /* SMSG_ACTION_BUTTONS */
        Common::ByteBuffer actionButtons;
        actionButtons.Write<u8>(1);
        for (u8 button = 0; button < 144; ++button)
        {
            actionButtons.Write<u32>(0);
        }
        playerInitializeData.socket->SendPacket(actionButtons, Common::Opcode::SMSG_ACTION_BUTTONS);

        /* SMSG_INITIAL_SPELLS */
        if (spellStorageData.spells.size() > 0)
        {
            Common::ByteBuffer initialSpells;
            initialSpells.Write<u8>(0);
            initialSpells.Write<u16>(static_cast<u16>(spellStorageData.spells.size()));

            for (u32 spell : spellStorageData.spells)
            {
                initialSpells.Write<u32>(spell);
                initialSpells.Write<u16>(0);
            }

            initialSpells.Write<u16>(0); // Cooldown History Size
            playerInitializeData.socket->SendPacket(initialSpells, Common::Opcode::SMSG_INITIAL_SPELLS);
        }

        /* SMSG_ALL_ACHIEVEMENT_DATA */
        Common::ByteBuffer achievementData;
        achievementData.Write<i32>(-1);
        achievementData.Write<i32>(-1);
        playerInitializeData.socket->SendPacket(achievementData, Common::Opcode::SMSG_ALL_ACHIEVEMENT_DATA);

        /* SMSG_LOGIN_SETTIMESPEED */
        tm lt;
        time_t const tmpServerTime = time(nullptr);
        localtime_s(&lt, &tmpServerTime);

        Common::ByteBuffer loginSetTimeSpeed;
        loginSetTimeSpeed.Write<u32>(((lt.tm_year - 100) << 24 | lt.tm_mon << 20 | (lt.tm_mday - 1) << 14 | lt.tm_wday << 11 | lt.tm_hour << 6 | lt.tm_min));
        loginSetTimeSpeed.Write<f32>(0.01666667f);
        loginSetTimeSpeed.Write<u32>(0);
        playerInitializeData.socket->SendPacket(loginSetTimeSpeed, Common::Opcode::SMSG_LOGIN_SETTIMESPEED);

        /* Set Initial Fields */
        CharacterData characterData;
        characterDatabase.cache->GetCharacterData(playerInitializeData.characterGuid, characterData);
        CharacterVisualData characterVisualData;
        characterDatabase.cache->GetCharacterVisualData(playerInitializeData.characterGuid, characterVisualData);

        playerFieldData.SetGuidValue(OBJECT_FIELD_GUID, characterData.guid);
        playerFieldData.SetFieldValue<u32>(OBJECT_FIELD_TYPE, 0x19); // Object Type Player (Player, Unit, Object)
        playerFieldData.SetFieldValue<f32>(OBJECT_FIELD_SCALE_X, 1.0f);

        playerFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, characterData.race, 0);
        playerFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, characterData.classId, 1);
        playerFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, characterData.gender, 2);
        playerFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, 1, 3);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_HEALTH, 60);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER1, 0);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER2, 0);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER3, 100);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER4, 100);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER5, 0);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER6, 8);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER7, 1000);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXHEALTH, 60);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER1, 0);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER2, 1000);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER3, 100);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER4, 100);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER5, 0);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER6, 8);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER7, 1000);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_LEVEL, characterData.level);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_FACTIONTEMPLATE, 14);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_FLAGS, 0x00000008);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_FLAGS_2, 0x00000800);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_BASEATTACKTIME + 0, 2900);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_BASEATTACKTIME + 1, 2000);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_RANGEDATTACKTIME, 0);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_BOUNDINGRADIUS, 0.208000f);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_COMBATREACH, 1.5f);

        u32 displayId = 0;
        CharacterUtils::GetDisplayIdFromRace(characterData, displayId);

        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_DISPLAYID, displayId);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_NATIVEDISPLAYID, displayId);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_MOUNTDISPLAYID, 0);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_MINDAMAGE, 9.007143f);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_MAXDAMAGE, 11.007143f);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_MINOFFHANDDAMAGE, 0);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_MAXOFFHANDDAMAGE, 0);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_BYTES_1, 0);
        playerFieldData.SetFieldValue<f32>(UNIT_MOD_CAST_SPEED, 1);

        for (i32 i = 0; i < 5; i++)
        {
            playerFieldData.SetFieldValue<u32>(UNIT_FIELD_STAT0 + i, 20);
            playerFieldData.SetFieldValue<i32>(UNIT_FIELD_POSSTAT0 + i, 0);
            playerFieldData.SetFieldValue<i32>(UNIT_FIELD_NEGSTAT0 + i, 0);
        }

        for (i32 i = 0; i < 7; i++)
        {
            playerFieldData.SetFieldValue<u32>(UNIT_FIELD_RESISTANCES + i, 0);
            playerFieldData.SetFieldValue<i32>(UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + i, 0);
            playerFieldData.SetFieldValue<i32>(UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + i, 0);
        }
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_STAT0, 42);

        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_BASE_MANA, 0);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_BASE_HEALTH, 20);
        playerFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0);
        playerFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0x05, 1);
        playerFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0, 2);
        playerFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0, 3);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_ATTACK_POWER, 29);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_ATTACK_POWER_MODS, 0);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_ATTACK_POWER_MULTIPLIER, 1.0f);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_RANGED_ATTACK_POWER, 0);
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_RANGED_ATTACK_POWER_MODS, 0);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER, 1.0f);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_MINRANGEDDAMAGE, 0);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_MAXRANGEDDAMAGE, 0);
        playerFieldData.SetFieldValue<f32>(UNIT_FIELD_HOVERHEIGHT, 1);

        playerFieldData.SetFieldValue<u32>(PLAYER_FLAGS, 0);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.skin);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.face, 1);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.hairStyle, 2);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.hairColor, 3);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES_2, characterVisualData.facialStyle, 0);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES_2, 0, 1);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES_2, 0, 2);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES_2, 3, 3);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES_3, characterData.gender);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES_3, 0, 1);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES_3, 0, 2);
        playerFieldData.SetFieldValue<u8>(PLAYER_BYTES_3, 0, 3);

        for (u8 slot = 0; slot < 19; ++slot)
        {
            playerFieldData.SetGuidValue(PLAYER_FIELD_INV_SLOT_HEAD + (slot * 2), 0);

            playerFieldData.SetFieldValue<u32>(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), 0);
            playerFieldData.SetFieldValue<u32>(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0);
        }

        playerFieldData.SetFieldValue<u32>(PLAYER_XP, 0);
        playerFieldData.SetFieldValue<u32>(PLAYER_NEXT_LEVEL_XP, 400);

        i32 skillSize = i32(skillStorageData.skills.size());
        for (i32 i = 0; i < 127; ++i)
        {
            if (i < skillSize)
            {
                skillData skill = skillStorageData.skills.at(i);
                playerFieldData.SetFieldValue<u16>((PLAYER_SKILL_INFO_1_1 + ((i)*3)), skill.id);
                playerFieldData.SetFieldValue<u16>((PLAYER_SKILL_INFO_1_1 + ((i)*3)), 0, 2);

                playerFieldData.SetFieldValue<u16>((PLAYER_SKILL_INFO_1_1 + ((i)*3)) + 1, skill.value);
                playerFieldData.SetFieldValue<u16>((PLAYER_SKILL_INFO_1_1 + ((i)*3)) + 1, skill.maxValue, 2);

                playerFieldData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i)*3)) + 2, 0);
            }
            else
            {
                playerFieldData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i)*3)), 0);
                playerFieldData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i)*3)) + 1, 0);
                playerFieldData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i)*3)) + 2, 0);
            }
        }

        playerFieldData.SetFieldValue<u32>(PLAYER_CHARACTER_POINTS1, 0);
        playerFieldData.SetFieldValue<u32>(PLAYER_CHARACTER_POINTS2, 2);
        playerFieldData.SetFieldValue<f32>(PLAYER_BLOCK_PERCENTAGE, 4.0f);
        playerFieldData.SetFieldValue<f32>(PLAYER_DODGE_PERCENTAGE, 4.0f);
        playerFieldData.SetFieldValue<f32>(PLAYER_PARRY_PERCENTAGE, 4.0f);
        playerFieldData.SetFieldValue<f32>(PLAYER_CRIT_PERCENTAGE, 4.0f);
        playerFieldData.SetFieldValue<f32>(PLAYER_RANGED_CRIT_PERCENTAGE, 4.0f);
        playerFieldData.SetFieldValue<f32>(PLAYER_OFFHAND_CRIT_PERCENTAGE, 4.0f);

        for (i32 i = 0; i < 127; i++)
            playerFieldData.SetFieldValue<u32>(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);

        playerFieldData.SetFieldValue<i32>(PLAYER_REST_STATE_EXPERIENCE, 0);
        playerFieldData.SetFieldValue<u32>(PLAYER_FIELD_COINAGE, 5000000);

        for (i32 i = 0; i < 7; i++)
        {
            playerFieldData.SetFieldValue<i32>(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, 0);
            playerFieldData.SetFieldValue<i32>(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + i, 0);
            playerFieldData.SetFieldValue<f32>(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, 1.0f);
        }

        playerFieldData.SetFieldValue<i32>(PLAYER_FIELD_WATCHED_FACTION_INDEX, -1);
        playerFieldData.SetFieldValue<u32>(PLAYER_FIELD_MAX_LEVEL, 80);

        for (i32 i = 0; i < 3; i++)
        {
            playerFieldData.SetFieldValue<f32>(PLAYER_RUNE_REGEN_1 + i, 0.1f);
        }

        for (i32 i = 0; i < 5; i++)
        {
            playerFieldData.SetFieldValue<f32>(PLAYER_FIELD_GLYPH_SLOTS_1 + i, f32(21 + i));
        }
    });
}
} // namespace PlayerInitializeSystem