#include "ConnectionSystem.h"
#include <Networking/Opcode/Opcode.h>

namespace ConnectionSystem
{
    using namespace std::chrono;
    const steady_clock::time_point ApplicationStartTime = steady_clock::now();
    void SetGuidValue(PlayerUpdateDataComponent& updateData, u16 index, u64 value)
    {
        updateData.playerFields.WriteAt<u64>(value, index * 4);
        updateData.updateMask.SetBit(index);
        updateData.updateMask.SetBit(index + 1);
    }

    void Update(f32 deltaTime, NovusConnection& novusConnection, entt::registry &registry)
    {
        auto view = registry.view<ConnectionComponent, PlayerUpdateDataComponent, PositionComponent>();
        auto subView = registry.view<ConnectionComponent>();

        view.each([&novusConnection, subView](const auto, ConnectionComponent& connection, PlayerUpdateDataComponent& updateData, PositionComponent& positionData)
        {
            NovusHeader packetHeader;
            packetHeader.command = NOVUS_FORWARDPACKET;
            packetHeader.account = connection.accountGuid;

            for (OpcodePacket& packet : connection.packets)
            {
                Common::Opcode opcode = Common::Opcode(packet.opcode);
                if (!connection.isInitialized)
                {
                    if (opcode == Common::Opcode::CMSG_PLAYER_LOGIN)
                    {
                        /* Login Code Here */
                        updateData.playerFields.Clean();
                        updateData.playerFields.Resize(PLAYER_END * 4);
                        updateData.updateMask.Reset();

                        /* SMSG_LOGIN_VERIFY_WORLD */
                        Common::ByteBuffer verifyWorld;
                        packetHeader.opcode = Common::Opcode::SMSG_LOGIN_VERIFY_WORLD;
                        packetHeader.size = 4 + (4 * 4);
                        packetHeader.AddTo(verifyWorld);

                        verifyWorld.Write<u32>(positionData.mapId); // Map (0 == Eastern Kingdom) & Elwynn Forest (Zone is 12) & Northshire (Area is 9)
                        verifyWorld.Write<f32>(positionData.x);
                        verifyWorld.Write<f32>(positionData.y);
                        verifyWorld.Write<f32>(positionData.z);
                        verifyWorld.Write<f32>(positionData.y);
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

                        /* Set Initial Fields */
                        SetGuidValue(updateData, OBJECT_FIELD_GUID, connection.characterGuid);
                        SetFieldValue<u32>(updateData, OBJECT_FIELD_TYPE, 0x19); // Object Type Player (Player, Unit, Object)
                        SetFieldValue<f32>(updateData, OBJECT_FIELD_SCALE_X, 1.0f); // Object Type Player (Player, Unit, Object)

                        SetFieldValue<u8>(updateData, UNIT_FIELD_BYTES_0, 4, 0);
                        SetFieldValue<u8>(updateData, UNIT_FIELD_BYTES_0, 1, 1);
                        SetFieldValue<u8>(updateData, UNIT_FIELD_BYTES_0, 1, 2);
                        SetFieldValue<u8>(updateData, UNIT_FIELD_BYTES_0, 1, 3);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_HEALTH, 60);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_POWER1, 0);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_POWER2, 0);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_POWER3, 100);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_POWER4, 100);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_POWER5, 0);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_POWER6, 8);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_POWER7, 1000);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_MAXHEALTH, 60);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_MAXPOWER1, 0);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_MAXPOWER2, 1000);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_MAXPOWER3, 100);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_MAXPOWER4, 100);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_MAXPOWER5, 0);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_MAXPOWER6, 8);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_MAXPOWER7, 1000);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_LEVEL, 80);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_FACTIONTEMPLATE, 1);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_FLAGS, 0x00000008);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_FLAGS_2, 0x00000800);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_BASEATTACKTIME + 0, 2900);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_BASEATTACKTIME + 1, 2000);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_RANGEDATTACKTIME, 0);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_BOUNDINGRADIUS, 0.208000f);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_COMBATREACH, 1.5f);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_DISPLAYID, 56);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_NATIVEDISPLAYID, 50);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_MOUNTDISPLAYID, 0);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_MINDAMAGE, 9.007143f);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_MAXDAMAGE, 11.007143f);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_MINOFFHANDDAMAGE, 0);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_MAXOFFHANDDAMAGE, 0);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_BYTES_1, 0);
                        SetFieldValue<f32>(updateData, UNIT_MOD_CAST_SPEED, 1);

                        /* 3 individual for loops would make some for nice cache improvements :') */
                        for (int i = 0; i < 5; i++)
                        {
                            SetFieldValue<u32>(updateData, UNIT_FIELD_STAT0 + i, 20);
                            SetFieldValue<i32>(updateData, UNIT_FIELD_POSSTAT0 + i, 0);
                            SetFieldValue<i32>(updateData, UNIT_FIELD_NEGSTAT0 + i, 0);
                        }

                        for (int i = 0; i < 7; i++)
                        {
                            SetFieldValue<u32>(updateData, UNIT_FIELD_RESISTANCES + i, 0);
                            SetFieldValue<i32>(updateData, UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + i, 0);
                            SetFieldValue<i32>(updateData, UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + i, 0);
                        }
                        SetFieldValue<u32>(updateData, UNIT_FIELD_STAT0, 42);

                        SetFieldValue<u32>(updateData, UNIT_FIELD_BASE_MANA, 0);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_BASE_HEALTH, 20);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_BYTES_2, 0);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_ATTACK_POWER, 29);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_ATTACK_POWER_MODS, 0);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_ATTACK_POWER_MULTIPLIER, 1.0f);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_RANGED_ATTACK_POWER, 0);
                        SetFieldValue<u32>(updateData, UNIT_FIELD_RANGED_ATTACK_POWER_MODS, 0);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER, 1.0f);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_MINRANGEDDAMAGE, 0);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_MAXRANGEDDAMAGE, 0);
                        SetFieldValue<f32>(updateData, UNIT_FIELD_HOVERHEIGHT, 1);

                        SetFieldValue<u32>(updateData, PLAYER_FLAGS, 0);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES, 1, 0);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES, 1, 1);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES, 1, 2);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES, 1, 3);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES_2, 1, 0);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES_2, 0, 1);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES_2, 0, 2);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES_2, 3, 3);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES_3, 1, 0);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES_3, 0, 1);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES_3, 0, 2);
                        SetFieldValue<u8>(updateData, PLAYER_BYTES_3, 0, 3);

                        for (u8 slot = 0; slot < 19; ++slot)
                        {
                            SetGuidValue(updateData, PLAYER_FIELD_INV_SLOT_HEAD + (slot * 2), 0);

                            SetFieldValue<u32>(updateData, PLAYER_VISIBLE_ITEM_1_ENTRYID + (slot * 2), 0);
                            SetFieldValue<u32>(updateData, PLAYER_VISIBLE_ITEM_1_ENCHANTMENT + (slot * 2), 0);
                        }

                        SetFieldValue<u32>(updateData, PLAYER_XP, 0);
                        SetFieldValue<u32>(updateData, PLAYER_NEXT_LEVEL_XP, 400);

                        for (int i = 0; i < 127; ++i)
                        {
                            SetFieldValue<u32>(updateData, (PLAYER_SKILL_INFO_1_1 + ((i) * 3)), 0);
                            SetFieldValue<u32>(updateData, (PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 1, 0);
                            SetFieldValue<u32>(updateData, (PLAYER_SKILL_INFO_1_1 + ((i) * 3)) + 2, 0);
                        }

                        SetFieldValue<u32>(updateData, PLAYER_CHARACTER_POINTS1, 0);
                        SetFieldValue<u32>(updateData, PLAYER_CHARACTER_POINTS2, 2);
                        SetFieldValue<f32>(updateData, PLAYER_BLOCK_PERCENTAGE, 4.0f);
                        SetFieldValue<f32>(updateData, PLAYER_DODGE_PERCENTAGE, 4.0f);
                        SetFieldValue<f32>(updateData, PLAYER_PARRY_PERCENTAGE, 4.0f);
                        SetFieldValue<f32>(updateData, PLAYER_CRIT_PERCENTAGE, 4.0f);
                        SetFieldValue<f32>(updateData, PLAYER_RANGED_CRIT_PERCENTAGE, 4.0f);
                        SetFieldValue<f32>(updateData, PLAYER_OFFHAND_CRIT_PERCENTAGE, 4.0f);

                        for (int i = 0; i < 127; i++)
                            SetFieldValue<u32>(updateData, PLAYER_EXPLORED_ZONES_1 + i, 0xFFFFFFFF);

                        SetFieldValue<i32>(updateData, PLAYER_REST_STATE_EXPERIENCE, 0);
                        SetFieldValue<u32>(updateData, PLAYER_FIELD_COINAGE, 5000000);

                        for (int i = 0; i < 7; i++)
                        {
                            SetFieldValue<i32>(updateData, PLAYER_FIELD_MOD_DAMAGE_DONE_POS + i, 0);
                            SetFieldValue<i32>(updateData, PLAYER_FIELD_MOD_DAMAGE_DONE_NEG + i, 0);
                            SetFieldValue<f32>(updateData, PLAYER_FIELD_MOD_DAMAGE_DONE_PCT + i, 1.0f);
                        }

                        SetFieldValue<i32>(updateData, PLAYER_FIELD_WATCHED_FACTION_INDEX, -1);
                        SetFieldValue<u32>(updateData, PLAYER_FIELD_MAX_LEVEL, 80);

                        for (int i = 0; i < 3; i++)
                        {
                            SetFieldValue<f32>(updateData, PLAYER_RUNE_REGEN_1 + i, 0.1f);
                        }

                        for (int i = 0; i < 5; i++)
                        {
                            SetFieldValue<f32>(updateData, PLAYER_FIELD_GLYPH_SLOTS_1 + i, f32(21 + i));
                        }

                        packet.handled = true;
                    }
                }
                else
                {
                    // Do we want to set it to true, and then false if we fail or false and then set it to true if we succeed?
                    packet.handled = true;
                    switch (Common::Opcode(packet.opcode))
                    {
                        case Common::Opcode::CMSG_SET_ACTIVE_MOVER:
                        {
                            packetHeader.opcode = Common::Opcode::SMSG_TIME_SYNC_REQ;
                            packetHeader.size = 4;

                            Common::ByteBuffer timeSync(9 + 4);
                            packetHeader.AddTo(timeSync);

                            timeSync.Write<u32>(0);
                            novusConnection.SendPacket(timeSync);
                            break;
                        }
                        case Common::Opcode::CMSG_LOGOUT_REQUEST:
                        {
                            packetHeader.opcode = Common::Opcode::SMSG_LOGOUT_COMPLETE;
                            packetHeader.size = 0;

                            Common::ByteBuffer logoutRequest(0);
                            packetHeader.AddTo(logoutRequest);

                            novusConnection.SendPacket(logoutRequest);
                            break;
                        }
                        case Common::Opcode::CMSG_STANDSTATECHANGE:
                        {
                            u32 standState = 0;
                            packet.data.Read<uint32_t>(standState);

                            /* Should Update Unit Field Here */

                            packetHeader.opcode = Common::Opcode::SMSG_STANDSTATE_UPDATE;
                            packetHeader.size = 1;

                            Common::ByteBuffer standStateChange(0);
                            packetHeader.AddTo(standStateChange);

                            standStateChange.Write<u8>(u8(standState));
                            novusConnection.SendPacket(standStateChange);
                            break;
                        }
                        /* These packets should be read here, but preferbly handled elsewhere */
                        case Common::Opcode::MSG_MOVE_STOP:
                        case Common::Opcode::MSG_MOVE_STOP_STRAFE:
                        case Common::Opcode::MSG_MOVE_STOP_TURN:
                        case Common::Opcode::MSG_MOVE_STOP_PITCH:
                        case Common::Opcode::MSG_MOVE_START_FORWARD:
                        case Common::Opcode::MSG_MOVE_START_BACKWARD:
                        case Common::Opcode::MSG_MOVE_START_STRAFE_LEFT:
                        case Common::Opcode::MSG_MOVE_START_STRAFE_RIGHT:
                        case Common::Opcode::MSG_MOVE_START_TURN_LEFT:
                        case Common::Opcode::MSG_MOVE_START_TURN_RIGHT:
                        case Common::Opcode::MSG_MOVE_START_PITCH_UP:
                        case Common::Opcode::MSG_MOVE_START_PITCH_DOWN:
                        case Common::Opcode::MSG_MOVE_START_ASCEND:
                        case Common::Opcode::MSG_MOVE_STOP_ASCEND:
                        case Common::Opcode::MSG_MOVE_START_DESCEND:
                        case Common::Opcode::MSG_MOVE_START_SWIM:
                        case Common::Opcode::MSG_MOVE_STOP_SWIM:
                        case Common::Opcode::MSG_MOVE_FALL_LAND:
                        case Common::Opcode::CMSG_MOVE_FALL_RESET:
                        case Common::Opcode::MSG_MOVE_JUMP:
                        case Common::Opcode::MSG_MOVE_SET_FACING:
                        case Common::Opcode::MSG_MOVE_SET_PITCH:
                        case Common::Opcode::MSG_MOVE_SET_RUN_MODE:
                        case Common::Opcode::MSG_MOVE_SET_WALK_MODE:
                        case Common::Opcode::CMSG_MOVE_SET_FLY:
                        case Common::Opcode::CMSG_MOVE_CHNG_TRANSPORT:
                        case Common::Opcode::MSG_MOVE_HEARTBEAT:
                        {
                            // Read GUID here as packed
                            uint64_t guid;
                            packet.data.ReadPackedGUID(guid);

                            uint32_t movementFlags;
                            uint16_t movementExtraFlags;
                            uint32_t gameTime;
                            float position_x;
                            float position_y;
                            float position_z;
                            float position_o;
                            uint32_t fallTime;

                            packet.data.Read(&movementFlags, 4);
                            packet.data.Read(&movementExtraFlags, 2);
                            packet.data.Read(&gameTime, 4);
                            packet.data.Read(&position_x, 4);
                            packet.data.Read(&position_y, 4);
                            packet.data.Read(&position_z, 4);
                            packet.data.Read(&position_o, 4);
                            packet.data.Read(&fallTime, 4);

                            uint32_t gameTimeMS = uint32_t(duration_cast<milliseconds>(steady_clock::now() - ApplicationStartTime).count());
                            uint32_t timeDelay = gameTimeMS - gameTime;
                            //std::cout << "Movement Info (" << opcode << "," << guid << "," << movementFlags << "," << movementExtraFlags << ",(" << gameTime << "," << gameTimeMS << "," << timeDelay << ")," << position_x << "," << position_y << "," << position_z << "," << position_o << "," << fallTime << ")" << std::endl;

                            Common::ByteBuffer movementData(packet.data.size());
                            movementData.AppendGuid(guid);
                            movementData.Write<uint32_t>(movementFlags); // MovementFlags
                            movementData.Write<uint16_t>(movementExtraFlags); // Extra MovementFlags
                            movementData.Write<uint32_t>(gameTime + timeDelay); // Game Time
                            movementData.Write<float>(position_x);
                            movementData.Write<float>(position_y);
                            movementData.Write<float>(position_z);
                            movementData.Write<float>(position_o);

                            // FallTime
                            movementData.Write<uint32_t>(fallTime);

                            positionData.x = position_x;
                            positionData.y = position_y;
                            positionData.z = position_z;
                            positionData.o = position_o;

                            subView.each([this, opcode, guid, movementData, &novusConnection](const auto, ConnectionComponent& connection)
                            {
                                if (guid != connection.characterGuid)
                                {
                                    NovusHeader packetHeader;
                                    packetHeader.command = NOVUS_FORWARDPACKET;
                                    packetHeader.account = connection.accountGuid;
                                    packetHeader.opcode = opcode;
                                    packetHeader.size = movementData.size();

                                    Common::ByteBuffer writtenData(packetHeader.size);
                                    packetHeader.AddTo(writtenData);
                                    writtenData.Append(movementData);

                                    novusConnection.SendPacket(writtenData);
                                }
                            });

                            //SendPacket(movementData, opcode);

                            break;
                        }
                    }
                }
            }

            std::vector<OpcodePacket>& packets = connection.packets;
            if (packets.size() > 0)
            {
                packets.erase(std::remove_if(packets.begin(), packets.end(), [](OpcodePacket& packet)
                {
                    return packet.handled;
                }), packets.end());
            }
        });
    }
}