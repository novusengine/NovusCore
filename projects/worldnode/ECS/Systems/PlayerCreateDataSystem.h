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
#include <Networking/DataStore.h>

#include "../../NovusEnums.h"
#include "../../Utils/UpdateData.h"
#include "../../Utils/UpdateMask.h"

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerPositionComponent.h"
#include "../Components/PlayerInitializeComponent.h"
#include "../Components/PlayerFieldDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerUpdatesQueueSingleton.h"
#include "../Components/Singletons/EntityCreateQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"

#include "../../Scripting/PlayerFunctions.h"

namespace PlayerCreateDataSystem
{
    std::shared_ptr<DataStore> BuildPlayerCreateData(u64 characterGuid, u8 updateType, u16 updateFlags, u32 visibleFlags, u32 lifeTimeInMS, PlayerFieldDataComponent& playerFieldData, PlayerPositionComponent position, u16& opcode)
    {
        std::shared_ptr<DataStore> buffer = DataStore::Borrow<5304>();
        buffer->PutU8(updateType);
        buffer->PutGuid(characterGuid);

        buffer->PutU8(4); // TYPEID_PLAYER

        // BuildMovementUpdate(ByteBuffer* data, uint16 flags)
        buffer->PutU16(updateFlags);

        if (updateFlags & UPDATEFLAG_LIVING)
        {
            buffer->PutU32(0x00); // MovementFlags
            buffer->PutU16(0x00); // Extra MovementFlags
            buffer->PutU32(lifeTimeInMS); // Game Time
            // TaggedPosition<Position::XYZO>(pos);
            buffer->PutF32(position.x);
            buffer->PutF32(position.y);
            buffer->PutF32(position.z);
            buffer->PutF32(position.orientation);

            // FallTime
            buffer->PutU32(0);

            // Movement Speeds
            buffer->PutF32(2.5f); // MOVE_WALK
            buffer->PutF32(7.0f); // MOVE_RUN
            buffer->PutF32(4.5f); // MOVE_RUN_BACK
            buffer->PutF32(4.722222f); // MOVE_SWIM
            buffer->PutF32(2.5f); // MOVE_SWIM_BACK
            buffer->PutF32(7.0f); // MOVE_FLIGHT
            buffer->PutF32(4.5f); // MOVE_FLIGHT_BACK
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
                    buffer->PutF32(position.x);
                    buffer->PutF32(position.y);
                    buffer->PutF32(position.z);
                    buffer->PutF32(position.orientation);
                }
            }
        }

        std::shared_ptr<DataStore> fieldbuffer = DataStore::Borrow<5304>();
        UpdateMask<1344> updateMask(PLAYER_END);

        u32* flags = UnitUpdateFieldFlags;
        i32 fieldDataValue = 0;
        u16 fieldNotifyFlags = UF_FLAG_DYNAMIC;
        
        for (u16 index = 0; index < PLAYER_END; index++)
        {
            playerFieldData.playerFields->Get<i32>(fieldDataValue, index * 4);
            if (fieldNotifyFlags & flags[index] || ((flags[index] & visibleFlags) & UF_FLAG_SPECIAL_INFO) || (fieldDataValue && (flags[index] & visibleFlags)))
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
                    // convert from i32 to uint32 and send
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
                    // convert from i32 to uint32 and send
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
                                //fieldbuffer << (m_uint32Values[UNIT_FIELD_BYTES_2] & ((UNIT_BYTE2_FLAG_SANCTUARY /*| UNIT_BYTE2_FLAG_AURAS | UNIT_BYTE2_FLAG_UNK5*/) << 8)); // this flag is at uint8 offset 1 !!
                            //else
                                // pretend that all other HOSTILE players have own faction, to allow follow, heal, rezz (trade wont work)
                                //fieldbuffer << uint32(target->GetFaction());
                        //}
                        //else
                            //fieldbuffer << m_uint32Values[index];
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

        buffer->PutU8(updateMask.GetBlocks());
        updateMask.AddTo(buffer.get());
        buffer->PutBytes(fieldbuffer->GetInternalData(), fieldbuffer->WrittenData);

        UpdateData updateData;
        updateData.AddBlock(buffer.get());

        std::shared_ptr<DataStore> tempBuffer = DataStore::Borrow<8192>();
        updateData.Build(tempBuffer.get(), opcode);

        return tempBuffer;
    }

    void Update(entt::registry& registry)
    {
        auto view = registry.view<PlayerInitializeComponent, PlayerFieldDataComponent, PlayerPositionComponent>();
        if (!view.empty())
        {
            SingletonComponent& singleton = registry.ctx<SingletonComponent>();
            PlayerUpdatesQueueSingleton& playerUpdatesQueue = registry.ctx<PlayerUpdatesQueueSingleton>();
            EntityCreateQueueSingleton& entityCreateQueue = registry.ctx<EntityCreateQueueSingleton>();
            CharacterDatabaseCacheSingleton& characterDatabase = registry.ctx<CharacterDatabaseCacheSingleton>();
            u32 lifeTimeInMS = static_cast<u32>(singleton.lifeTimeInMS);

            auto subView = registry.view<PlayerConnectionComponent, PlayerFieldDataComponent, PlayerPositionComponent>();
            view.each([&registry, &playerUpdatesQueue, &entityCreateQueue, &characterDatabase, lifeTimeInMS, subView](const auto, PlayerInitializeComponent& playerInitializeData, PlayerFieldDataComponent& playerFieldData, PlayerPositionComponent& clientPositionData)
                {
                    /* Build Self Packet, must be sent immediately */
                    u8 updateType = UPDATETYPE_CREATE_OBJECT2;
                    u16 selfUpdateFlag = (UPDATEFLAG_SELF | UPDATEFLAG_LIVING | UPDATEFLAG_STATIONARY_POSITION);
                    u32 selfVisibleFlags = (UF_FLAG_PUBLIC | UF_FLAG_PRIVATE);
                    u16 buildOpcode = 0;

                    std::shared_ptr<DataStore> selfPlayerUpdate = BuildPlayerCreateData(playerInitializeData.characterGuid, updateType, selfUpdateFlag, selfVisibleFlags, lifeTimeInMS, playerFieldData, clientPositionData, buildOpcode);
                    playerInitializeData.socket->SendPacket(selfPlayerUpdate.get(), buildOpcode);

                    // Call OnPlayerLogin script hooks
                    AngelScriptPlayer asPlayer(playerInitializeData.entityGuid, &registry);
                    PlayerHooks::CallHook(PlayerHooks::Hooks::HOOK_ONPLAYERLOGIN, &asPlayer);

                    robin_hood::unordered_map<u32, CharacterItemData> characterItemData;
                    if (characterDatabase.cache->GetCharacterItemData(playerInitializeData.characterGuid, characterItemData))
                    {
                        EntityCreationRequest entityCreationRequest;
                        entityCreationRequest.typeId = TYPEID_ITEM;

                        for (auto itr : characterItemData)
                        {
                            CharacterItemData characterItemData = itr.second;

                            ItemCreationInformation* itemCreationInformation = ItemCreationInformation::Create(characterItemData.lowGuid, characterItemData.itemEntry, characterItemData.bagSlot, characterItemData.bagPosition, playerInitializeData.entityGuid, playerInitializeData.characterGuid);
                            entityCreationRequest.typeInformation = itemCreationInformation;
                            entityCreateQueue.newEntityQueue->enqueue(entityCreationRequest);

                            ObjectGuid itemGuid(HighGuid::Item, itemCreationInformation->entryId, itemCreationInformation->lowGuid);
                            playerFieldData.SetGuidValue(PLAYER_FIELD_PACK_SLOT_1 + (itemCreationInformation->bagPosition - 23) * 2, itemGuid);
                        }
                    }

                    /* Build Self Packet for public */
                    u16 publicUpdateFlag = (UPDATEFLAG_LIVING | UPDATEFLAG_STATIONARY_POSITION);
                    u32 publicVisibleFlags = UF_FLAG_PUBLIC;

                    PlayerUpdatePacket playerUpdatePacket;
                    playerUpdatePacket.characterGuid = playerInitializeData.characterGuid;
                    playerUpdatePacket.updateType = updateType;
                    playerUpdatePacket.data = BuildPlayerCreateData(playerInitializeData.characterGuid, updateType, publicUpdateFlag, publicVisibleFlags, lifeTimeInMS, playerFieldData, clientPositionData, buildOpcode);
                    playerUpdatePacket.opcode = buildOpcode;
                    playerUpdatesQueue.playerUpdatePacketQueue.push_back(playerUpdatePacket);

                    subView.each([&playerUpdatesQueue, &playerInitializeData, lifeTimeInMS](const auto, PlayerConnectionComponent& connection, PlayerFieldDataComponent& fieldData, PlayerPositionComponent& positionData)
                        {
                            if (playerInitializeData.characterGuid != connection.characterGuid)
                            {
                                /* Build Player Packet for self */
                                u8 updateType = UPDATETYPE_CREATE_OBJECT;
                                u16 publicUpdateFlag = (UPDATEFLAG_LIVING | UPDATEFLAG_STATIONARY_POSITION);
                                u32 publicVisibleFlags = UF_FLAG_PUBLIC;
                                u16 buildOpcode = 0;

                                PlayerUpdatePacket playerUpdatePacket;
                                playerUpdatePacket.characterGuid = connection.characterGuid;
                                playerUpdatePacket.updateType = updateType;
                                playerUpdatePacket.data = BuildPlayerCreateData(connection.characterGuid, updateType, publicUpdateFlag, publicVisibleFlags, lifeTimeInMS, fieldData, positionData, buildOpcode);
                                playerUpdatePacket.opcode = buildOpcode;
                                playerUpdatesQueue.playerUpdatePacketQueue.push_back(playerUpdatePacket);
                            }
                        });
                });

            // Remove PlayerInitializeComponent from all entities (They've just been handled above)
            registry.reset<PlayerInitializeComponent>();
        }
    }
}
