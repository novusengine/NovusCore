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

namespace EntityCreateDataSystem
{
std::shared_ptr<ByteBuffer> BuildItemCreateData(u64 itemGuid, u8 updateType, u16 updateFlags, u32 visibleFlags, ItemFieldDataComponent& itemFieldData, u16& opcode)
{
    std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<4096>();
    buffer->PutU8(updateType);
    buffer->PutGuid(itemGuid);
    buffer->PutU8(1); // TYPEID_ITEM

    // BuildMovementUpdate(ByteBuffer* data, uint16 flags)
    buffer->PutU16(updateFlags);

    if (updateFlags & UPDATEFLAG_LOWGUID)
    {
        buffer->PutU32(1);
    }

    std::shared_ptr<ByteBuffer> fieldbuffer = ByteBuffer::Borrow<256>();
    UpdateMask<64> updateMask(ITEM_END);

    u32* flags = ItemUpdateFieldFlags;
    i32 fieldDataValue = 0;
    u16 fieldNotifyFlags = UPDATEFIELD_FLAG_ALL;

    for (u16 index = 0; index < ITEM_END; index++)
    {
        if (updateType != UPDATETYPE_VALUES)
        {
            itemFieldData.itemFields->Get<i32>(fieldDataValue, index * 4);
        }

        if (fieldNotifyFlags & flags[index] ||
            ((updateType == UPDATETYPE_VALUES ? itemFieldData.changesMask.IsSet(index) : fieldDataValue) &&
             (flags[index] & visibleFlags)))
        {
            updateMask.SetBit(index);
            fieldbuffer->PutBytes(itemFieldData.itemFields->GetInternalData() + index * 4, 4);
        }
    }

    buffer->PutU8(updateMask.GetBlocks());
    updateMask.AddTo(buffer.get());
    buffer->PutBytes(fieldbuffer->GetInternalData(), fieldbuffer->WrittenData);

    UpdateData updateData;
    updateData.AddBlock(buffer.get());

    std::shared_ptr<ByteBuffer> tempBuffer = ByteBuffer::Borrow<8192>();
    updateData.Build(tempBuffer.get(), opcode);

    return tempBuffer;
}
std::shared_ptr<ByteBuffer> BuildUnitCreateData(ObjectGuid unitGuid, u8 updateType, u16 updateFlags, u32 visibleFlags, u32 lifeTimeInMS, UnitFieldDataComponent& unitFieldData, u16& opcode)
{
    std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<4096>();
    buffer->PutU8(updateType);
    buffer->PutGuid(unitGuid);

    // TypeId
    buffer->PutU8(3);

    // Build Movement Update
    buffer->PutU16(updateFlags);

    if (updateFlags & UPDATEFLAG_LIVING)
    {
        buffer->PutU32(0x00);         // MovementFlags
        buffer->PutU16(0x00);         // Extra MovementFlags
        buffer->PutU32(lifeTimeInMS); // Game Time

        // X, Y, Z, O
        buffer->PutF32(-4235.57f);
        buffer->PutF32(-12637.f);
        buffer->PutF32(31.8969f);
        buffer->PutF32(4.40294f);

        // FallTime
        buffer->PutU32(0);

        // Movement Speeds
        buffer->PutF32(2.5f);      // MOVE_WALK
        buffer->PutF32(7.0f);      // MOVE_RUN
        buffer->PutF32(4.5f);      // MOVE_RUN_BACK
        buffer->PutF32(4.722222f); // MOVE_SWIM
        buffer->PutF32(2.5f);      // MOVE_SWIM_BACK
        buffer->PutF32(7.0f);      // MOVE_FLIGHT
        buffer->PutF32(4.5f);      // MOVE_FLIGHT_BACK
        buffer->PutF32(3.141593f); // MOVE_TURN_RATE
        buffer->PutF32(3.141593f); // MOVE_PITCH_RATE
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
                    buffer->PutF32(-4235.57f);
                    buffer->PutF32(-12637.f);
                    buffer->PutF32(31.8969f);
                    buffer->PutF32(4.40294f);
                }
                else
                {
                    buffer->PutF32(-4238.56f);
                    buffer->PutF32(-12636.36f);
                    buffer->PutF32(31.79f);
                    buffer->PutF32(4.40294f);
                }
            }
        }
    }

    std::shared_ptr<ByteBuffer> fieldbuffer = ByteBuffer::Borrow<640>();
    UpdateMask<160> updateMask(UNIT_END);

    u32* flags = UnitUpdateFieldFlags;
    i32 fieldDataValue = 0;
    u16 fieldNotifyFlags = UPDATEFIELD_FLAG_ALL;

    for (u16 index = 0; index < UNIT_END; index++)
    {
        if (updateType != UPDATETYPE_VALUES)
        {
            unitFieldData.unitFields->Get<i32>(fieldDataValue, index * 4);
        }

        if (fieldNotifyFlags & flags[index] || ((flags[index] & visibleFlags) & UPDATEFIELD_FLAG_SPECIAL_INFO) || ((updateType == UPDATETYPE_VALUES ? unitFieldData.changesMask.IsSet(index) : fieldDataValue) && (flags[index] & visibleFlags)))
        {
            updateMask.SetBit(index);

            if (index == UNIT_NPC_FLAGS)
            {
                u32 appendValue = 0;
                unitFieldData.unitFields->Get<u32>(appendValue, UNIT_NPC_FLAGS * 4);

                /*if (creature)
                        if (!target->CanSeeSpellClickOn(creature))
                            appendValue &= ~UNIT_NPC_FLAG_SPELLCLICK;*/

                fieldbuffer->PutU32(appendValue);
            }
            else if (index == UNIT_FIELD_AURASTATE)
            {
                // Check per caster aura states to not enable using a spell in client if specified aura is not by target
                u32 auraState = 0;
                unitFieldData.unitFields->Get<u32>(auraState, UNIT_FIELD_AURASTATE * 4);
                auraState &= ~(((1 << (14 - 1)) | (1 << (16 - 1))));

                fieldbuffer->PutU32(auraState);
            }
            // Seems to be fixed already??
            // FIXME: Some values at server stored in f32 format but must be sent to client in uint32 format
            else if (index >= UNIT_FIELD_BASEATTACKTIME && index <= UNIT_FIELD_RANGEDATTACKTIME)
            {
                // convert from f32 to uint32 and send
                i32 fieldValue = 0;
                unitFieldData.unitFields->Get<i32>(fieldValue, index * 4);
                fieldbuffer->PutU32(static_cast<u32>(fieldValue));
            }
            // there are some (said f32 in TC, but all these are ints)int values which may be negative or can't get negative due to other checks
            else if ((index >= UNIT_FIELD_NEGSTAT0 && index <= UNIT_FIELD_NEGSTAT4) ||
                     (index >= UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + 6)) ||
                     (index >= UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE && index <= (UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + 6)) ||
                     (index >= UNIT_FIELD_POSSTAT0 && index <= UNIT_FIELD_POSSTAT4))
            {
                i32 fieldValue = 0;
                unitFieldData.unitFields->Get<i32>(fieldValue, index * 4);
                fieldbuffer->PutU32(static_cast<u32>(fieldValue));
            }
            // Gamemasters should be always able to select units - remove not selectable flag
            else if (index == UNIT_FIELD_FLAGS)
            {
                u32 appendValue = 0;
                unitFieldData.unitFields->Get<u32>(appendValue, UNIT_FIELD_FLAGS * 4);
                //if (target->IsGameMaster())
                //appendValue &= ~UNIT_FLAG_NOT_SELECTABLE;

                fieldbuffer->PutU32(appendValue);
            }
            // use modelid_a if not gm, _h if gm for CREATURE_FLAG_EXTRA_TRIGGER creatures
            else if (index == UNIT_FIELD_DISPLAYID)
            {
                u32 displayId = 0;
                unitFieldData.unitFields->Get<u32>(displayId, UNIT_FIELD_DISPLAYID * 4);
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
                unitFieldData.unitFields->Get<u32>(dynamicFlags, UNIT_DYNAMIC_FLAGS * 4); // UNIT_DYNFLAG_TAPPED | UNIT_DYNFLAG_TAPPED_BY_PLAYER
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
                fieldbuffer->PutBytes(unitFieldData.unitFields->GetInternalData() + index * 4, 4);
            }
            else
            {
                // send in current format (f32 as f32, uint32 as uint32)
                fieldbuffer->PutBytes(unitFieldData.unitFields->GetInternalData() + index * 4, 4);
            }
        }
    }

    buffer->PutU8(updateMask.GetBlocks());
    updateMask.AddTo(buffer.get());
    buffer->PutBytes(fieldbuffer->GetInternalData(), fieldbuffer->WrittenData);

    UpdateData updateData;
    updateData.AddBlock(buffer.get());

    std::shared_ptr<ByteBuffer> tempBuffer = ByteBuffer::Borrow<8192>();
    updateData.Build(tempBuffer.get(), opcode);

    return tempBuffer;
}

