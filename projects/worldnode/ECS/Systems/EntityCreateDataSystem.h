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

#include "../../NovusEnums.h"
#include "../../Utils/UpdateData.h"
#include "../../Utils/UpdateMask.h"

#include "../Components/ItemDataComponent.h"
#include "../Components/ItemInitializeComponent.h"
#include "../Components/ItemFieldDataComponent.h"
#include "../Components/UnitDataComponent.h"
#include "../Components/UnitInitializeComponent.h"
#include "../Components/UnitFieldDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerUpdatesQueueSingleton.h"

namespace EntityCreateDataSystem
{
    Common::ByteBuffer BuildItemCreateData(u64 itemGuid, u8 updateType, u16 updateFlags, u32 visibleFlags, ItemFieldDataComponent& itemFieldData, u16& opcode)
    {
        Common::ByteBuffer buffer(500);
        buffer.Write<u8>(updateType);
        buffer.AppendGuid(itemGuid);
        buffer.Write<u8>(1); // TYPEID_ITEM

        // BuildMovementUpdate(ByteBuffer* data, uint16 flags)
        buffer.Write<u16>(updateFlags);

        if (updateFlags & UPDATEFLAG_LOWGUID)
        {
            buffer.Write<u32>(1);
        }

        Common::ByteBuffer fieldBuffer;
        fieldBuffer.Resize(256);
        UpdateMask<64> updateMask(ITEM_END);

        u32* flags = ItemUpdateFieldFlags;
        u16 fieldNotifyFlags = UF_FLAG_DYNAMIC;

        for (u16 index = 0; index < ITEM_END; index++)
        {
            if (fieldNotifyFlags & flags[index] ||
                ((updateType == UPDATETYPE_VALUES ? itemFieldData.changesMask.IsSet(index) : itemFieldData.itemFields.ReadAt<i32>(index * 4)) &&
                (flags[index] & visibleFlags)))
            {
                updateMask.SetBit(index);
                fieldBuffer.Write(itemFieldData.itemFields.GetDataPointer() + index * 4, 4);
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
    Common::ByteBuffer BuildUnitCreateData(ObjectGuid unitGuid, u8 updateType, u16 updateFlags, u32 visibleFlags, u32 lifeTimeInMS, UnitFieldDataComponent& unitFieldData, u16& opcode)
    {
        Common::ByteBuffer buffer(500);
        buffer.Write<u8>(updateType);
        buffer.AppendGuid(unitGuid);
        
        // TypeId
        buffer.Write<u8>(3);

        // Build Movement Update
        buffer.Write<u16>(updateFlags);

        if (updateFlags & UPDATEFLAG_LIVING)
        {
            buffer.Write<u32>(0x00); // MovementFlags
            buffer.Write<u16>(0x00); // Extra MovementFlags
            buffer.Write<u32>(lifeTimeInMS); // Game Time

            // X, Y, Z, O
            buffer.Write<f32>(-4235.57f);
            buffer.Write<f32>(-12637.f);
            buffer.Write<f32>(31.8969f);
            buffer.Write<f32>(4.40294f);

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
        else
        {
            if (updateFlags & UPDATEFLAG_POSITION)
            {

            }
            else
            {
                if (updateFlags & UPDATEFLAG_STATIONARY_POSITION)
                {
                    if (unitGuid.GetEntry() == 25)
                    {
                        buffer.Write<f32>(-4235.57f);
                        buffer.Write<f32>(-12637.f);
                        buffer.Write<f32>(31.8969f);
                        buffer.Write<f32>(4.40294f);
                    }
                    else
                    {
                        buffer.Write<f32>(-4238.56f);
                        buffer.Write<f32>(-12636.36f);
                        buffer.Write<f32>(31.79f);
                        buffer.Write<f32>(4.40294f);
                    }
                }
            }
        }

        Common::ByteBuffer fieldBuffer;
        fieldBuffer.Resize(640);
        UpdateMask<160> updateMask(UNIT_END);

        u32* flags = UnitUpdateFieldFlags;
        u16 fieldNotifyFlags = UF_FLAG_DYNAMIC;

        for (u16 index = 0; index < UNIT_END; index++)
        {
            if (fieldNotifyFlags & flags[index] || ((flags[index] & visibleFlags) & UF_FLAG_SPECIAL_INFO)
                || ((updateType == 0 ? unitFieldData.changesMask.IsSet(index) : unitFieldData.unitFields.ReadAt<i32>(index * 4))
                && (flags[index] & visibleFlags)))
            {
                updateMask.SetBit(index);

                if (index == UNIT_NPC_FLAGS)
                {
                    u32 appendValue = unitFieldData.unitFields.ReadAt<u32>(UNIT_NPC_FLAGS * 4);

                    /*if (creature)
                        if (!target->CanSeeSpellClickOn(creature))
                            appendValue &= ~UNIT_NPC_FLAG_SPELLCLICK;*/

                    fieldBuffer.Write<u32>(appendValue);
                }
                else if (index == UNIT_FIELD_AURASTATE)
                {
                    // Check per caster aura states to not enable using a spell in client if specified aura is not by target
                    u32 auraState = unitFieldData.unitFields.ReadAt<u32>(UNIT_FIELD_AURASTATE * 4) & ~(((1 << (14 - 1)) | (1 << (16 - 1))));

                    fieldBuffer.Write<u32>(auraState);
                }
                // Seems to be fixed already??
                // FIXME: Some values at server stored in f32 format but must be sent to client in uint32 format
                else if (index >= UNIT_FIELD_BASEATTACKTIME && index <= UNIT_FIELD_RANGEDATTACKTIME)
                {
                    // convert from f32 to uint32 and send
                    fieldBuffer.Write<u32>(static_cast<u32>(unitFieldData.unitFields.ReadAt<i32>(index * 4)));
                }
                // there are some (said f32 in TC, but all these are ints)int values which may be negative or can't get negative due to other checks
                else if ((index >= UNIT_FIELD_NEGSTAT0 && index <= UNIT_FIELD_NEGSTAT4) ||
                    (index >= UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 6)) ||
                         (index >= UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 6)) ||
                         (index >= UNIT_FIELD_POSSTAT0 && index <= UNIT_FIELD_POSSTAT4))
                {
                    fieldBuffer.Write<u32>(static_cast<u32>(unitFieldData.unitFields.ReadAt<i32>(index * 4)));
                }
                // Gamemasters should be always able to select units - remove not selectable flag
                else if (index == UNIT_FIELD_FLAGS)
                {
                    u32 appendValue = unitFieldData.unitFields.ReadAt<u32>(UNIT_FIELD_FLAGS * 4);
                    //if (target->IsGameMaster())
                        //appendValue &= ~UNIT_FLAG_NOT_SELECTABLE;

                    fieldBuffer.Write<u32>(appendValue);
                }
                // use modelid_a if not gm, _h if gm for CREATURE_FLAG_EXTRA_TRIGGER creatures
                else if (index == UNIT_FIELD_DISPLAYID)
                {
                    u32 displayId = unitFieldData.unitFields.ReadAt<u32>(UNIT_FIELD_DISPLAYID * 4);
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
                    u32 dynamicFlags = unitFieldData.unitFields.ReadAt<u32>(UNIT_DYNAMIC_FLAGS * 4) & ~(0x4 | 0x08); // UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_TAPPED_BY_PLAYER

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
                    fieldBuffer.Write(unitFieldData.unitFields.GetDataPointer() + index * 4, 4);
                }
                else
                {
                    // send in current format (f32 as f32, uint32 as uint32)
                    fieldBuffer.Write(unitFieldData.unitFields.GetDataPointer() + index * 4, 4);
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

    void Update(entt::registry& registry)
    {
        SingletonComponent& singleton = registry.ctx<SingletonComponent>();
        PlayerUpdatesQueueSingleton& playerUpdatesQueue = registry.ctx<PlayerUpdatesQueueSingleton>();
        u32 lifeTimeInMS = static_cast<u32>(singleton.lifeTimeInMS);

        auto itemView = registry.view<ItemInitializeComponent, ItemFieldDataComponent>();
        if (!itemView.empty())
        {
            itemView.each([&registry](const auto, ItemInitializeComponent& itemInitializeData, ItemFieldDataComponent& itemFieldData)
            {
                /* Build Self Packet, must be sent immediately */
                u8 updateType = UPDATETYPE_CREATE_OBJECT;
                u16 selfUpdateFlag = UPDATEFLAG_LOWGUID;
                u32 selfVisibleFlags = (UF_FLAG_PUBLIC | UF_FLAG_OWNER);
                u16 buildOpcode = 0;

                Common::ByteBuffer selfItemUpdate = BuildItemCreateData(itemInitializeData.itemGuid, updateType, selfUpdateFlag, selfVisibleFlags, itemFieldData, buildOpcode);
                PlayerConnectionComponent playerConnection = registry.get<PlayerConnectionComponent>(itemInitializeData.characterEntityId);
                playerConnection.socket->SendPacket(selfItemUpdate, buildOpcode);

                Common::ByteBuffer itemPushResult;
                itemPushResult.Write<u64>(itemInitializeData.characterGuid);
                itemPushResult.Write<u32>(0); // Received:     0 = Looted,   1 = By NPC
                itemPushResult.Write<u32>(0); // Created:      0 = Received, 1 = Created
                itemPushResult.Write<u32>(1); // Show in chat: 0 = No Print, 1 = Print
                itemPushResult.Write<u8>(itemInitializeData.bagSlot); // BagSlot
                itemPushResult.Write<u32>(itemInitializeData.bagPosition); // BagPosition
                itemPushResult.Write<u32>(itemInitializeData.itemGuid.GetEntry());
                itemPushResult.Write<u32>(0);
                itemPushResult.Write<u32>(0);
                itemPushResult.Write<u32>(1);
                playerConnection.socket->SendPacket(itemPushResult, Common::Opcode::SMSG_ITEM_PUSH_RESULT);
            });
            // Remove ItemInitializeComponent from all entities (They've just been handled above)
            registry.reset<ItemInitializeComponent>();
        }

        auto unitView = registry.view<UnitInitializeComponent, UnitFieldDataComponent>();
        if (!unitView.empty())
        {
            unitView.each([&registry, &playerUpdatesQueue, lifeTimeInMS](const auto, UnitInitializeComponent& unitInitializeData, UnitFieldDataComponent& unitFieldData)
            {
                /* Build Self Packet for public */
                u8 updateType = UPDATETYPE_CREATE_OBJECT2;
                u16 publicUpdateFlag = (UPDATEFLAG_LIVING | UPDATEFLAG_STATIONARY_POSITION);
                u32 publicVisibleFlags = UF_FLAG_PUBLIC;
                u16 buildOpcode = 0;

                PlayerUpdatePacket playerUpdatePacket;
                playerUpdatePacket.characterGuid = unitInitializeData.unitGuid;
                playerUpdatePacket.updateType = updateType;
                playerUpdatePacket.data = BuildUnitCreateData(unitInitializeData.unitGuid, updateType, publicUpdateFlag, publicVisibleFlags, lifeTimeInMS, unitFieldData, buildOpcode);
                playerUpdatePacket.opcode = buildOpcode;
                playerUpdatesQueue.playerUpdatePacketQueue.push_back(playerUpdatePacket);
            });
        }
    }
}
