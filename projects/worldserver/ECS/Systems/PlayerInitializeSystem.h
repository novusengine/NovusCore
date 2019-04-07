#include "PlayerConnectionSystem.h"
#include <Networking/Opcode/Opcode.h>
#include <NovusTypes.h>
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
    void Update(entt::registry &registry)
    {
        SingletonComponent& singleton = registry.ctx<SingletonComponent>();
        CharacterDatabaseCacheSingleton& characterDatabase = registry.ctx<CharacterDatabaseCacheSingleton>();
        NovusConnection& novusConnection = *singleton.connection;

        auto view = registry.view<PlayerInitializeComponent, PlayerFieldDataComponent, PlayerPositionComponent, PlayerSpellStorageComponent, PlayerSkillStorageComponent>();
        view.each([&characterDatabase, &novusConnection](const auto, PlayerInitializeComponent& clientInitializeData, PlayerFieldDataComponent& clientFieldData, PlayerPositionComponent& clientPositionData, PlayerSpellStorageComponent& spellStorageData, PlayerSkillStorageComponent& skillStorageData)
        {
            /* Load Cached Data for character */
            std::vector<CharacterSpellStorage> characterSpellStorage;
            if (characterDatabase.cache->GetCharacterSpellStorage(clientInitializeData.characterGuid, characterSpellStorage))
            {
                for (CharacterSpellStorage spell : characterSpellStorage)
                {
                    spellStorageData.spells.push_back(spell.id);
                }
            }

            std::vector<CharacterSkillStorage> characterSkillStorage;
            if (characterDatabase.cache->GetCharacterSkillStorage(clientInitializeData.characterGuid, characterSkillStorage))
            {
                for (CharacterSkillStorage skill : characterSkillStorage)
                {
                    skillData newSkill;
                    newSkill.id = skill.id;
                    newSkill.value = skill.value;
                    newSkill.maxValue = skill.maxValue;
                    skillStorageData.skills.push_back(newSkill);
                }
            }

            for (i32 i = 0; i < MAX_MOVEMENT_OPCODES; i++)
            {
                clientPositionData.lastMovementOpcodeTime[i] = 0;
            }
            
            NovusHeader packetHeader;
            packetHeader.command = NOVUS_FORWARDPACKET;
            packetHeader.account = clientInitializeData.accountGuid;

            /* Login Code Here */
            clientFieldData.ResetFields();

            /* SMSG_LOGIN_VERIFY_WORLD */
            Common::ByteBuffer verifyWorld;
            packetHeader.opcode = Common::Opcode::SMSG_LOGIN_VERIFY_WORLD;
            packetHeader.size = 4 + (4 * 4);
            packetHeader.AddTo(verifyWorld);

            verifyWorld.Write<u32>(clientPositionData.mapId); // Map (0 == Eastern Kingdom) & Elwynn Forest (Zone is 12) & Northshire (Area is 9)
            verifyWorld.Write<f32>(clientPositionData.x);
            verifyWorld.Write<f32>(clientPositionData.y);
            verifyWorld.Write<f32>(clientPositionData.z);
            verifyWorld.Write<f32>(clientPositionData.y);
            novusConnection.SendPacket(verifyWorld);

            /* SMSG_ACCOUNT_DATA_TIMES */
            Common::ByteBuffer accountDataTimes;
            packetHeader.opcode = Common::Opcode::SMSG_ACCOUNT_DATA_TIMES;

            u32 mask = 0xEA;
            accountDataTimes.Write<u32>((u32)time(nullptr)); // Unix Time
            accountDataTimes.Write<u8>(1); // bitmask blocks count
            accountDataTimes.Write<u32>(mask); // PER_CHARACTER_CACHE_MASK

            for (u32 i = 0; i < 8; ++i)
            {
                if (mask & (1 << i))
                    accountDataTimes.Write<u32>(0);
            }
            
            novusConnection.SendPacket(packetHeader.BuildHeaderPacket(accountDataTimes));


            /* SMSG_FEATURE_SYSTEM_STATUS */
            Common::ByteBuffer featureSystemStatus;
            packetHeader.opcode = Common::Opcode::SMSG_FEATURE_SYSTEM_STATUS;
            packetHeader.size = 1 + 1;
            packetHeader.AddTo(featureSystemStatus);

            featureSystemStatus.Write<u8>(2);
            featureSystemStatus.Write<u8>(0);
            novusConnection.SendPacket(featureSystemStatus);


            /* SMSG_MOTD */
            Common::ByteBuffer motd;
            packetHeader.opcode = Common::Opcode::SMSG_MOTD;
            packetHeader.AddTo(motd);

            motd.Write<u32>(0);
            motd.WriteString("Welcome to NovusCore.");

            novusConnection.SendPacket(packetHeader.BuildHeaderPacket(motd));


            /* SMSG_LEARNED_DANCE_MOVES */
            Common::ByteBuffer learnedDanceMoves;
            packetHeader.opcode = Common::Opcode::SMSG_LEARNED_DANCE_MOVES;
            packetHeader.size = 4 + 4;
            packetHeader.AddTo(learnedDanceMoves);

            learnedDanceMoves.Write<u32>(0);
            learnedDanceMoves.Write<u32>(0);
            novusConnection.SendPacket(learnedDanceMoves);


            /* SMSG_ACTION_BUTTONS */
            Common::ByteBuffer actionButtons;
            packetHeader.opcode = Common::Opcode::SMSG_ACTION_BUTTONS;
            packetHeader.size = 1 + (4 * 144);
            packetHeader.AddTo(actionButtons);

            actionButtons.Write<u8>(1);
            for (u8 button = 0; button < 144; ++button)
            {
                actionButtons.Write<u32>(0);
            }
            novusConnection.SendPacket(actionButtons);


            /* SMSG_INITIAL_SPELLS */
            if (spellStorageData.spells.size() > 0)
            {
                packetHeader.opcode = Common::Opcode::SMSG_INITIAL_SPELLS;
                Common::ByteBuffer initialSpells;
                initialSpells.Write<u8>(0);
                initialSpells.Write<u16>(u16(spellStorageData.spells.size()));

                for (u32 spell : spellStorageData.spells)
                {
                    initialSpells.Write<u32>(spell);
                    initialSpells.Write<u16>(0);
                }

                initialSpells.Write<u16>(0); // Cooldown History Size
                novusConnection.SendPacket(packetHeader.BuildHeaderPacket(initialSpells));
            }


            /* SMSG_ALL_ACHIEVEMENT_DATA */
            Common::ByteBuffer achivementData;
            packetHeader.opcode = Common::Opcode::SMSG_ALL_ACHIEVEMENT_DATA;
            packetHeader.size = 4 + 4;
            packetHeader.AddTo(achivementData);

            achivementData.Write<i32>(-1);
            achivementData.Write<i32>(-1);
            novusConnection.SendPacket(achivementData);


            /* SMSG_LOGIN_SETTIMESPEED */
            Common::ByteBuffer loginSetTimeSpeed;
            packetHeader.opcode = Common::Opcode::SMSG_LOGIN_SETTIMESPEED;
            packetHeader.size = 4 + 4 + 4;
            packetHeader.AddTo(loginSetTimeSpeed);

            tm lt;
            time_t const tmpServerTime = time(nullptr);
            localtime_s(&lt, &tmpServerTime);

            loginSetTimeSpeed.Write<u32>(((lt.tm_year - 100) << 24 | lt.tm_mon << 20 | (lt.tm_mday - 1) << 14 | lt.tm_wday << 11 | lt.tm_hour << 6 | lt.tm_min));
            loginSetTimeSpeed.Write<f32>(0.01666667f);
            loginSetTimeSpeed.Write<u32>(0);
            novusConnection.SendPacket(loginSetTimeSpeed);

            /* Set Initial Fields */
            CharacterData characterData;
            characterDatabase.cache->GetCharacterData(clientInitializeData.characterGuid, characterData);
            CharacterVisualData characterVisualData;
            characterDatabase.cache->GetCharacterVisualData(clientInitializeData.characterGuid, characterVisualData);

            clientFieldData.SetGuidValue(OBJECT_FIELD_GUID, characterData.guid);
            clientFieldData.SetFieldValue<u32>(OBJECT_FIELD_TYPE, 0x19); // Object Type Player (Player, Unit, Object)
            clientFieldData.SetFieldValue<f32>(OBJECT_FIELD_SCALE_X, 1.0f);

            clientFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, characterData.race, 0);
            clientFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, characterData.classId, 1);
            clientFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, characterData.gender, 2);
            clientFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, 1, 3);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_HEALTH, 60);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER1, 0);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER2, 0);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER3, 100);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER4, 100);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER5, 0);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER6, 8);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER7, 1000);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXHEALTH, 60);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER1, 0);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER2, 1000);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER3, 100);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER4, 100);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER5, 0);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER6, 8);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER7, 1000);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_LEVEL, characterData.level);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_FACTIONTEMPLATE, 14);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_FLAGS, 0x00000008);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_FLAGS_2, 0x00000800);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_BASEATTACKTIME + 0, 2900);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_BASEATTACKTIME + 1, 2000);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_RANGEDATTACKTIME, 0);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_BOUNDINGRADIUS, 0.208000f);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_COMBATREACH, 1.5f);

            u32 displayId = 0;
            CharacterUtils::GetDisplayIdFromRace(characterData, displayId);

            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_DISPLAYID, displayId);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_NATIVEDISPLAYID, displayId);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_MOUNTDISPLAYID, 0);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_MINDAMAGE, 9.007143f);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_MAXDAMAGE, 11.007143f);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_MINOFFHANDDAMAGE, 0);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_MAXOFFHANDDAMAGE, 0);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_BYTES_1, 0);
            clientFieldData.SetFieldValue<f32>(UNIT_MOD_CAST_SPEED, 1);

            for (i32 i = 0; i < 5; i++)
            {
                clientFieldData.SetFieldValue<u32>(UNIT_FIELD_STAT0 + i, 20);
                clientFieldData.SetFieldValue<i32>(UNIT_FIELD_POSSTAT0 + i, 0);
                clientFieldData.SetFieldValue<i32>(UNIT_FIELD_NEGSTAT0 + i, 0);
            }

            for (i32 i = 0; i < 7; i++)
            {
                clientFieldData.SetFieldValue<u32>(UNIT_FIELD_RESISTANCES + i, 0);
                clientFieldData.SetFieldValue<i32>(UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + i, 0);
                clientFieldData.SetFieldValue<i32>(UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + i, 0);
            }
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_STAT0, 42);

            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_BASE_MANA, 0);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_BASE_HEALTH, 20);
            clientFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0);
            clientFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0x05, 1);
            clientFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0, 2);
            clientFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0, 3);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_ATTACK_POWER, 29);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_ATTACK_POWER_MODS, 0);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_ATTACK_POWER_MULTIPLIER, 1.0f);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_RANGED_ATTACK_POWER, 0);
            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_RANGED_ATTACK_POWER_MODS, 0);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER, 1.0f);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_MINRANGEDDAMAGE, 0);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_MAXRANGEDDAMAGE, 0);
            clientFieldData.SetFieldValue<f32>(UNIT_FIELD_HOVERHEIGHT, 1);

            clientFieldData.SetFieldValue<u32>(PLAYER_FLAGS, 0);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.skin);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.face, 1);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.hairStyle, 2);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.hairColor, 3);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES_2, characterVisualData.facialStyle, 0);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES_2, 0, 1);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES_2, 0, 2);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES_2, 3, 3);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES_3, characterData.gender);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES_3, 0, 1);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES_3, 0, 2);
            clientFieldData.SetFieldValue<u8>(PLAYER_BYTES_3, 0, 3);

            for (u8 slot = 0; slot < 19; ++slot)
            {
                clientFieldData.SetGuidValue(PLAYER_FIELD_INV_SLOT_HEAD + (slot * 2), 0);

                clientFieldData.SetFieldValue<u32>(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), 0);
                clientFieldData.SetFieldValue<u32>(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0);
            }

            clientFieldData.SetFieldValue<u32>(PLAYER_XP, 0);
            clientFieldData.SetFieldValue<u32>(PLAYER_NEXT_LEVEL_XP, 400);

            i32 skillSize = i32(skillStorageData.skills.size());
            for (i32 i = 0; i < 127; ++i)
            {
                if (i < skillSize)
                {
                    skillData skill = skillStorageData.skills.at(i);
                    clientFieldData.SetFieldValue<u16>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)), skill.id);
                    clientFieldData.SetFieldValue<u16>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)), 0, 2);

                    clientFieldData.SetFieldValue<u16>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 1, skill.value);
                    clientFieldData.SetFieldValue<u16>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 1, skill.maxValue, 2);

                    clientFieldData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 2, 0);
                }
                else
                {
                    clientFieldData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)), 0);
                    clientFieldData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 1, 0);
                    clientFieldData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 2, 0);
                }
            }

            clientFieldData.SetFieldValue<u32>(PLAYER_CHARACTER_POINTS1, 0);
            clientFieldData.SetFieldValue<u32>(PLAYER_CHARACTER_POINTS2, 2);
            clientFieldData.SetFieldValue<f32>(PLAYER_BLOCK_PERCENTAGE, 4.0f);
            clientFieldData.SetFieldValue<f32>(PLAYER_DODGE_PERCENTAGE, 4.0f);
            clientFieldData.SetFieldValue<f32>(PLAYER_PARRY_PERCENTAGE, 4.0f);
            clientFieldData.SetFieldValue<f32>(PLAYER_CRIT_PERCENTAGE, 4.0f);
            clientFieldData.SetFieldValue<f32>(PLAYER_RANGED_CRIT_PERCENTAGE, 4.0f);
            clientFieldData.SetFieldValue<f32>(PLAYER_OFFHAND_CRIT_PERCENTAGE, 4.0f);

            for (i32 i = 0; i < 127; i++)
                clientFieldData.SetFieldValue<u32>(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);

            clientFieldData.SetFieldValue<i32>(PLAYER_REST_STATE_EXPERIENCE, 0);
            clientFieldData.SetFieldValue<u32>(PLAYER_FIELD_COINAGE, 5000000);

            for (i32 i = 0; i < 7; i++)
            {
                clientFieldData.SetFieldValue<i32>(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, 0);
                clientFieldData.SetFieldValue<i32>(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + i, 0);
                clientFieldData.SetFieldValue<f32>(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, 1.0f);
            }

            clientFieldData.SetFieldValue<i32>(PLAYER_FIELD_WATCHED_FACTION_INDEX, -1);
            clientFieldData.SetFieldValue<u32>(PLAYER_FIELD_MAX_LEVEL, 80);

            for (i32 i = 0; i < 3; i++)
            {
                clientFieldData.SetFieldValue<f32>(PLAYER_RUNE_REGEN_1 + i, 0.1f);
            }

            for (i32 i = 0; i < 5; i++)
            {
                clientFieldData.SetFieldValue<f32>(PLAYER_FIELD_GLYPH_SLOTS_1 + i, f32(21 + i));
            }
        });
    }
}