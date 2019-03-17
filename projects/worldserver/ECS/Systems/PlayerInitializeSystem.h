#include "ConnectionSystem.h"
#include <Networking/Opcode/Opcode.h>
#include <NovusTypes.h>
#include "../NovusEnums.h"

#include "../DatabaseCache/CharacterDatabaseCache.h"

#include "../Components/PlayerInitializeComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"

namespace PlayerInitializeSystem
{
    void Update(entt::registry &registry)
    {
        SingletonComponent& singleton = registry.get<SingletonComponent>(0);
        CharacterDatabaseCacheSingleton& characterDatabase = registry.get<CharacterDatabaseCacheSingleton>(0);
        NovusConnection& novusConnection = *singleton.connection;

        auto view = registry.view<PlayerInitializeComponent, PlayerUpdateDataComponent, PositionComponent>();
        view.each([&characterDatabase, &novusConnection](const auto, PlayerInitializeComponent& clientInitializeData, PlayerUpdateDataComponent& clientUpdateData, PositionComponent& clientPositionData)
        {
            NovusHeader packetHeader;
            packetHeader.command = NOVUS_FORWARDPACKET;
            packetHeader.account = clientInitializeData.accountGuid;

            /* Login Code Here */
            clientUpdateData.ResetFields();

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
            Common::ByteBuffer accountDataForwardPacket;
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

            packetHeader.size = (u16)accountDataTimes.GetActualSize();
            packetHeader.AddTo(accountDataForwardPacket);
            accountDataForwardPacket.Append(accountDataTimes);
            novusConnection.SendPacket(accountDataForwardPacket);


            /* SMSG_FEATURE_SYSTEM_STATUS */
            Common::ByteBuffer featureSystemStatus;
            packetHeader.opcode = Common::Opcode::SMSG_FEATURE_SYSTEM_STATUS;
            packetHeader.size = 1 + 1;
            packetHeader.AddTo(featureSystemStatus);

            featureSystemStatus.Write<u8>(2);
            featureSystemStatus.Write<u8>(0);
            novusConnection.SendPacket(featureSystemStatus);


            /* SMSG_MOTD */
            Common::ByteBuffer motdForwardPacket;
            Common::ByteBuffer motd;
            packetHeader.opcode = Common::Opcode::SMSG_MOTD;
            packetHeader.AddTo(motd);

            motd.Write<u32>(1);
            motd.WriteString("Welcome to NovusCore!");

            packetHeader.size = (u16)motd.GetActualSize();
            packetHeader.AddTo(motdForwardPacket);
            motdForwardPacket.Append(motd);
            novusConnection.SendPacket(motdForwardPacket);


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
            const CharacterData characterData = characterDatabase.cache->GetCharacterDataReadOnly(clientInitializeData.characterGuid);
            const CharacterVisualData characterVisualData = characterDatabase.cache->GetCharacterVisualDataReadOnly(clientInitializeData.characterGuid);

            clientUpdateData.SetGuidValue(OBJECT_FIELD_GUID, clientInitializeData.characterGuid);
            clientUpdateData.SetFieldValue<u32>(OBJECT_FIELD_TYPE, 0x19); // Object Type Player (Player, Unit, Object)
            clientUpdateData.SetFieldValue<f32>(OBJECT_FIELD_SCALE_X, 1.0f);

            clientUpdateData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, characterData.race, 0);
            clientUpdateData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, characterData.classId, 1);
            clientUpdateData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, characterData.gender, 2);
            clientUpdateData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, 1, 3);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_HEALTH, 60);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_POWER1, 0);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_POWER2, 0);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_POWER3, 100);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_POWER4, 100);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_POWER5, 0);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_POWER6, 8);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_POWER7, 1000);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_MAXHEALTH, 60);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER1, 0);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER2, 1000);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER3, 100);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER4, 100);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER5, 0);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER6, 8);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER7, 1000);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_LEVEL, characterData.level);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_FACTIONTEMPLATE, 1);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_FLAGS, 0x00000008);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_FLAGS_2, 0x00000800);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_BASEATTACKTIME + 0, 2900);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_BASEATTACKTIME + 1, 2000);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_RANGEDATTACKTIME, 0);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_BOUNDINGRADIUS, 0.208000f);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_COMBATREACH, 1.5f);

            u32 displayId = (47 + characterData.race * 2) + characterData.gender;
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_DISPLAYID, displayId);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_NATIVEDISPLAYID, displayId);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_MOUNTDISPLAYID, 0);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_MINDAMAGE, 9.007143f);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_MAXDAMAGE, 11.007143f);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_MINOFFHANDDAMAGE, 0);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_MAXOFFHANDDAMAGE, 0);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_BYTES_1, 0);
            clientUpdateData.SetFieldValue<f32>(UNIT_MOD_CAST_SPEED, 1);

            /* 3 individual for loops would make some for nice cache improvements :') */
            for (int i = 0; i < 5; i++)
            {
                clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_STAT0 + i, 20);
                clientUpdateData.SetFieldValue<i32>(UNIT_FIELD_POSSTAT0 + i, 0);
                clientUpdateData.SetFieldValue<i32>(UNIT_FIELD_NEGSTAT0 + i, 0);
            }

            for (int i = 0; i < 7; i++)
            {
                clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_RESISTANCES + i, 0);
                clientUpdateData.SetFieldValue<i32>(UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + i, 0);
                clientUpdateData.SetFieldValue<i32>(UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + i, 0);
            }
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_STAT0, 42);

            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_BASE_MANA, 0);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_BASE_HEALTH, 20);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_BYTES_2, 0);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_ATTACK_POWER, 29);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_ATTACK_POWER_MODS, 0);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_ATTACK_POWER_MULTIPLIER, 1.0f);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_RANGED_ATTACK_POWER, 0);
            clientUpdateData.SetFieldValue<u32>(UNIT_FIELD_RANGED_ATTACK_POWER_MODS, 0);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER, 1.0f);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_MINRANGEDDAMAGE, 0);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_MAXRANGEDDAMAGE, 0);
            clientUpdateData.SetFieldValue<f32>(UNIT_FIELD_HOVERHEIGHT, 1);

            clientUpdateData.SetFieldValue<u32>(PLAYER_FLAGS, 0);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.skin, 0);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.face, 1);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.hairStyle, 2);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES, characterVisualData.hairColor, 3);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES_2, characterVisualData.facialStyle, 0);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES_2, 0, 1);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES_2, 0, 2);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES_2, 3, 3);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES_3, characterData.gender, 0);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES_3, 0, 1);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES_3, 0, 2);
            clientUpdateData.SetFieldValue<u8>(PLAYER_BYTES_3, 0, 3);

            for (u8 slot = 0; slot < 19; ++slot)
            {
                clientUpdateData.SetGuidValue(PLAYER_FIELD_INV_SLOT_HEAD + (slot * 2), 0);

                clientUpdateData.SetFieldValue<u32>(PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), 0);
                clientUpdateData.SetFieldValue<u32>(PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0);
            }

            clientUpdateData.SetFieldValue<u32>(PLAYER_XP, 0);
            clientUpdateData.SetFieldValue<u32>(PLAYER_NEXT_LEVEL_XP, 400);

            for (int i = 0; i < 127; ++i)
            {
                {
                    clientUpdateData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)), 0);
                    clientUpdateData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 1, 0);
                    clientUpdateData.SetFieldValue<u32>((PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 2, 0);
                }
            }

            clientUpdateData.SetFieldValue<u32>(PLAYER_CHARACTER_POINTS1, 0);
            clientUpdateData.SetFieldValue<u32>(PLAYER_CHARACTER_POINTS2, 2);
            clientUpdateData.SetFieldValue<f32>(PLAYER_BLOCK_PERCENTAGE, 4.0f);
            clientUpdateData.SetFieldValue<f32>(PLAYER_DODGE_PERCENTAGE, 4.0f);
            clientUpdateData.SetFieldValue<f32>(PLAYER_PARRY_PERCENTAGE, 4.0f);
            clientUpdateData.SetFieldValue<f32>(PLAYER_CRIT_PERCENTAGE, 4.0f);
            clientUpdateData.SetFieldValue<f32>(PLAYER_RANGED_CRIT_PERCENTAGE, 4.0f);
            clientUpdateData.SetFieldValue<f32>(PLAYER_OFFHAND_CRIT_PERCENTAGE, 4.0f);

            for (int i = 0; i < 127; i++)
                clientUpdateData.SetFieldValue<u32>(PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);

            clientUpdateData.SetFieldValue<i32>(PLAYER_REST_STATE_EXPERIENCE, 0);
            clientUpdateData.SetFieldValue<u32>(PLAYER_FIELD_COINAGE, 5000000);

            for (int i = 0; i < 7; i++)
            {
                clientUpdateData.SetFieldValue<i32>(PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, 0);
                clientUpdateData.SetFieldValue<i32>(PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + i, 0);
                clientUpdateData.SetFieldValue<f32>(PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, 1.0f);
            }

            clientUpdateData.SetFieldValue<i32>(PLAYER_FIELD_WATCHED_FACTION_INDEX, -1);
            clientUpdateData.SetFieldValue<u32>(PLAYER_FIELD_MAX_LEVEL, 80);

            for (int i = 0; i < 3; i++)
            {
                clientUpdateData.SetFieldValue<f32>(PLAYER_RUNE_REGEN_1 + i, 0.1f);
            }

            for (int i = 0; i < 5; i++)
            {
                clientUpdateData.SetFieldValue<f32>(PLAYER_FIELD_GLYPH_SLOTS_1 + i, f32(21 + i));
            }
        });
    }
}