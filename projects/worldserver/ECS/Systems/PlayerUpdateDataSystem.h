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
#include <NovusTypes.h>
#include <entt.hpp>
#include <Networking/ByteBuffer.h>

#include "../NovusEnums.h"

#include "../Connections/NovusConnection.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerUpdatesQueueSingleton.h"

#include <tracy/Tracy.hpp>

namespace PlayerUpdateDataSystem
{
    Common::ByteBuffer BuildPlayerUpdateData(u64 playerGuid, u32 visibleFlags, PlayerUpdateDataComponent& playerUpdateData, u16& opcode)
    {
		ZoneScopedNC("BuildPlayerUpdateData", tracy::Color::Yellow2)

        Common::ByteBuffer buffer(500);
        buffer.Write<u8>(UPDATETYPE_VALUES);
        buffer.AppendGuid(playerGuid);

        Common::ByteBuffer fieldBuffer;
        fieldBuffer.Resize(5304);
        UpdateMask<1344> updateMask(PLAYER_END);

        u32* flags = UnitUpdateFieldFlags;
        u16 fieldNotifyFlags = UF_FLAG_DYNAMIC;

        for (u16 index = 0; index < PLAYER_END; ++index)
        {
            if (fieldNotifyFlags & flags[index] || ((flags[index] & visibleFlags) & UF_FLAG_SPECIAL_INFO) || 
               (playerUpdateData.changesMask.IsSet(index) && (flags[index] & visibleFlags)))
            {
                updateMask.SetBit(index);

                if (index == UNIT_NPC_FLAGS)
                {
                    u32 appendValue = playerUpdateData.playerFields.ReadAt<u32>(UNIT_NPC_FLAGS * 4);

                    /*if (creature)
                        if (!target->CanSeeSpellClickOn(creature))
                            appendValue &= ~UNIT_NPC_FLAG_SPELLCLICK;*/

                    fieldBuffer.Write<u32>(appendValue);
                }
                else if (index == UNIT_FIELD_AURASTATE)
                {
                    // Check per caster aura states to not enable using a spell in client if specified aura is not by target
                    u32 auraState = playerUpdateData.playerFields.ReadAt<u32>(UNIT_FIELD_AURASTATE * 4) &~(((1 << (14 - 1)) | (1 << (16 - 1))));

                    fieldBuffer.Write<u32>(auraState);
                }
                // Seems to be fixed already??
                // FIXME: Some values at server stored in f32 format but must be sent to client in uint32 format
                else if (index >= UNIT_FIELD_BASEATTACKTIME && index <= UNIT_FIELD_RANGEDATTACKTIME)
                {
                    // convert from f32 to uint32 and send
                    fieldBuffer.Write<u32>(u32(playerUpdateData.playerFields.ReadAt<i32>(index * 4)));
                }
                // there are some (said f32 in TC, but all these are ints)int values which may be negative or can't get negative due to other checks
                else if ((index >= UNIT_FIELD_NEGSTAT0 && index <= UNIT_FIELD_NEGSTAT4) ||
                    (index >= UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 6)) ||
                    (index >= UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 6)) ||
                    (index >= UNIT_FIELD_POSSTAT0 && index <= UNIT_FIELD_POSSTAT4))
                {
                    fieldBuffer.Write<u32>(u32(playerUpdateData.playerFields.ReadAt<i32>(index * 4)));
                }
                // Gamemasters should be always able to select units - remove not selectable flag
                else if (index == UNIT_FIELD_FLAGS)
                {
                    u32 appendValue = playerUpdateData.playerFields.ReadAt<u32>(UNIT_FIELD_FLAGS * 4);
                    //if (target->IsGameMaster())
                        //appendValue &= ~UNIT_FLAG_NOT_SELECTABLE;

                    fieldBuffer.Write<u32>(appendValue);
                }
                // use modelid_a if not gm, _h if gm for CREATURE_FLAG_EXTRA_TRIGGER creatures
                else if (index == UNIT_FIELD_DISPLAYID)
                {
                    u32 displayId = playerUpdateData.playerFields.ReadAt<u32>(UNIT_FIELD_DISPLAYID * 4);
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
                    u32 dynamicFlags = playerUpdateData.playerFields.ReadAt<u32>(UNIT_DYNAMIC_FLAGS * 4) & ~(0x4 | 0x08); // UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_TAPPED_BY_PLAYER

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
                    fieldBuffer.Write(playerUpdateData.playerFields.GetDataPointer() + index * 4, 4);
                }
                else
                {
                    // send in current format (f32 as f32, uint32 as uint32)
                    fieldBuffer.Write(playerUpdateData.playerFields.GetDataPointer() + index * 4, 4);
                }
            }
        }

        buffer.Write<u8>(updateMask.GetBlocks());
        updateMask.AddTo(buffer);
        buffer.Append(fieldBuffer);

        UpdateData updateData;
        updateData.AddBlock(buffer);

        Common::ByteBuffer tempBuffer;
        updateData.Build(tempBuffer, opcode);

        return tempBuffer;
    }

    void Update(entt::registry &registry)
    {
		SingletonComponent& singleton = registry.get<SingletonComponent>(0);
        PlayerUpdatesQueueSingleton& playerUpdatesQueue = registry.get<PlayerUpdatesQueueSingleton>(0);
		NovusConnection& novusConnection = *singleton.connection;

        auto view = registry.view<ConnectionComponent, PlayerUpdateDataComponent, PositionComponent>();
        view.each([&novusConnection, &playerUpdatesQueue](const auto, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData, PositionComponent& clientPositionData)
        {
			ZoneScopedNC("Connection", tracy::Color::Yellow2)
            /* Only Build Packet if any fields were changed */
            if (clientUpdateData.changesMask.Any())
            {
				ZoneScopedNC("Build Packet", tracy::Color::Yellow2)
                /* Build Self Packet, must be sent immediately */
                u32 selfVisibleFlags = (UF_FLAG_PUBLIC | UF_FLAG_PRIVATE);
                u16 buildOpcode = 0;

                NovusHeader novusHeader;
                Common::ByteBuffer selfPlayerUpdate = BuildPlayerUpdateData(clientConnection.characterGuid, selfVisibleFlags, clientUpdateData, buildOpcode);
                novusHeader.CreateForwardHeader(clientConnection.accountGuid, buildOpcode, selfPlayerUpdate.GetActualSize());
                novusConnection.SendPacket(novusHeader.BuildHeaderPacket(selfPlayerUpdate));

                /* Build Self Packet for public */
                u32 publicVisibleFlags = UF_FLAG_PUBLIC;
                PlayerUpdatePacket playerUpdatePacket;
                playerUpdatePacket.characterGuid = clientConnection.characterGuid;
                playerUpdatePacket.updateType = UPDATETYPE_VALUES;
                playerUpdatePacket.data = BuildPlayerUpdateData(clientConnection.characterGuid, publicVisibleFlags, clientUpdateData, buildOpcode);
                playerUpdatePacket.opcode = buildOpcode;
                playerUpdatesQueue.playerUpdatePacketQueue.push_back(playerUpdatePacket);

                // Clear Updates
                clientUpdateData.changesMask.Reset();
            }

            if(clientUpdateData.positionUpdateData.size() > 0)
            {
				ZoneScopedNC("PositionUpdate", tracy::Color::Yellow2)
                for (PositionUpdateData positionData : clientUpdateData.positionUpdateData)
                {
                    MovementPacket movementPacket;
                    movementPacket.opcode = positionData.opcode;
                    movementPacket.characterGuid = clientConnection.characterGuid;

                    movementPacket.data.AppendGuid(movementPacket.characterGuid);
                    movementPacket.data.Write<u32>(positionData.movementFlags);
                    movementPacket.data.Write<u16>(positionData.movementFlagsExtra);
                    movementPacket.data.Write<u32>(positionData.gameTime);
                    movementPacket.data.Write<f32>(positionData.x);
                    movementPacket.data.Write<f32>(positionData.y);
                    movementPacket.data.Write<f32>(positionData.z);
                    movementPacket.data.Write<f32>(positionData.orientation);
                    movementPacket.data.Write<u32>(positionData.fallTime);

                    playerUpdatesQueue.playerMovementPacketQueue.push_back(movementPacket);
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

                    chatPacket.data.Write<u8>(chatData.chatType);
                    chatPacket.data.Write<i32>(chatData.language);
                    chatPacket.data.Write<u64>(chatData.sender);
                    chatPacket.data.Write<u32>(0); // Chat Flag (??)

                    // This is based on chatType
                    chatPacket.data.Write<u64>(0); // Receiver (0) for none

                    chatPacket.data.Write<u32>(u32(chatData.message.length()) + 1);
                    chatPacket.data.WriteString(chatData.message);
                    chatPacket.data.Write<u8>(0); // Chat Tag

                    playerUpdatesQueue.playerChatPacketQueue.push_back(chatPacket);
                }
                // Clear Chat Updates
                clientUpdateData.chatUpdateData.clear();
            }
        });
    }
}