void Update(entt::registry& registry)
{
    SingletonComponent& singleton = registry.ctx<SingletonComponent>();
    u32 lifeTimeInMS = static_cast<u32>(singleton.lifeTimeInMS);

    auto itemView = registry.view<ItemInitializeComponent, ItemFieldDataComponent>();
    if (!itemView.empty())
    {
        itemView.each([&registry](const auto, ItemInitializeComponent& itemInitializeData, ItemFieldDataComponent& itemFieldData) {
            /* Build Self Packet, must be sent immediately */
            u8 updateType = UPDATETYPE_CREATE_OBJECT;
            u16 selfUpdateFlag = UPDATEFLAG_LOWGUID;
            u32 selfVisibleFlags = (UPDATEFIELD_FLAG_PUBLIC | UPDATEFIELD_FLAG_ITEM_OWNER);
            u16 buildOpcode = 0;

            std::shared_ptr<ByteBuffer> selfItemUpdate = BuildItemCreateData(itemInitializeData.itemGuid, updateType, selfUpdateFlag, selfVisibleFlags, itemFieldData, buildOpcode);
            PlayerConnectionComponent playerConnection = registry.get<PlayerConnectionComponent>(itemInitializeData.characterEntityId);
            playerConnection.socket->SendPacket(selfItemUpdate.get(), buildOpcode);

            std::shared_ptr<ByteBuffer> itemPushResult = ByteBuffer::Borrow<41>();
            itemPushResult->PutU64(itemInitializeData.characterGuid);
            itemPushResult->PutU32(0);                              // Received:     0 = Looted,   1 = By NPC
            itemPushResult->PutU32(0);                              // Created:      0 = Received, 1 = Created
            itemPushResult->PutU32(1);                              // Show in chat: 0 = No Print, 1 = Print
            itemPushResult->PutU8(itemInitializeData.bagSlot);      // BagSlot
            itemPushResult->PutU32(itemInitializeData.bagPosition); // BagPosition
            itemPushResult->PutU32(itemInitializeData.itemGuid.GetEntry());
            itemPushResult->PutU32(0);
            itemPushResult->PutU32(0);
            itemPushResult->PutU32(1);
            playerConnection.socket->SendPacket(itemPushResult.get(), Opcode::SMSG_ITEM_PUSH_RESULT);
        });
        // Remove ItemInitializeComponent from all entities (They've just been handled above)
        registry.reset<ItemInitializeComponent>();
    }

    auto unitView = registry.view<UnitInitializeComponent, UnitFieldDataComponent>();
    if (!unitView.empty())
    {
        unitView.each([lifeTimeInMS](const auto, UnitInitializeComponent& unitInitializeData, UnitFieldDataComponent& unitFieldData) {
            /* Build Self Packet for public */
            /*u8 updateType = UPDATETYPE_CREATE_OBJECT2;
            u16 publicUpdateFlag = (UPDATEFLAG_LIVING | UPDATEFLAG_STATIONARY_POSITION);
            u32 publicVisibleFlags = UPDATEFIELD_FLAG_PUBLIC;
            u16 buildOpcode = 0;

            PlayerUpdatePacket playerUpdatePacket;
            playerUpdatePacket.characterGuid = unitInitializeData.unitGuid;
            playerUpdatePacket.updateType = updateType;
            playerUpdatePacket.data = BuildUnitCreateData(unitInitializeData.unitGuid, updateType, publicUpdateFlag, publicVisibleFlags, lifeTimeInMS, unitFieldData, buildOpcode);
            playerUpdatePacket.opcode = buildOpcode;
            playerUpdatesQueue.playerUpdatePacketQueue.push_back(playerUpdatePacket);*/
        });

        registry.reset<UnitInitializeComponent>();
    }
}
} // namespace EntityCreateDataSystem
