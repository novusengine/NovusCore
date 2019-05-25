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
#include <zlib.h>

#include "../../NovusEnums.h"
#include "../../Utils/CharacterUtils.h"
#include "../../DatabaseCache/CharacterDatabaseCache.h"
#include "../../DatabaseCache/DBCDatabaseCache.h"
#include "../../WorldNodeHandler.h"

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerFieldDataComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/PlayerPositionComponent.h"

#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerDeleteQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"
#include "../Components/Singletons/WorldDatabaseCacheSingleton.h"
#include "../Components/Singletons/DBCDatabaseCacheSingleton.h"
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
        DBCDatabaseCacheSingleton& dbcDatabase = registry.ctx<DBCDatabaseCacheSingleton>();
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
        view.each([&singleton, &playerDeleteQueue, &characterDatabase, &worldDatabase, &dbcDatabase, &playerPacketQueue, &worldNodeHandler, &mapSingleton](const auto, PlayerConnectionComponent& playerConnection, PlayerFieldDataComponent& clientFieldData, PlayerUpdateDataComponent& clientUpdateData, PlayerPositionComponent& clientPositionData)
        {
            ZoneScopedNC("Connection", tracy::Color::Orange2)

                for (OpcodePacket& packet : playerConnection.packets)
                {
                    ZoneScopedNC("Packet", tracy::Color::Orange2)

                        Common::Opcode opcode = static_cast<Common::Opcode>(packet.opcode);
                    switch (opcode)
                    {
                    case Common::Opcode::CMSG_SET_ACTIVE_MOVER:
                    {
                        ZoneScopedNC("Packet::SetActiveMover", tracy::Color::Orange2)

                            Common::ByteBuffer timeSync(9 + 4);
                        timeSync.Write<u32>(0);

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, timeSync, Common::Opcode::SMSG_TIME_SYNC_REQ));
                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_LOGOUT_REQUEST:
                    {
                        ZoneScopedNC("Packet::LogoutRequest", tracy::Color::Orange2)

                        ExpiredPlayerData expiredPlayerData;
                        expiredPlayerData.entityGuid = playerConnection.entityGuid;
                        expiredPlayerData.accountGuid = playerConnection.accountGuid;
                        expiredPlayerData.characterGuid = playerConnection.characterGuid;
                        playerDeleteQueue.expiredEntityQueue->enqueue(expiredPlayerData);

                        CharacterInfo characterInfo;
                        characterDatabase.cache->GetCharacterInfo(playerConnection.characterGuid, characterInfo);

                        characterInfo.level = clientFieldData.GetFieldValue<u32>(UNIT_FIELD_LEVEL);
                        characterInfo.mapId = clientPositionData.mapId;
                        characterInfo.coordinateX = clientPositionData.x;
                        characterInfo.coordinateY = clientPositionData.y;
                        characterInfo.coordinateZ = clientPositionData.z;
                        characterInfo.orientation = clientPositionData.orientation;
                        characterInfo.online = 0;
                        characterInfo.UpdateCache(playerConnection.characterGuid);

                        characterDatabase.cache->SaveAndUnloadCharacter(playerConnection.characterGuid);

                        // Here we need to Redirect the client back to Realmserver. The Realmserver will send SMSG_LOGOUT_COMPLETE
                        Common::ByteBuffer redirectClient;
                        i32 ip = 16777343;
                        i16 port = 8001;

                        // 127.0.0.1/1.0.0.127
                        // 2130706433/16777343(https://www.browserling.com/tools/ip-to-dec)
                        redirectClient.Write<i32>(ip);
                        redirectClient.Write<i16>(port);
                        redirectClient.Write<i32>(0); // unk
#pragma warning(push)
#pragma warning(disable: 4312)
                        HMACH hmac(40, playerConnection.socket->sessionKey.BN2BinArray(20).get());
                        hmac.UpdateHash((u8*)& ip, 4);
                        hmac.UpdateHash((u8*)& port, 2);
                        hmac.Finish();
                        redirectClient.Append(hmac.GetData(), 20);
#pragma warning(pop)
                        playerConnection.socket->SendPacket(redirectClient, Common::Opcode::SMSG_REDIRECT_CLIENT);

                        Common::ByteBuffer suspendComms;
                        suspendComms.Write<u32>(1);
                        playerConnection.socket->SendPacket(suspendComms, Common::Opcode::SMSG_SUSPEND_COMMS);

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_READY_FOR_ACCOUNT_DATA_TIMES:
                    {
                        /* Packet Structure */
                        // UInt32:  Server Time (time(nullptr))
                        // UInt8:   Unknown Byte Value
                        // UInt32:  Mask for the account data fields

                        Common::ByteBuffer accountDataTimes;

                        u32 mask = 0x15;
                        accountDataTimes.Write<u32>(static_cast<u32>(time(nullptr)));
                        accountDataTimes.Write<u8>(1); // bitmask blocks count
                        accountDataTimes.Write<u32>(mask);

                        for (u32 i = 0; i < 8; ++i)
                        {
                            if (mask & (1 << i))
                            {
                                CharacterData characterData;
                                if (characterDatabase.cache->GetCharacterData(playerConnection.characterGuid, i, characterData))
                                {
                                    accountDataTimes.Write<u32>(characterData.timestamp);
                                }
                            }
                        }

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, accountDataTimes, Common::Opcode::SMSG_ACCOUNT_DATA_TIMES));
                        packet.handled = true; 
                        break;
                    }
                    case Common::Opcode::CMSG_UPDATE_ACCOUNT_DATA:
                    {
                        packet.handled = true;

                        u32 type, timestamp, decompressedSize;
                        packet.data.Read<u32>(type);
                        packet.data.Read<u32>(timestamp);
                        packet.data.Read<u32>(decompressedSize);

                        if (type > 8)
                        {
                            break;
                        }

                        bool characterDataUpdate = ((1 << type) & CHARACTER_DATA_CACHE_MASK);

                        // This is here temporarily as I'm not certain if the client will UPDATE any AccountData while connected to a WorldNode
                        if (!characterDataUpdate)
                        {
                            NC_LOG_WARNING("Received AccountDataUpdate");
                            break;
                        }

                        // Clear Data
                        if (decompressedSize == 0)
                        {
                            if (characterDataUpdate)
                            {
                                CharacterData characterData;
                                if (characterDatabase.cache->GetCharacterData(playerConnection.characterGuid, type, characterData))
                                {
                                    characterData.timestamp = 0;
                                    characterData.data = "";
                                    characterData.UpdateCache();
                                }
                            }
                        }
                        else
                        {
                            if (decompressedSize > 0xFFFF)
                            {
                                break;
                            }

                            Common::ByteBuffer DataInfo;
                            DataInfo.Append(packet.data.data() + packet.data._readPos, packet.data.size() - packet.data._readPos);

                            uLongf uSize = decompressedSize;
                            u32 pos = static_cast<u32>(DataInfo._readPos);

                            Common::ByteBuffer dataInfo;
                            dataInfo.Resize(decompressedSize);

                            if (uncompress(dataInfo.data(), &uSize, DataInfo.data() + pos, DataInfo.size() - pos) != Z_OK)
                            {
                                break;
                            }

                            std::string finalData = "";
                            dataInfo.Read(finalData);

                            if (characterDataUpdate)
                            {
                                CharacterData characterData;
                                if (characterDatabase.cache->GetCharacterData(playerConnection.characterGuid, type, characterData))
                                {
                                    characterData.timestamp = timestamp;
                                    characterData.data = finalData;
                                    characterData.UpdateCache();
                                }
                            }
                        }

                        Common::ByteBuffer updateAccountDataComplete(4 + 4);
                        updateAccountDataComplete.Write<u32>(type);
                        updateAccountDataComplete.Write<u32>(0);

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, updateAccountDataComplete, Common::Opcode::SMSG_UPDATE_ACCOUNT_DATA_COMPLETE));
                        break;
                    }
                    case Common::Opcode::MSG_MOVE_SET_ALL_SPEED_CHEAT:
                    {
                        f32 speed = 1;
                        packet.data.Read<f32>(speed);

                        Common::ByteBuffer speedChange;
                        CharacterUtils::BuildSpeedChangePacket(playerConnection.accountGuid, playerConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_WALK_SPEED_CHANGE, speedChange);
                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, speedChange, Common::Opcode::SMSG_FORCE_WALK_SPEED_CHANGE));

                        CharacterUtils::BuildSpeedChangePacket(playerConnection.accountGuid, playerConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_RUN_SPEED_CHANGE, speedChange);
                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, speedChange, Common::Opcode::SMSG_FORCE_RUN_SPEED_CHANGE));

                        CharacterUtils::BuildSpeedChangePacket(playerConnection.accountGuid, playerConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE, speedChange);
                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, speedChange, Common::Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE));

                        CharacterUtils::BuildSpeedChangePacket(playerConnection.accountGuid, playerConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE, speedChange);
                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, speedChange, Common::Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE));

                        CharacterUtils::BuildSpeedChangePacket(playerConnection.accountGuid, playerConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE, speedChange);
                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, speedChange, Common::Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE));

                        CharacterUtils::BuildSpeedChangePacket(playerConnection.accountGuid, playerConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE, speedChange);
                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, speedChange, Common::Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE));

                        CharacterUtils::BuildSpeedChangePacket(playerConnection.accountGuid, playerConnection.characterGuid, speed, Common::Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE, speedChange);
                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, speedChange, Common::Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE));

                        playerConnection.SendChatNotification("Speed Updated: %f", speed);
                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_MOVE_START_SWIM_CHEAT:
                    {
                        Common::ByteBuffer enableFlying = CharacterUtils::BuildFlyModePacket(playerConnection.accountGuid, playerConnection.characterGuid);
                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, enableFlying, Common::Opcode::SMSG_MOVE_SET_CAN_FLY));

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_MOVE_STOP_SWIM_CHEAT:
                    {
                        Common::ByteBuffer disableFlying = CharacterUtils::BuildFlyModePacket(playerConnection.accountGuid, playerConnection.characterGuid);
                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, disableFlying, Common::Opcode::SMSG_MOVE_UNSET_CAN_FLY));

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_QUERY_OBJECT_POSITION:
                    {
                        packet.handled = true;

                        Common::ByteBuffer objectPosition;
                        objectPosition.Write<f32>(clientPositionData.x);
                        objectPosition.Write<f32>(clientPositionData.y);
                        objectPosition.Write<f32>(clientPositionData.z);

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, objectPosition, Common::Opcode::SMSG_QUERY_OBJECT_POSITION));
                        break;
                    }
                    case Common::Opcode::CMSG_LEVEL_CHEAT:
                    {
                        packet.handled = true;
                        u32 level = 0;
                        packet.data.Read<u32>(level);

                        if (level != clientFieldData.GetFieldValue<u32>(UNIT_FIELD_LEVEL))
                            clientFieldData.SetFieldValue<u32>(UNIT_FIELD_LEVEL, level);
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

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, standStateChange, Common::Opcode::SMSG_STANDSTATE_UPDATE));

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

                        CharacterInfo characterInfo;
                        if (characterDatabase.cache->GetCharacterInfo(guid, characterInfo))
                        {
                            nameQuery.Write<u8>(0); // Name Unknown (0 = false, 1 = true);
                            nameQuery.WriteString(characterInfo.name);
                            nameQuery.Write<u8>(0);
                            nameQuery.Write<u8>(characterInfo.race);
                            nameQuery.Write<u8>(characterInfo.gender);
                            nameQuery.Write<u8>(characterInfo.classId);
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

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, nameQuery, Common::Opcode::SMSG_NAME_QUERY_RESPONSE));

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

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, itemQuery, Common::Opcode::SMSG_ITEM_QUERY_SINGLE_RESPONSE));

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

                        u64 guid;
                        u32 movementFlags;
                        u16 movementFlagsExtra;
                        u32 gameTime;
                        f32 position_x;
                        f32 position_y;
                        f32 position_z;
                        f32 orientation;
                        u32 fallTime;

                        u8 opcodeIndex = CharacterUtils::GetLastMovementTimeIndexFromOpcode(opcode);
                        u32 opcodeTime = clientPositionData.lastMovementOpcodeTime[opcodeIndex];

                        // Handle GM Client 'facing' command
                        if (opcode == Common::Opcode::MSG_MOVE_SET_FACING && packet.data.GetActualSize() == 4)
                        {
                            guid = playerConnection.characterGuid;
                            movementFlags = 0;
                            movementFlagsExtra = 0;
                            gameTime = opcodeTime + 1;
                            position_x = clientPositionData.x;
                            position_y = clientPositionData.y;
                            position_z = clientPositionData.z;
                            packet.data.Read(&orientation, 4);
                            fallTime = 0;

                            Common::ByteBuffer setFacing;
                            setFacing.AppendGuid(guid);
                            setFacing.Write<u32>(movementFlags);
                            setFacing.Write<u16>(movementFlagsExtra);
                            setFacing.Write<u32>(gameTime);
                            setFacing.Write<f32>(position_x);
                            setFacing.Write<f32>(position_y);
                            setFacing.Write<f32>(position_z);
                            setFacing.Write<f32>(orientation);
                            setFacing.Write<u32>(fallTime);
                            playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, setFacing, Common::Opcode::MSG_MOVE_SET_FACING));
                            playerConnection.SendConsoleNotification("Facing set to %f", orientation);
                        }
                        else
                        {
                            // Read GUID here as packed  
                            packet.data.ReadPackedGUID(guid);

                            packet.data.Read(&movementFlags, 4);
                            packet.data.Read(&movementFlagsExtra, 2);
                            packet.data.Read(&gameTime, 4);
                            packet.data.Read(&position_x, 4);
                            packet.data.Read(&position_y, 4);
                            packet.data.Read(&position_z, 4);
                            packet.data.Read(&orientation, 4);
                            packet.data.Read(&fallTime, 4);
                        }

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
                                worldNodeHandler.PrintMessage("Account(%u), Character(%u) sent unhandled message type %u", playerConnection.accountGuid, playerConnection.characterGuid, msgType);
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
                        chatUpdateData.sender = playerConnection.characterGuid;
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
                        attackStart.Write<u64>(playerConnection.characterGuid);
                        attackStart.Write<u64>(attackGuid);

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, attackStart, Common::Opcode::SMSG_ATTACKSTART));

                        Common::ByteBuffer attackerStateUpdate;
                        attackerStateUpdate.Write<u32>(0);
                        attackerStateUpdate.AppendGuid(playerConnection.characterGuid);
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

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, attackerStateUpdate, Common::Opcode::SMSG_ATTACKERSTATEUPDATE));

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_ATTACKSTOP:
                    {
                        ZoneScopedNC("Packet::AttackStop", tracy::Color::Orange2)

                            u64 attackGuid = clientFieldData.GetFieldValue<u64>(UNIT_FIELD_TARGET);

                        Common::ByteBuffer attackStop;
                        attackStop.AppendGuid(playerConnection.characterGuid);
                        attackStop.AppendGuid(attackGuid);
                        attackStop.Write<u32>(0);

                        playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, attackStop, Common::Opcode::SMSG_ATTACKSTOP));

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

                        
                        EmoteData emoteData;
                        if (dbcDatabase.cache->GetEmoteData(textEmote, emoteData))
                        {
                            /* Play animation packet. */
                            {
                                //The animation shouldn't play if the player is dead. In the future we should check for that.

                                Common::ByteBuffer emote;
                                emote.Write<u32>(emoteData.animationId);
                                emote.Write<u64>(playerConnection.characterGuid);

                                playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, emote, Common::Opcode::SMSG_EMOTE));
                            }

                            /* Emote Chat Message Packet. */
                            {
                                CharacterInfo targetData;
                                u32 targetNameLength = 0;
                                if (characterDatabase.cache->GetCharacterInfo(targetGuid, targetData))
                                {
                                    targetNameLength = static_cast<u32>(targetData.name.size());
                                }

                                Common::ByteBuffer textEmoteMessage;
                                textEmoteMessage.Write<u64>(playerConnection.characterGuid);
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

                                playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, textEmoteMessage, Common::Opcode::SMSG_TEXT_EMOTE));
                            }
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

								playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, spellFailed, Common::Opcode::SMSG_CAST_FAILED));
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
							buffer.AppendGuid(playerConnection.characterGuid);
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

							playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, buffer, Common::Opcode::MSG_MOVE_TELEPORT_ACK));
						}
						Common::ByteBuffer spellStart;
						spellStart.AppendGuid(playerConnection.characterGuid);
						spellStart.AppendGuid(playerConnection.characterGuid);
						spellStart.Write<u8>(0); // CastCount
						spellStart.Write<u32>(spellId);
						spellStart.Write<u32>(0x00000002);
						spellStart.Write<u32>(0);
						spellStart.Write<u32>(0);

						playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, spellStart, Common::Opcode::SMSG_SPELL_START));

						Common::ByteBuffer spellCast;
						spellCast.AppendGuid(playerConnection.characterGuid);
						spellCast.AppendGuid(playerConnection.characterGuid);
						spellCast.Write<u8>(0); // CastCount
						spellCast.Write<u32>(spellId);
						spellCast.Write<u32>(0x00000100);
						spellCast.Write<u32>(static_cast<u32>(singleton.lifeTimeInMS));

						spellCast.Write<u8>(1); // Affected Targets
						spellCast.Write<u64>(playerConnection.characterGuid); // Target GUID
						spellCast.Write<u8>(0); // Resisted Targets

						if (targetFlags == 0) // SELF
						{
							targetFlags = 0x02; // UNIT
						}
						spellCast.Write<u32>(targetFlags); // Target Flags
						spellCast.Write<u8>(0); // Target Flags

						playerPacketQueue.packetQueue->enqueue(PacketQueueData(playerConnection.socket, spellCast, Common::Opcode::SMSG_SPELL_GO));
						break;
					}
                    default:
                    {
                        ZoneScopedNC("Packet::Unhandled", tracy::Color::Orange2)
                        {
                            ZoneScopedNC("Packet::Unhandled::Log", tracy::Color::Orange2)
                                worldNodeHandler.PrintMessage("Account(%u), Character(%u) sent unhandled opcode %u", playerConnection.accountGuid, playerConnection.characterGuid, opcode);
                        }

                        // Mark all unhandled opcodes as handled to prevent the queue from trying to handle them every tick.
                        packet.handled = true;
                        break;
                    }
                    }
                }
            /* Cull Movement Data */

            if (playerConnection.packets.size() > 0)
            {
                ZoneScopedNC("Packet::PacketClear", tracy::Color::Orange2)
                    playerConnection.packets.erase(std::remove_if(playerConnection.packets.begin(), playerConnection.packets.end(), [](OpcodePacket& packet)
                {
                    return packet.handled;
                }), playerConnection.packets.end());
            }
        });
    }
}
