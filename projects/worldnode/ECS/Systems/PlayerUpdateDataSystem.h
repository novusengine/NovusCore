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
#include <algorithm>
#include <NovusTypes.h>
#include <entt.hpp>
#include <Networking/DataStore.h>

#include "../../NovusEnums.h"

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerFieldDataComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerUpdatesQueueSingleton.h"

#include <tracy/Tracy.hpp>

namespace PlayerUpdateDataSystem
{
    std::shared_ptr<DataStore> BuildPlayerUpdateData(u64 playerGuid, u32 visibleFlags, PlayerFieldDataComponent& playerFieldData, u16& opcode)
    {
        ZoneScopedNC("BuildplayerFieldData", tracy::Color::Yellow2)

            std::shared_ptr<DataStore> buffer = DataStore::Borrow<8192>();
        /*
            Buffer Structure

            Start Core Structure
            - UpdateType : u8 (Specifies if the Data we are sending is a value update or a new object creation)
            - CharacterGuid : u64 (A compact GUID of the entity we are sending this update for)

            Movement Structure
            - UpdateFlags : u16 (Specifies which components should be updated so the client can properly read the data)
            - MovementFlags : u32 (Specifies which movementflags the entity has)
            - ExtraMovementFlags : u16 (Specifies which movementflags the entity has)
            - GameTime : u32 (Specifies the server's time in miliseconds)
            - Position : Vec4 (Specifies the entity's location + orientation)
            - FallTime : u32 (Specifies how long an entity have been falling)
            - MovementSpeeds : 9xf32 (Specifies how fast the entity can move)

            End Core Structure
            - BlockCount : u8 (Specifies the amount of update blocks in this update packet)
            - BlockMask : BlockCount x u32 (Specifies the update mask for the given block)
            - FieldBuffer : DataStore (Specifies the updated EntityFields)
        */

        buffer->PutU8(UPDATETYPE_VALUES);
        buffer->PutGuid(playerGuid);

        // Offset end of buffer (PLAYER_END*4)
        std::shared_ptr<DataStore> fieldbuffer = DataStore::Borrow<5304>();
        UpdateMask<1344> updateMask(PLAYER_END);

        u32* flags = UnitUpdateFieldFlags;

        {
            ZoneScopedNC("BuildplayerFieldData::Loop", tracy::Color::Yellow2)
                for (u16 index = 0; index < PLAYER_END; index++)
                {
                    if (UF_FLAG_DYNAMIC & flags[index] || ((flags[index] & visibleFlags) & UF_FLAG_SPECIAL_INFO) ||
                        (playerFieldData.changesMask.IsSet(index) && (flags[index] & visibleFlags)))
                    {
                        updateMask.SetBit(index);

                        if (index == UNIT_NPC_FLAGS)
                        {
                            u32 appendValue = 0;
                            playerFieldData.playerFields->Get<u32>(appendValue, UNIT_NPC_FLAGS * 4);

                            /*if (creature)
                                if (!target->CanSeeSpellClickOn(creature))
                                    appendValue &= ~UNIT_NPC_FLAG_SPELLCLICK;*/

                            fieldbuffer->PutU32(appendValue);
                        }
                        else if (index == UNIT_FIELD_AURASTATE)
                        {
                            // Check per caster aura states to not enable using a spell in client if specified aura is not by target
                            u32 auraState = 0;
                            playerFieldData.playerFields->Get<u32>(auraState, UNIT_FIELD_AURASTATE * 4);
                            auraState &= ~(((1 << (14 - 1)) | (1 << (16 - 1))));

                            fieldbuffer->PutU32(auraState);
                        }
                        // Seems to be fixed already??
                        // FIXME: Some values at server stored in f32 format but must be sent to client in uint32 format
                        else if (index >= UNIT_FIELD_BASEATTACKTIME && index <= UNIT_FIELD_RANGEDATTACKTIME)
                        {
                            // convert from f32 to uint32 and send
                            i32 fieldValue = 0;
                            playerFieldData.playerFields->Get<i32>(fieldValue, index * 4);
                            fieldbuffer->PutU32(static_cast<u32>(fieldValue));
                        }
                        // there are some (said f32 in TC, but all these are ints)int values which may be negative or can't get negative due to other checks
                        else if ((index >= UNIT_FIELD_NEGSTAT0 && index <= UNIT_FIELD_NEGSTAT4) ||
                            (index >= UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 6)) ||
                            (index >= UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 6)) ||
                            (index >= UNIT_FIELD_POSSTAT0 && index <= UNIT_FIELD_POSSTAT4))
                        {
                            i32 fieldValue = 0;
                            playerFieldData.playerFields->Get<i32>(fieldValue, index * 4);
                            fieldbuffer->PutU32(static_cast<u32>(fieldValue));
                        }
                        // Gamemasters should be always able to select units - remove not selectable flag
                        else if (index == UNIT_FIELD_FLAGS)
                        {
                            u32 appendValue = 0;
                            playerFieldData.playerFields->Get<u32>(appendValue, UNIT_FIELD_FLAGS * 4);
                            //if (target->IsGameMaster())
                                //appendValue &= ~UNIT_FLAG_NOT_SELECTABLE;

                            fieldbuffer->PutU32(appendValue);
                        }
                        // use modelid_a if not gm, _h if gm for CREATURE_FLAG_EXTRA_TRIGGER creatures
                        else if (index == UNIT_FIELD_DISPLAYID)
                        {
                            u32 displayId = 0;
                            playerFieldData.playerFields->Get<u32>(displayId, UNIT_FIELD_DISPLAYID * 4);
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

                            fieldbuffer->PutU32(displayId);
                        }
                        // hide lootable animation for unallowed players
                        else if (index == UNIT_DYNAMIC_FLAGS)
                        {
                            u32 dynamicFlags = 0;
                            playerFieldData.playerFields->Get<u32>(dynamicFlags, UNIT_DYNAMIC_FLAGS * 4); // UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_TAPPED_BY_PLAYER
                            dynamicFlags &= ~(0x4 | 0x08);

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

                            fieldbuffer->PutU32(dynamicFlags);
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
                            fieldbuffer->PutBytes(playerFieldData.playerFields->GetInternalData() + index * 4, 4);
                        }
                        else
                        {
                            // send in current format (f32 as f32, uint32 as uint32)
                            fieldbuffer->PutBytes(playerFieldData.playerFields->GetInternalData() + index * 4, 4);
                        }
                    }
                }
        }

        {
            ZoneScopedNC("BuildplayerFieldData::GetBlocks", tracy::Color::Yellow2)
                buffer->PutU8(updateMask.GetBlocks());
            updateMask.AddTo(buffer.get());
            buffer->PutBytes(fieldbuffer->GetInternalData(), fieldbuffer->WrittenData);
        }

        UpdateData updateData;
        {
            ZoneScopedNC("BuildplayerFieldData::AddBlock", tracy::Color::Yellow2)
                updateData.AddBlock(buffer.get());
        }

        std::shared_ptr<DataStore> tempBuffer = DataStore::Borrow<8192>();
        {
            ZoneScopedNC("BuildplayerFieldData::Build", tracy::Color::Yellow2)
                updateData.Build(tempBuffer.get(), opcode);
        }

        return tempBuffer;
    }

    void Update(entt::registry& registry)
    {
        PlayerUpdatesQueueSingleton& playerUpdatesQueue = registry.ctx<PlayerUpdatesQueueSingleton>();
        MapSingleton& mapSingleton = registry.ctx<MapSingleton>();

        auto view = registry.view<PlayerConnectionComponent, PlayerFieldDataComponent, PlayerUpdateDataComponent, PlayerPositionComponent>();
        view.each([&playerUpdatesQueue, &mapSingleton](const auto, PlayerConnectionComponent& clientConnection, PlayerFieldDataComponent& clientFieldData, PlayerUpdateDataComponent& clientUpdateData, PlayerPositionComponent& clientPositionData)
            {
                ZoneScopedNC("Connection", tracy::Color::Yellow2)
                    /* Only Build Packet if any fields were changed */
                    if (clientFieldData.changesMask.Any())
                    {
                        ZoneScopedNC("Build Packet", tracy::Color::Yellow2)
                            /* Build Self Packet, must be sent immediately */
                            u32 selfVisibleFlags = (UF_FLAG_PUBLIC | UF_FLAG_PRIVATE);
                        u16 buildOpcode = 0;

                        // Currently we have not observed any issues with sending private field flags to any other client but themselves, this offers a good speed increase but if we see issues in the future we should recheck this.
                        /*NovusHeader novusHeader;
                        Common::ByteBuffer selfPlayerUpdate = BuildplayerFieldData(clientConnection.characterGuid, selfVisibleFlags, clientFieldData, buildOpcode);
                        novusHeader.CreateForwardHeader(clientConnection.accountGuid, buildOpcode, selfPlayerUpdate.GetActualSize());
                        novusConnection.SendPacket(novusHeader.BuildHeaderPacket(selfPlayerUpdate));*/

                        /* Build Self Packet for public */
                        //u32 publicVisibleFlags = UF_FLAG_PUBLIC;
                        PlayerUpdatePacket playerUpdatePacket;
                        playerUpdatePacket.characterGuid = clientConnection.characterGuid;
                        playerUpdatePacket.updateType = UPDATETYPE_VALUES;
                        playerUpdatePacket.data = BuildPlayerUpdateData(clientConnection.characterGuid, selfVisibleFlags, clientFieldData, buildOpcode);
                        playerUpdatePacket.opcode = buildOpcode;
                        playerUpdatesQueue.playerUpdatePacketQueue.push_back(playerUpdatePacket);

                        // Clear Updates
                        clientFieldData.changesMask.Reset();
                    }

                if (clientUpdateData.positionUpdateData.size())
                {
                    ZoneScopedNC("PositionUpdate", tracy::Color::Yellow2)
                        for (PositionUpdateData positionData : clientUpdateData.positionUpdateData)
                        {
                            MovementPacket movementPacket;
                            movementPacket.opcode = positionData.opcode;
                            movementPacket.characterGuid = clientConnection.characterGuid;

                            movementPacket.data = DataStore::Borrow<38>();
                            movementPacket.data->PutGuid(movementPacket.characterGuid);
                            movementPacket.data->PutU32(positionData.movementFlags);
                            movementPacket.data->PutU16(positionData.movementFlagsExtra);
                            movementPacket.data->PutU32(positionData.gameTime);
                            movementPacket.data->Put<Vector3>(positionData.position);
                            movementPacket.data->PutF32(positionData.orientation);
                            movementPacket.data->PutU32(positionData.fallTime);

                            playerUpdatesQueue.playerMovementPacketQueue.push_back(movementPacket);
                        }

                    // Figure out what ADT the player is in
                    Vector2 position = Vector2(clientPositionData.position.x, clientPositionData.position.y);
                    u32 mapId = clientPositionData.mapId;

                    u16 adtId;
                    if (mapSingleton.maps[mapId].GetAdtIdFromWorldPosition(position, adtId))
                    {
                        // If the ADT doesnt match the previous known ADT we need to update it.
                        if (adtId != clientPositionData.adtId)
                        {
                            u32 guid = clientConnection.entityGuid;

                            std::vector<u32>& playerList = mapSingleton.maps[mapId].playersInAdts[clientPositionData.adtId];
                            if (clientPositionData.adtId != INVALID_ADT)
                            {
                                auto iterator = std::find(playerList.begin(), playerList.end(), guid);
                                assert(iterator != playerList.end());
                                playerList.erase(iterator);

                                clientConnection.SendChatNotification("[DEBUG] Old ADT: %u has %u players", clientPositionData.adtId, playerList.size());
                            }

                            clientPositionData.adtId = adtId;

                            mapSingleton.maps[mapId].playersInAdts[adtId].push_back(guid);
                            clientConnection.SendChatNotification("[DEBUG] New ADT: %u has %u players", adtId, mapSingleton.maps[mapId].playersInAdts[adtId].size());
                        }
                    }

                    // Clear Position Updates
                    clientUpdateData.positionUpdateData.clear();
                }

                if (clientUpdateData.chatUpdateData.size() > 0)
                {
                    ZoneScopedNC("ChatUpdate", tracy::Color::Yellow2)
                        for (ChatUpdateData chatData : clientUpdateData.chatUpdateData)
                        {
                            ChatPacket chatPacket;
                            chatPacket.data = DataStore::Borrow<286>();

                            chatPacket.data->PutU8(chatData.chatType);
                            chatPacket.data->PutI32(chatData.language);
                            chatPacket.data->PutU64(chatData.sender);
                            chatPacket.data->PutU32(0); // Chat Flag (??)

                            // This is based on chatType
                            chatPacket.data->PutU64(0); // Receiver (0) for none

                            chatPacket.data->PutU32(static_cast<u32>(chatData.message.length()) + 1);
                            chatPacket.data->PutString(chatData.message);
                            chatPacket.data->PutU8(0); // Chat Tag

                            playerUpdatesQueue.playerChatPacketQueue.push_back(chatPacket);
                        }

                    // Clear Chat Updates
                    clientUpdateData.chatUpdateData.clear();
                }
            });
    }
}
