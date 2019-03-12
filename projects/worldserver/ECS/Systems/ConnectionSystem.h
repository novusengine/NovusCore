#pragma once
#include <entt.hpp>
#include <NovusTypes.h>

#include "../Connections/NovusConnection.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/PlayerConnectionData.h"

namespace ConnectionSystem
{
    using namespace std::chrono;
    static const steady_clock::time_point ApplicationStartTime = steady_clock::now();

    static void SetGuidValue(PlayerConnectionData& connectionData, u16 index, u64 value)
    {
        connectionData.playerFields.WriteAt<u64>(value, index * 4);
        connectionData.updateMask.SetBit(index);
        connectionData.updateMask.SetBit(index + 1);
    }
    template <typename T>
    static void SetFieldValue(PlayerConnectionData& connectionData, u16 index, T value, u8 offset = 0)
    {
        connectionData.playerFields.WriteAt<T>(value, (index * 4) + offset);
        connectionData.updateMask.SetBit(index);
    }

	void Update(f32 deltaTime, NovusConnection& novusConnection, entt::registry<u32> &registry) 
    {
		auto view = registry.view<ConnectionComponent, PlayerConnectionData>();

		view.each([&novusConnection](const auto, ConnectionComponent& connection, PlayerConnectionData& connectionData)
        {
            if (!connection.isInitialized)
            {
                NovusHeader packetHeader;
                packetHeader.command = NOVUS_FOWARDPACKET;
                packetHeader.account = connection.accountGuid;

                connectionData.playerFields.Clean();
                connectionData.playerFields.Resize(PLAYER_END * 4);
                connectionData.updateMask.Reset();

                /* SMSG_LOGIN_VERIFY_WORLD */
                Common::ByteBuffer verifyWorld;
                packetHeader.opcode = Common::Opcode::SMSG_LOGIN_VERIFY_WORLD;
                packetHeader.size = 4 + (4 * 4);
                packetHeader.AddTo(verifyWorld);

                verifyWorld.Write<u32>(0); // Map (0 == Eastern Kingdom) & Elwynn Forest (Zone is 12) & Northshire (Area is 9)
                verifyWorld.Write<f32>(-8949.950195f);
                verifyWorld.Write<f32>(-132.492996f);
                verifyWorld.Write<f32>(83.531197f);
                verifyWorld.Write<f32>(0.0f);
                novusConnection.SendPacket(verifyWorld);


                /* SMSG_ACCOUNT_DATA_TIMES */
                Common::ByteBuffer accountDataForwardPacket;
                Common::ByteBuffer accountDataTimes;
                packetHeader.opcode = Common::Opcode::SMSG_ACCOUNT_DATA_TIMES;

                u32 mask = 0xEA;
                accountDataTimes.Write<u32>((u32)time(nullptr));
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
                motd.WriteString("Welcome");

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

                /* SMSG_UPDATE_OBJECT */
                SetGuidValue(connectionData, OBJECT_FIELD_GUID, connection.characterGuid);
                SetFieldValue<u32>(connectionData, OBJECT_FIELD_TYPE, 0x19); // Object Type Player (Player, Unit, Object)
                SetFieldValue<f32>(connectionData, OBJECT_FIELD_SCALE_X, 1.0f); // Object Type Player (Player, Unit, Object)

                SetFieldValue<u8>(connectionData, UNIT_FIELD_BYTES_0, 4, 0);
                SetFieldValue<u8>(connectionData, UNIT_FIELD_BYTES_0, 1, 1);
                SetFieldValue<u8>(connectionData, UNIT_FIELD_BYTES_0, 1, 2);
                SetFieldValue<u8>(connectionData, UNIT_FIELD_BYTES_0, 1, 3);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_HEALTH, 60);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_POWER1, 0);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_POWER2, 0);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_POWER3, 100);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_POWER4, 100);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_POWER5, 0);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_POWER6, 8);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_POWER7, 1000);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_MAXHEALTH, 60);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_MAXPOWER1, 0);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_MAXPOWER2, 1000);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_MAXPOWER3, 100);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_MAXPOWER4, 100);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_MAXPOWER5, 0);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_MAXPOWER6, 8);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_MAXPOWER7, 1000);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_LEVEL, 80);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_FACTIONTEMPLATE, 1);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_FLAGS, 0x00000008);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_FLAGS_2, 0x00000800);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_BASEATTACKTIME + 0, 2900);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_BASEATTACKTIME + 1, 2000);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_RANGEDATTACKTIME, 0);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_BOUNDINGRADIUS, 0.208000f);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_COMBATREACH, 1.5f);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_DISPLAYID, 56);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_NATIVEDISPLAYID, 50);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_MOUNTDISPLAYID, 0);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_MINDAMAGE, 9.007143f);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_MAXDAMAGE, 11.007143f);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_MINOFFHANDDAMAGE, 0);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_MAXOFFHANDDAMAGE, 0);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_BYTES_1, 0);
                SetFieldValue<f32>(connectionData, UNIT_MOD_CAST_SPEED, 1);

                /* 3 individual for loops would make some for nice cache improvements :') */
                for (int i = 0; i < 5; i++)
                {
                    SetFieldValue<u32>(connectionData, UNIT_FIELD_STAT0 + i, 20);
                    SetFieldValue<i32>(connectionData, UNIT_FIELD_POSSTAT0 + i, 0);
                    SetFieldValue<i32>(connectionData, UNIT_FIELD_NEGSTAT0 + i, 0);
                }

                for (int i = 0; i < 7; i++)
                {
                    SetFieldValue<u32>(connectionData, UNIT_FIELD_RESISTANCES + i, 0);
                    SetFieldValue<i32>(connectionData, UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + i, 0);
                    SetFieldValue<i32>(connectionData, UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + i, 0);
                }
                SetFieldValue<u32>(connectionData, UNIT_FIELD_STAT0, 42);

                SetFieldValue<u32>(connectionData, UNIT_FIELD_BASE_MANA, 0);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_BASE_HEALTH, 20);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_BYTES_2, 0);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_ATTACK_POWER, 29);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_ATTACK_POWER_MODS, 0);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_ATTACK_POWER_MULTIPLIER, 1.0f);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_RANGED_ATTACK_POWER, 0);
                SetFieldValue<u32>(connectionData, UNIT_FIELD_RANGED_ATTACK_POWER_MODS, 0);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER, 1.0f);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_MINRANGEDDAMAGE, 0);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_MAXRANGEDDAMAGE, 0);
                SetFieldValue<f32>(connectionData, UNIT_FIELD_HOVERHEIGHT, 1);

                SetFieldValue<u32>(connectionData, PLAYER_FLAGS, 0);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES, 1, 0);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES, 1, 1);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES, 1, 2);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES, 1, 3);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES_2, 1, 0);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES_2, 0, 1);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES_2, 0, 2);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES_2, 3, 3);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES_3, 1, 0);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES_3, 0, 1);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES_3, 0, 2);
                SetFieldValue<u8>(connectionData, PLAYER_BYTES_3, 0, 3);

                for (u8 slot = 0; slot < 19; ++slot)
                {
                    SetGuidValue(connectionData, PLAYER_FIELD_INV_SLOT_HEAD + (slot * 2), 0);

                    SetFieldValue<u32>(connectionData, PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), 0);
                    SetFieldValue<u32>(connectionData, PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0);
                }

                SetFieldValue<u32>(connectionData, PLAYER_XP, 0);
                SetFieldValue<u32>(connectionData, PLAYER_NEXT_LEVEL_XP, 400);

                for (int i = 0; i < 127; ++i)
                {
                    SetFieldValue<u32>(connectionData, (PLAYER_SKILL_INFO_1_1 + ((i) * 3)), 0);
                    SetFieldValue<u32>(connectionData, (PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 1, 0);
                    SetFieldValue<u32>(connectionData, (PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 2, 0);
                }

                SetFieldValue<u32>(connectionData, PLAYER_CHARACTER_POINTS1, 0);
                SetFieldValue<u32>(connectionData, PLAYER_CHARACTER_POINTS2, 2);
                SetFieldValue<f32>(connectionData, PLAYER_BLOCK_PERCENTAGE, 4.0f);
                SetFieldValue<f32>(connectionData, PLAYER_DODGE_PERCENTAGE, 4.0f);
                SetFieldValue<f32>(connectionData, PLAYER_PARRY_PERCENTAGE, 4.0f);
                SetFieldValue<f32>(connectionData, PLAYER_CRIT_PERCENTAGE, 4.0f);
                SetFieldValue<f32>(connectionData, PLAYER_RANGED_CRIT_PERCENTAGE, 4.0f);
                SetFieldValue<f32>(connectionData, PLAYER_OFFHAND_CRIT_PERCENTAGE, 4.0f);

                for (int i = 0; i < 127; i++)
                    SetFieldValue<u32>(connectionData, PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);

                SetFieldValue<i32>(connectionData, PLAYER_REST_STATE_EXPERIENCE, 0);
                SetFieldValue<u32>(connectionData, PLAYER_FIELD_COINAGE, 5000000);

                for (int i = 0; i < 7; i++)
                {
                    SetFieldValue<i32>(connectionData, PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, 0);
                    SetFieldValue<i32>(connectionData, PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + i, 0);
                    SetFieldValue<f32>(connectionData, PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, 1.0f);
                }

                SetFieldValue<i32>(connectionData, PLAYER_FIELD_WATCHED_FACTION_INDEX, -1);
                SetFieldValue<u32>(connectionData, PLAYER_FIELD_MAX_LEVEL, 80);

                for (int i = 0; i < 3; i++)
                {
                    SetFieldValue<f32>(connectionData, PLAYER_RUNE_REGEN_1 + i, 0.1f);
                }

                for (int i = 0; i < 5; i++)
                {
                    SetFieldValue<f32>(connectionData, PLAYER_FIELD_GLYPH_SLOTS_1 + i, f32(21 + i));
                }

                u8  updateType = 3; // UPDATETYPE_CREATE_OBJECT2
                u16 updateFlags = 0x0000; // UPDATEFLAG_NONE

                // Correct & Checked Flags
                updateFlags |= 0x0001; // UPDATEFLAG_SELF
                updateFlags |= 0x0020; // UPDATEFLAG_LIVING
                updateFlags |= 0x0040; // UPDATEFLAG_STATIONARY_POSITION

                Common::ByteBuffer buffer(500);
                buffer.Write<u8>(updateType);
                buffer.AppendGuid(connection.characterGuid);
                buffer.Write<u8>(4); // TYPEID_PLAYER

                // BuildMovementUpdate(ByteBuffer* data, uint16 flags)
                buffer.Write<u16>(updateFlags);

                //if (updateFlags & 0x0020) If living
                {
                    u32 gameTimeMS = u32(duration_cast<milliseconds>(steady_clock::now() - ApplicationStartTime).count());
                    //std::cout << "Gametime MS: " << gameTimeMS << std::endl;

                    buffer.Write<u32>(0x00); // MovementFlags
                    buffer.Write<u16>(0x00); // Extra MovementFlags
                    buffer.Write<u32>(gameTimeMS); // Game Time
                    // TaggedPosition<Position::XYZO>(pos);
                    buffer.Write<f32>(-8949.950195f);
                    buffer.Write<f32>(-132.492996f);
                    buffer.Write<f32>(83.531197f);
                    buffer.Write<f32>(0.0f);

                    // FallTime
                    buffer.Write<u32>(0);

                    // Movement Speeds
                    buffer.Write<f32>(2.5f); // MOVE_WALK
                    buffer.Write<f32>(7.0f); // MOVE_RUN
                    buffer.Write<f32>(4.5f); // MOVE_RUN_BACK
                    buffer.Write<f32>(4.722222f); // MOVE_SWIM
                    buffer.Write<f32>(2.5f); // MOVE_SWIM_BACK
                    buffer.Write<f32>(7.0f); // MOVE_FLIGHT
                    buffer.Write<f32>(4.5f); // MOVE_FLIGHT_BACK
                    buffer.Write<f32>(3.141593f); // MOVE_TURN_RATE
                    buffer.Write<f32>(3.141593f); // MOVE_PITCH_RATE
                }

                Common::ByteBuffer fieldBuffer;
                fieldBuffer.Resize(5304);
                UpdateMask<1344> updateMask(PLAYER_END);

                u16 _fieldNotifyFlags = UF_FLAG_DYNAMIC;

                u32* flags = UnitUpdateFieldFlags;
                u32 visibleFlag = UF_FLAG_PUBLIC;
                visibleFlag |= UF_FLAG_PRIVATE;

                for (u16 index = 0; index < PLAYER_END; ++index)
                {
                    if (_fieldNotifyFlags & flags[index] || ((flags[index] & visibleFlag) & UF_FLAG_SPECIAL_INFO)
                        || ((updateType == 0 ? updateMask.IsSet(index) : connectionData.playerFields.ReadAt<i32>(index * 4))
                            && (flags[index] & visibleFlag)))
                    {
                        updateMask.SetBit(index);

                        if (index == UNIT_NPC_FLAGS)
                        {
                            u32 appendValue = connectionData.playerFields.ReadAt<u32>(UNIT_NPC_FLAGS * 4);

                            /*if (creature)
                                if (!target->CanSeeSpellClickOn(creature))
                                    appendValue &= ~UNIT_NPC_FLAG_SPELLCLICK;*/

                            fieldBuffer.Write<u32>(appendValue);
                        }
                        else if (index == UNIT_FIELD_AURASTATE)
                        {
                            // Check per caster aura states to not enable using a spell in client if specified aura is not by target
                            u32 auraState = connectionData.playerFields.ReadAt<u32>(UNIT_FIELD_AURASTATE * 4) &~(((1 << (14 - 1)) | (1 << (16 - 1))));

                            fieldBuffer.Write<u32>(auraState);
                        }
                        // Seems to be fixed already??
                        // FIXME: Some values at server stored in f32 format but must be sent to client in uint32 format
                        else if (index >= UNIT_FIELD_BASEATTACKTIME && index <= UNIT_FIELD_RANGEDATTACKTIME)
                        {
                            // convert from f32 to uint32 and send
                            fieldBuffer.Write<u32>(u32(connectionData.playerFields.ReadAt<i32>(index * 4)));
                        }
                        // there are some (said f32 in TC, but all these are ints)int values which may be negative or can't get negative due to other checks
                        else if ((index >= UNIT_FIELD_NEGSTAT0 && index <= UNIT_FIELD_NEGSTAT4) ||
                            (index >= UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 6)) ||
                            (index >= UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 6)) ||
                            (index >= UNIT_FIELD_POSSTAT0 && index <= UNIT_FIELD_POSSTAT4))
                        {
                            fieldBuffer.Write<u32>(u32(connectionData.playerFields.ReadAt<i32>(index * 4)));
                        }
                        // Gamemasters should be always able to select units - remove not selectable flag
                        else if (index == UNIT_FIELD_FLAGS)
                        {
                            u32 appendValue = connectionData.playerFields.ReadAt<u32>(UNIT_FIELD_FLAGS * 4);
                            //if (target->IsGameMaster())
                                //appendValue &= ~UNIT_FLAG_NOT_SELECTABLE;

                            fieldBuffer.Write<u32>(appendValue);
                        }
                        // use modelid_a if not gm, _h if gm for CREATURE_FLAG_EXTRA_TRIGGER creatures
                        else if (index == UNIT_FIELD_DISPLAYID)
                        {
                            u32 displayId = connectionData.playerFields.ReadAt<u32>(UNIT_FIELD_DISPLAYID * 4);
                            /*if (creature)
                            {
                                CreatureTemplate const* cinfo = creature->GetCreatureTemplate();
                                // this also applies for transform auras
                                if (SpellInfo const* transform = sSpellMgr->GetSpellInfo(GetTransformSpell()))
                                    for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                                        if (transform->Effects[i].IsAura(SPELL_AURA_TRANSFORM))
                                            if (CreatureTemplate const* transformInfo = sObjectMgr->GetCreatureTemplate(transform->Effects[i].MiscValue))
                                            {
                                                cinfo = transformInfo;
                                                break;
                                            }
                                if (cinfo->flags_extra & CREATURE_FLAG_EXTRA_TRIGGER)
                                    if (target->IsGameMaster())
                                        displayId = cinfo->GetFirstVisibleModel();
                            }*/

                            fieldBuffer.Write<u32>(displayId);
                        }
                        // hide lootable animation for unallowed players
                        else if (index == UNIT_DYNAMIC_FLAGS)
                        {
                            u32 dynamicFlags = connectionData.playerFields.ReadAt<u32>(UNIT_DYNAMIC_FLAGS * 4) & ~(0x4 | 0x08); // UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_TAPPED_BY_PLAYER

                            /*if (creature)
                            {
                                if (creature->hasLootRecipient())
                                {
                                    dynamicFlags |= UNIT_DYNFLAG_TAPPED;
                                    if (creature->isTappedBy(target))
                                        dynamicFlags |= UNIT_DYNFLAG_TAPPED_BY_PLAYER;
                                }
                                if (!target->isAllowedToLoot(creature))
                                    dynamicFlags &= ~UNIT_DYNFLAG_LOOTABLE;
                            }*/

                            // unit UNIT_DYNFLAG_TRACK_UNIT should only be sent to caster of SPELL_AURA_MOD_STALKED auras
                            //if (dynamicFlags & UNIT_DYNFLAG_TRACK_UNIT)
                                //if (!HasAuraTypeWithCaster(SPELL_AURA_MOD_STALKED, target->GetGUID()))
                                    //dynamicFlags &= ~UNIT_DYNFLAG_TRACK_UNIT;

                            fieldBuffer.Write<u32>(dynamicFlags);
                        }
                        // FG: pretend that OTHER players in own group are friendly ("blue")
                        else if (index == UNIT_FIELD_BYTES_2 || index == UNIT_FIELD_FACTIONTEMPLATE)
                        {
                            //if (IsControlledByPlayer() && target != this && sWorld->getBoolConfig(CONFIG_ALLOW_TWO_SIDE_INTERACTION_GROUP) && IsInRaidWith(target))
                            //{
                                //FactionTemplateEntry const* ft1 = GetFactionTemplateEntry();
                                //FactionTemplateEntry const* ft2 = target->GetFactionTemplateEntry();
                                //if (!ft1->IsFriendlyTo(*ft2))
                                //{
                                    //if (index == UNIT_FIELD_BYTES_2)
                                        // Allow targetting opposite faction in party when enabled in config
                                        //fieldBuffer << (m_uint32Values[UNIT_FIELD_BYTES_2] & ((UNIT_BYTE2_FLAG_SANCTUARY /*| UNIT_BYTE2_FLAG_AURAS | UNIT_BYTE2_FLAG_UNK5*/) << 8)); // this flag is at uint8 offset 1 !!
                                    //else
                                        // pretend that all other HOSTILE players have own faction, to allow follow, heal, rezz (trade wont work)
                                        //fieldBuffer << uint32(target->GetFaction());
                                //}
                                //else
                                    //fieldBuffer << m_uint32Values[index];
                            //}
                            //else
                            fieldBuffer.Write(connectionData.playerFields.GetDataPointer() + index * 4, 4);
                        }
                        else
                        {
                            // send in current format (f32 as f32, uint32 as uint32)
                            fieldBuffer.Write(connectionData.playerFields.GetDataPointer() + index * 4, 4);
                        }
                    }
                }

                buffer.Write<u8>(updateMask.GetBlocks());
                updateMask.AddTo(buffer);
                buffer.Append(fieldBuffer);

                UpdateData updateData;
                updateData.AddBlock(buffer);

                Common::ByteBuffer tempBuffer;
                updateData.Build(tempBuffer);

                Common::ByteBuffer objectUpdate;
                packetHeader.opcode = Common::Opcode::SMSG_COMPRESSED_UPDATE_OBJECT;
                packetHeader.size = tempBuffer.GetActualSize();
                packetHeader.AddTo(objectUpdate);

                objectUpdate.Append(tempBuffer);
                novusConnection.SendPacket(objectUpdate);

                connection.isInitialized = true;
            }

            
		});
	}
}
