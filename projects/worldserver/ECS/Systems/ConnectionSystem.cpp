#include "ConnectionSystem.h"
#include <Networking/Opcode/Opcode.h>
#include <Utils/DebugHandler.h>
#include "../NovusEnums.h"

#include "../DatabaseCache/CharacterDatabaseCache.h"

#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/DeletePlayerQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"

namespace ConnectionSystem
{
    void Update(entt::registry &registry)
    {
		SingletonComponent& singleton = registry.get<SingletonComponent>(0);
        DeletePlayerQueueSingleton& deletePlayerQueue = registry.get<DeletePlayerQueueSingleton>(0);
        CharacterDatabaseCacheSingleton& characterDatabase = registry.get<CharacterDatabaseCacheSingleton>(0);
		NovusConnection& novusConnection = *singleton.connection;

        auto view = registry.view<ConnectionComponent, PlayerUpdateDataComponent, PositionComponent>();

        view.each([&singleton, &deletePlayerQueue, &characterDatabase, &novusConnection](const auto, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData, PositionComponent& clientPositionData)
        {
            NovusHeader packetHeader;
            packetHeader.command = NOVUS_FORWARDPACKET;
            packetHeader.account = clientConnection.accountGuid;

            for (OpcodePacket& packet : clientConnection.packets)
            {
                Common::Opcode opcode = Common::Opcode(packet.opcode);
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
                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_LOGOUT_REQUEST:
                    {
                        packetHeader.opcode = Common::Opcode::SMSG_LOGOUT_COMPLETE;
                        packetHeader.size = 0;

                        Common::ByteBuffer logoutRequest(0);
                        packetHeader.AddTo(logoutRequest);
                        novusConnection.SendPacket(logoutRequest);

                        ExpiredPlayerData expiredPlayerData;
                        expiredPlayerData.account = clientConnection.accountGuid;
                        expiredPlayerData.guid = clientConnection.characterGuid;
                        deletePlayerQueue.expiredEntityQueue->enqueue(expiredPlayerData);

                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_STANDSTATECHANGE:
                    {
                        u32 standState = 0;
                        packet.data.Read<u32>(standState);

                        /* Should Update Unit Field Here */
                        clientUpdateData.SetFieldValue<u8>(UNIT_FIELD_BYTES_1, u8(standState));

                        packetHeader.opcode = Common::Opcode::SMSG_STANDSTATE_UPDATE;
                        packetHeader.size = 1;

                        Common::ByteBuffer standStateChange(0);
                        packetHeader.AddTo(standStateChange);

                        standStateChange.Write<u8>(u8(standState));
                        novusConnection.SendPacket(standStateChange);
                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_SET_SELECTION:
                    {
                        u64 selectedGuid = 0;
                        packet.data.Read<u64>(selectedGuid);

                        clientUpdateData.SetGuidValue(UNIT_FIELD_TARGET, selectedGuid);
                        packet.handled = true;
                        break;
                    }
                    case Common::Opcode::CMSG_NAME_QUERY:
                    {
                        u64 guid;
                        packet.data.Read<u64>(guid);

                        const CharacterData characterData = characterDatabase.cache->GetCharacterDataReadOnly(guid);

                        NovusHeader novusHeader;
                        Common::ByteBuffer nameQueryForward;
                        Common::ByteBuffer nameQuery;

                        nameQuery.AppendGuid(guid);
                        nameQuery.Write<u8>(0); // Name Unknown (0 = false, 1 = true);
                        nameQuery.WriteString(characterData.name);
                        nameQuery.Write<u8>(0);
                        nameQuery.Write<u8>(characterData.race);
                        nameQuery.Write<u8>(characterData.gender);
                        nameQuery.Write<u8>(characterData.classId);
                        nameQuery.Write<u8>(0);

                        novusHeader.CreateForwardHeader(clientConnection.accountGuid, Common::Opcode::SMSG_NAME_QUERY_RESPONSE, nameQuery.GetActualSize());
                        novusHeader.AddTo(nameQueryForward);
                        nameQueryForward.Append(nameQuery);
                        novusConnection.SendPacket(nameQueryForward);
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
                        // Read GUID here as packed
                        uint64_t guid;
                        packet.data.ReadPackedGUID(guid);

                        uint32_t movementFlags;
                        uint16_t movementFlagsExtra;
                        uint32_t gameTime;
                        float position_x;
                        float position_y;
                        float position_z;
                        float position_orientation;
                        uint32_t fallTime;

                        // Store old movement info
                        clientPositionData.oldx = clientPositionData.x;
                        clientPositionData.oldy = clientPositionData.y;
                        clientPositionData.oldz = clientPositionData.z;
                        clientPositionData.oldorientation = clientPositionData.orientation;

                        packet.data.Read(&movementFlags, 4);
                        packet.data.Read(&movementFlagsExtra, 2);
                        packet.data.Read(&gameTime, 4);
                        packet.data.Read(&position_x, 4);
                        packet.data.Read(&position_y, 4);
                        packet.data.Read(&position_z, 4);
                        packet.data.Read(&position_orientation, 4);
                        packet.data.Read(&fallTime, 4);

                        clientPositionData.x = position_x;
                        clientPositionData.y = position_y;
                        clientPositionData.z = position_z;
                        clientPositionData.orientation = position_orientation;

                        uint32_t timeDelay = u32(singleton.lifeTimeInMS) - gameTime;
                        PositionUpdateData positionUpdateData;
                        positionUpdateData.opcode = opcode;
                        positionUpdateData.movementFlags = movementFlags;
                        positionUpdateData.movementFlagsExtra = movementFlagsExtra;
                        positionUpdateData.gameTime = gameTime + timeDelay;
                        positionUpdateData.x = clientPositionData.x;
                        positionUpdateData.y = clientPositionData.y;
                        positionUpdateData.z = clientPositionData.z;
                        positionUpdateData.orientation = clientPositionData.orientation;
                        positionUpdateData.fallTime = fallTime;
                        clientUpdateData.positionUpdateData.push_back(positionUpdateData);

                        packet.handled = true;
                        break;
                    }
                
                    case Common::Opcode::CMSG_MESSAGECHAT:
                    {
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
                                NC_LOG_MESSAGE("Account(%u), Character(%u) sent unhandled message type %u", clientConnection.accountGuid, clientConnection.characterGuid, msgType);
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
                    default:
                    {
                        // Mark all unhandled opcodes as handled to prevent the queue from trying to handle them every frame.
                        NC_LOG_MESSAGE("Account(%u), Character(%u) sent unhandled opcode %u", clientConnection.accountGuid, clientConnection.characterGuid, opcode);
                        packet.handled = true;
                        break;
                    }
                }
            }

            if (clientConnection.packets.size() > 0)
            {
                clientConnection.packets.erase(std::remove_if(clientConnection.packets.begin(), clientConnection.packets.end(), [](OpcodePacket& packet)
                {
                    return packet.handled;
                }), clientConnection.packets.end());
            }
        });
    }
}