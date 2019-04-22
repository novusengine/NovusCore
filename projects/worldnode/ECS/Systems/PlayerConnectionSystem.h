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
#include <entt.hpp>
#include <NovusTypes.h>
#include <Networking/Opcode/Opcode.h>
#include <Utils/DebugHandler.h>
#include <Utils/AtomicLock.h>
#include <Math/Math.h>
#include <Math/Vector2.h>
#include <Cryptography/HMAC.h>

#include "../../NovusEnums.h"
#include "../../Utils/CharacterUtils.h"
#include "../../DatabaseCache/CharacterDatabaseCache.h"
#include "../../WorldNodeHandler.h"

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerFieldDataComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/PlayerPositionComponent.h"

#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerDeleteQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"
#include "../Components/Singletons/WorldDatabaseCacheSingleton.h"
#include "../Components/Singletons/PlayerPacketQueueSingleton.h"
#include "../Components/Singletons/MapSingleton.h"

#include <tracy/Tracy.hpp>

namespace ConnectionSystem
{
    void Update(entt::registry &registry)
    {
        SingletonComponent& singleton = registry.ctx<SingletonComponent>();
        PlayerDeleteQueueSingleton& playerDeleteQueue = registry.ctx<PlayerDeleteQueueSingleton>();
        CharacterDatabaseCacheSingleton& characterDatabase = registry.ctx<CharacterDatabaseCacheSingleton>();
        WorldDatabaseCacheSingleton& worldDatabase = registry.ctx<WorldDatabaseCacheSingleton>();
        PlayerPacketQueueSingleton& playerPacketQueue = registry.ctx<PlayerPacketQueueSingleton>();
        WorldNodeHandler& worldNodeHandler = *singleton.worldNodeHandler;
		MapSingleton& mapSingleton = registry.ctx<MapSingleton>();

        LockRead(SingletonComponent);
        LockRead(PlayerDeleteQueueSingleton);
        LockRead(CharacterDatabaseCacheSingleton);

        LockWrite(PlayerConnectionComponent);
        LockWrite(PlayerFieldDataComponent);
        LockWrite(PlayerUpdateDataComponent);
        LockWrite(PlayerPositionComponent);

        auto view = registry.view<PlayerConnectionComponent, PlayerFieldDataComponent, PlayerUpdateDataComponent, PlayerPositionComponent>();
        view.each([&singleton, &playerDeleteQueue, &characterDatabase, &worldDatabase, &playerPacketQueue, &worldNodeHandler, &mapSingleton](const auto, PlayerConnectionComponent& clientConnection, PlayerFieldDataComponent& clientFieldData, PlayerUpdateDataComponent& clientUpdateData, PlayerPositionComponent& clientPositionData)
        {
            ZoneScopedNC("Connection", tracy::Color::Orange2)

                for (OpcodePacket& packet : clientConnection.packets)
                {
                    ZoneScopedNC("Packet", tracy::Color::Orange2)

                        Common::Opcode opcode = static_cast<Common::Opcode>(packet.opcode);
                    switch (static_cast<Common::Opcode>(packet.opcode))
                    {
                    case Common::Opcode::CMSG_SET_ACTIVE_MOVER:
                    {
                        ZoneScopedNC("Packet::SetActiveMover", tracy::Color::Orange2)

                            Common::ByteBuffer timeSync(9 + 4);
                        timeSync.Write<u32>(0);

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, timeSync, Common::Opcode::SMSG_TIME_SYNC_REQ));
                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_LOGOUT_REQUEST:
                    {
                        ZoneScopedNC("Packet::LogoutRequest", tracy::Color::Orange2)

                            Common::ByteBuffer logoutRequest(0);

                        //playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, logoutRequest, Common::Opcode::SMSG_LOGOUT_COMPLETE));
                        clientConnection.socket->SendPacket(logoutRequest, Common::Opcode::SMSG_LOGOUT_COMPLETE);

                        // Here we need to Redirect the client back to Realmserver
                        Common::ByteBuffer redirectClient;
                        i32 ip = 16777343;
                        i16 port = 8000;

                        // 127.0.0.1/1.0.0.127
                        // 2130706433/16777343(https://www.browserling.com/tools/ip-to-dec)
                        redirectClient.Write<i32>(ip);
                        redirectClient.Write<i16>(port);
                        redirectClient.Write<i32>(0); // unk
#pragma warning(push)
#pragma warning(disable: 4312)
                        HMACH hmac(40, clientConnection.socket->sessionKey.BN2BinArray(20).get());
                        hmac.UpdateHash((u8*)& ip, 4);
                        hmac.UpdateHash((u8*)& port, 2);
                        hmac.Finish();
                        redirectClient.Append(hmac.GetData(), 20);
#pragma warning(pop)
                        clientConnection.socket->SendPacket(redirectClient, Common::Opcode::SMSG_REDIRECT_CLIENT);

                        ExpiredPlayerData expiredPlayerData;
                        expiredPlayerData.entityGuid = clientConnection.entityGuid;
                        expiredPlayerData.accountGuid = clientConnection.accountGuid;
                        expiredPlayerData.characterGuid = clientConnection.characterGuid;
                        playerDeleteQueue.expiredEntityQueue->enqueue(expiredPlayerData);

                        CharacterData characterData;
                        characterDatabase.cache->GetCharacterData(clientConnection.characterGuid, characterData);

                        characterData.level = clientFieldData.GetFieldValue<u32>(UNIT_FIELD_LEVEL);
                        characterData.mapId = clientPositionData.mapId;
                        characterData.coordinateX = clientPositionData.x;
                        characterData.coordinateY = clientPositionData.y;
                        characterData.coordinateZ = clientPositionData.z;
                        characterData.orientation = clientPositionData.orientation;
                        characterData.online = 0;
                        characterData.UpdateCache(clientConnection.characterGuid);

                        characterDatabase.cache->SaveAndUnloadCharacter(clientConnection.characterGuid);


                        Common::ByteBuffer suspendComms;
                        suspendComms.Write<u32>(1);
                        clientConnection.socket->SendPacket(suspendComms, Common::Opcode::SMSG_SUSPEND_COMMS);

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_STANDSTATECHANGE:
                    {
                        ZoneScopedNC("Packet::StandStateChange", tracy::Color::Orange2)

                            u32 standState = 0;
                        packet.data.Read<u32>(standState);

                        clientFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_1, static_cast<u8>(standState));

                        Common::ByteBuffer standStateChange(0);
                        standStateChange.Write<u8>(static_cast<u8>(standState));

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, standStateChange, Common::Opcode::SMSG_STANDSTATE_UPDATE));

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_SET_SELECTION:
                    {
                        ZoneScopedNC("Packet::SetSelection", tracy::Color::Orange2)

                            u64 selectedGuid = 0;
                        packet.data.ReadPackedGUID(selectedGuid);

                        clientFieldData.SetGuidValue(UNIT_FIELD_TARGET, selectedGuid);
                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_NAME_QUERY:
                    {
                        ZoneScopedNC("Packet::NameQuery", tracy::Color::Orange2)

                            u64 guid;
                        packet.data.Read<u64>(guid);

                        Common::ByteBuffer nameQuery;
                        nameQuery.AppendGuid(guid);

                        CharacterData characterData;
                        if (characterDatabase.cache->GetCharacterData(guid, characterData))
                        {
                            nameQuery.Write<u8>(0); // Name Unknown (0 = false, 1 = true);
                            nameQuery.WriteString(characterData.name);
                            nameQuery.Write<u8>(0);
                            nameQuery.Write<u8>(characterData.race);
                            nameQuery.Write<u8>(characterData.gender);
                            nameQuery.Write<u8>(characterData.classId);
                        }
                        else
                        {
                            nameQuery.Write<u8>(1); // Name Unknown (0 = false, 1 = true);
                            nameQuery.WriteString("Unknown");
                            nameQuery.Write<u8>(0);
                            nameQuery.Write<u8>(0);
                            nameQuery.Write<u8>(0);
                            nameQuery.Write<u8>(0);
                        }
                        nameQuery.Write<u8>(0);

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, nameQuery, Common::Opcode::SMSG_NAME_QUERY_RESPONSE));

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_ITEM_QUERY_SINGLE:
                    {
                        u32 itemEntry;
                        packet.data.Read<u32>(itemEntry);
                        Common::ByteBuffer itemQuery;


                        ItemTemplate itemTemplate;
                        if (!worldDatabase.cache->GetItemTemplate(itemEntry, itemTemplate))
                        {
                            itemQuery.Write<u32>(itemEntry | 0x80000000);
                        }
                        else
                        {
                            itemQuery = itemTemplate.GetQuerySinglePacket();
                        }

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, itemQuery, Common::Opcode::SMSG_ITEM_QUERY_SINGLE_RESPONSE));

                        packet.handled = true;
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
                        ZoneScopedNC("Packet::Passthrough", tracy::Color::Orange2)

                            // Read GUID here as packed
                            u64 guid;
                        packet.data.ReadPackedGUID(guid);

                        u32 movementFlags;
                        u16 movementFlagsExtra;
                        u32 gameTime;
                        f32 position_x;
                        f32 position_y;
                        f32 position_z;
                        f32 orientation;
                        u32 fallTime;

                        packet.data.Read(&movementFlags, 4);
                        packet.data.Read(&movementFlagsExtra, 2);
                        packet.data.Read(&gameTime, 4);
                        packet.data.Read(&position_x, 4);
                        packet.data.Read(&position_y, 4);
                        packet.data.Read(&position_z, 4);
                        packet.data.Read(&orientation, 4);
                        packet.data.Read(&fallTime, 4);

                        u8 opcodeIndex = CharacterUtils::GetLastMovementTimeIndexFromOpcode(opcode);
                        u32 opcodeTime = clientPositionData.lastMovementOpcodeTime[opcodeIndex];
                        if (gameTime > opcodeTime)
                        {
                            clientPositionData.lastMovementOpcodeTime[opcodeIndex] = gameTime;

                            PositionUpdateData positionUpdateData;
                            positionUpdateData.opcode = opcode;
                            positionUpdateData.movementFlags = movementFlags;
                            positionUpdateData.movementFlagsExtra = movementFlagsExtra;
                            positionUpdateData.gameTime = static_cast<u32>(singleton.lifeTimeInMS);
                            positionUpdateData.fallTime = fallTime;

                            clientPositionData.x = position_x;
                            clientPositionData.y = position_y;
                            clientPositionData.z = position_z;
                            clientPositionData.orientation = orientation;

                            positionUpdateData.x = position_x;
                            positionUpdateData.y = position_y;
                            positionUpdateData.z = position_z;
                            positionUpdateData.orientation = orientation;

                            clientUpdateData.positionUpdateData.push_back(positionUpdateData);
                        }

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_MESSAGECHAT:
                    {
                        ZoneScopedNC("Packet::MessageChat", tracy::Color::Orange2)
                            packet.handled = true;

                        u32 msgType;
                        u32 msgLang;

                        packet.data.Read<u32>(msgType);
                        packet.data.Read<u32>(msgLang);

                        if (msgType >= MAX_MSG_TYPE)
                        {
                            // Client tried to use invalid type
                            break;
                        }

                        if (msgLang == LANG_UNIVERSAL && msgType != CHAT_MSG_AFK && msgType != CHAT_MSG_DND)
                        {
                            // Client tried to send a message in universal language. (While it not being afk or dnd)
                            break;
                        }

                        if (msgType == CHAT_MSG_AFK || msgType == CHAT_MSG_DND)
                        {
                            // We don't want to send this message to any client.
                            break;
                        }

                        std::string msgOutput;
                        switch (msgType)
                        {
                        case CHAT_MSG_SAY:
                        case CHAT_MSG_YELL:
                        case CHAT_MSG_EMOTE:
                        case CHAT_MSG_TEXT_EMOTE:
                        {
                            packet.data.Read(msgOutput);
                            break;
                        }

                        default:
                        {
                            worldNodeHandler.PrintMessage("Account(%u), Character(%u) sent unhandled message type %u", clientConnection.accountGuid, clientConnection.characterGuid, msgType);
                            break;
                        }
                        }

                        // Max Message Size is 255
                        if (msgOutput.size() > 255)
                            break;

                        /* Build Packet */
                        ChatUpdateData chatUpdateData;
                        chatUpdateData.chatType = msgType;
                        chatUpdateData.language = msgLang;
                        chatUpdateData.sender = clientConnection.characterGuid;
                        chatUpdateData.message = msgOutput;
                        chatUpdateData.handled = false;
                        clientUpdateData.chatUpdateData.push_back(chatUpdateData);
                        break;
                    }
                    case Common::Opcode::CMSG_ATTACKSWING:
                    {
                        ZoneScopedNC("Packet::AttackSwing", tracy::Color::Orange2)

                            u64 attackGuid;
                        packet.data.Read<u64>(attackGuid);

                        Common::ByteBuffer attackStart;
                        attackStart.Write<u64>(clientConnection.characterGuid);
                        attackStart.Write<u64>(attackGuid);

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, attackStart, Common::Opcode::SMSG_ATTACKSTART));

                        Common::ByteBuffer attackerStateUpdate;
                        attackerStateUpdate.Write<u32>(0);
                        attackerStateUpdate.AppendGuid(clientConnection.characterGuid);
                        attackerStateUpdate.AppendGuid(attackGuid);
                        attackerStateUpdate.Write<u32>(5);
                        attackerStateUpdate.Write<u32>(0);
                        attackerStateUpdate.Write<u8>(1);

                        attackerStateUpdate.Write<u32>(1);
                        attackerStateUpdate.Write<f32>(5);
                        attackerStateUpdate.Write<u32>(5);

                        attackerStateUpdate.Write<u8>(0);
                        attackerStateUpdate.Write<u32>(0);
                        attackerStateUpdate.Write<u32>(0);

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, attackerStateUpdate, Common::Opcode::SMSG_ATTACKERSTATEUPDATE));

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_ATTACKSTOP:
                    {
                        ZoneScopedNC("Packet::AttackStop", tracy::Color::Orange2)

                            u64 attackGuid = clientFieldData.GetFieldValue<u64>(UNIT_FIELD_TARGET);

                        Common::ByteBuffer attackStop;
                        attackStop.AppendGuid(clientConnection.characterGuid);
                        attackStop.AppendGuid(attackGuid);
                        attackStop.Write<u32>(0);

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, attackStop, Common::Opcode::SMSG_ATTACKSTOP));

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_SETSHEATHED:
                    {
                        ZoneScopedNC("Packet::SetSheathed", tracy::Color::Orange2)

                            u32 state;
                        packet.data.Read<u32>(state);

                        clientFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, static_cast<u8>(state));
                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_TEXT_EMOTE:
                    {
                        ZoneScopedNC("Packet::Text_emote", tracy::Color::Orange2)

                        u32 textEmote;
                        u32 emoteNum;
                        u64 targetGuid;

                        packet.data.Read<u32>(textEmote);
                        packet.data.Read<u32>(emoteNum);
                        packet.data.Read<u64>(targetGuid);

                        u32 animationID;
                        /* Pulling animation ID from database code here. */

                        animationID = 10;

                        /* End pulling animation ID from database here. */

                        /* Play animation packet. */
                        {
                            //The animation shouldn't play if the player is dead. In the future we should check for that.

                            Common::ByteBuffer emote;
                            emote.Write<u32>(animationID);
                            emote.Write<u64>(clientConnection.characterGuid);

                            playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, emote, Common::Opcode::SMSG_EMOTE));
                        }

                        /* Emote Chat Message Packet. */
                        {
                            CharacterData targetData;
                            characterDatabase.cache->GetCharacterData(targetGuid, targetData);

                            u32 targetNameLength = static_cast<u32>(targetData.name.size());

                            Common::ByteBuffer textEmoteMessage;
                            textEmoteMessage.Write<u64>(clientConnection.characterGuid);
                            textEmoteMessage.Write<u32>(textEmote);
                            textEmoteMessage.Write<u32>(emoteNum);
                            textEmoteMessage.Write<u32>(targetNameLength);
                            if (targetNameLength > 1)
                            {
                                textEmoteMessage.Write(targetData.name);
                            }
                            else
                            {
                                textEmoteMessage.Write<u8>(0x00);
                            }

                            playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, textEmoteMessage, Common::Opcode::SMSG_TEXT_EMOTE));
                        }

                        packet.handled = true;
                        break;
                    }
					case Common::Opcode::CMSG_CAST_SPELL:
					{
						packet.handled = true;

						u32 spellId = 0, targetFlags = 0;
						u8 castCount = 0, castFlags = 0;

						packet.data.Read<u8>(castCount);
						packet.data.Read<u32>(spellId);
						packet.data.Read<u8>(castFlags);
						packet.data.Read<u32>(targetFlags);

						// As far as I can tell, the client expects SMSG_SPELL_START followed by SMSG_SPELL_GO.

						// Handle blink!
						if (spellId == 1953)
						{
							f32 tempHeight = clientPositionData.z;
							u32 dest = 20;

							for (u32 i = 0; i < 20; i++)
							{
								f32 newPositionX = clientPositionData.x + i * Math::Cos(clientPositionData.orientation);
								f32 newPositionY = clientPositionData.y + i * Math::Sin(clientPositionData.orientation);
                                Vector2 newPos(newPositionX, newPositionY);
								f32 height = mapSingleton.maps[clientPositionData.mapId].GetHeight(newPos);
								f32 deltaHeight = Math::Abs(tempHeight - height);

								if (deltaHeight <= 2.0f || (i == 0 && deltaHeight <= 20))
								{
									dest = i;
									tempHeight = height;
								}
							}

							if (dest == 20)
							{
								Common::ByteBuffer spellFailed;
								spellFailed.Write<u8>(castCount);
								spellFailed.Write<u32>(spellId);
								spellFailed.Write<u8>(173); // SPELL_FAILED_TRY_AGAIN

								playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, spellFailed, Common::Opcode::SMSG_CAST_FAILED));
								break;
							}


							f32 newPositionX = clientPositionData.x + dest * Math::Cos(clientPositionData.orientation);
							f32 newPositionY = clientPositionData.y + dest * Math::Sin(clientPositionData.orientation);

							/*
								Adding 2.0f to the final height will solve 90%+ of issues where we fall through the terrain, remove this to fully test blink's capabilities.
								This also introduce the bug where after a blink, you might appear a bit over the ground and fall down.
							*/
                            Vector2 newPos(newPositionX, newPositionY);
							f32 height = mapSingleton.maps[clientPositionData.mapId].GetHeight(newPos);

							Common::ByteBuffer buffer;
							buffer.AppendGuid(clientConnection.characterGuid);
							buffer.Write<u32>(0); // Teleport Count

							/* Movement */
							buffer.Write<u32>(0);
							buffer.Write<u16>(0);
							buffer.Write<u32>(static_cast<u32>(singleton.lifeTimeInMS));

							buffer.Write<f32>(newPositionX);
							buffer.Write<f32>(newPositionY);
							buffer.Write<f32>(height);
							buffer.Write<f32>(clientPositionData.orientation);

							buffer.Write<u32>(targetFlags);

							playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, buffer, Common::Opcode::MSG_MOVE_TELEPORT_ACK));
						}
						Common::ByteBuffer spellStart;
						spellStart.AppendGuid(clientConnection.characterGuid);
						spellStart.AppendGuid(clientConnection.characterGuid);
						spellStart.Write<u8>(0); // CastCount
						spellStart.Write<u32>(spellId);
						spellStart.Write<u32>(0x00000002);
						spellStart.Write<u32>(0);
						spellStart.Write<u32>(0);

						playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, spellStart, Common::Opcode::SMSG_SPELL_START));

						Common::ByteBuffer spellCast;
						spellCast.AppendGuid(clientConnection.characterGuid);
						spellCast.AppendGuid(clientConnection.characterGuid);
						spellCast.Write<u8>(0); // CastCount
						spellCast.Write<u32>(spellId);
						spellCast.Write<u32>(0x00000100);
						spellCast.Write<u32>(static_cast<u32>(singleton.lifeTimeInMS));

						spellCast.Write<u8>(1); // Affected Targets
						spellCast.Write<u64>(clientConnection.characterGuid); // Target GUID
						spellCast.Write<u8>(0); // Resisted Targets

						if (targetFlags == 0) // SELF
						{
							targetFlags = 0x02; // UNIT
						}
						spellCast.Write<u32>(targetFlags); // Target Flags
						spellCast.Write<u8>(0); // Target Flags

						playerPacketQueue.packetQueue->enqueue(PacketQueueData(clientConnection.socket, spellCast, Common::Opcode::SMSG_SPELL_GO));
						break;
					}
                    default:
                    {
                        ZoneScopedNC("Packet::Unhandled", tracy::Color::Orange2)
                        {
                            ZoneScopedNC("Packet::Unhandled::Log", tracy::Color::Orange2)
                                worldNodeHandler.PrintMessage("Account(%u), Character(%u) sent unhandled opcode %u", clientConnection.accountGuid, clientConnection.characterGuid, opcode);
                        }

                        // Mark all unhandled opcodes as handled to prevent the queue from trying to handle them every tick.
                        packet.handled = true;
                        break;
                    }
                    }
                }
            /* Cull Movement Data */

            if (clientConnection.packets.size() > 0)
            {
                ZoneScopedNC("Packet::PacketClear", tracy::Color::Orange2)
                    clientConnection.packets.erase(std::remove_if(clientConnection.packets.begin(), clientConnection.packets.end(), [](OpcodePacket& packet)
                {
                    return packet.handled;
                }), clientConnection.packets.end());
            }
        });
    }
}
