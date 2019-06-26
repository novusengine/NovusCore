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
        std::shared_ptr<DataStore> buffer = DataStore::Borrow<8192>();
        /*
            Buffer Structure

            Start Core Structure
            - UpdateType : u8 (Specifies if the Data we are sending is a value update or a new object creation)
            - CharacterGuid : u64 (A compact GUID of the entity we are sending this update for)
            - TypeId : u8 (Specifies what kind of entity we are creating, e.g. TYPEID_PLAYER)

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

        buffer->PutU8(updateType);
        buffer->PutGuid(characterGuid);
        buffer->PutU8(TYPEID_PLAYER);

        buffer->PutU16(updateFlags);
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

        std::shared_ptr<DataStore> fieldbuffer = DataStore::Borrow<5304>();
        UpdateMask<1344> updateMask(PLAYER_END);

        u32* flags = UnitUpdateFieldFlags;
        i32 fieldDataValue = 0;

        for (u16 index = 0; index < PLAYER_END; index++)
        {
            playerFieldData.playerFields->Get<i32>(fieldDataValue, index * 4);
            if (UF_FLAG_DYNAMIC & flags[index] || (fieldDataValue && (flags[index] & visibleFlags)))
            {
                updateMask.SetBit(index);

                if (index == UNIT_FIELD_FLAGS)
                {
                    u32 appendValue = 0;
                    playerFieldData.playerFields->Get<u32>(appendValue, UNIT_FIELD_FLAGS * 4);

                    // If the player is a gamemaster, &= ~UNIT_FLAG_NOT_SELECTABLE 

                    fieldbuffer->PutU32(appendValue);
                }
                else if (index == UNIT_DYNAMIC_FLAGS)
                {
                    u32 dynamicFlags = 0;
                    playerFieldData.playerFields->Get<u32>(dynamicFlags, UNIT_DYNAMIC_FLAGS * 4);

                    // If player has tagged creature, |= ENTIY_DYNAMICFLAG_IS_TAGGED_BY_PLAYER;
                    // If player can loot creature, |= ENTIY_DYNAMICFLAG_IS_LOOTABLE;
                    // If player can track creature and has aura with MOD_STALKED, |= UNIT_DYNFLAG_TRACK_UNIT;
                    dynamicFlags &= ~(ENTIY_DYNAMICFLAG_IS_TAGGED | ENTIY_DYNAMICFLAG_IS_TAGGED_BY_PLAYER);

                    fieldbuffer->PutU32(dynamicFlags);
                }
                else
                {
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
                    u16 updateFlags = (UPDATEFLAG_SELF | UPDATEFLAG_LIVING | UPDATEFLAG_STATIONARY_POSITION);
                    u32 visibleFlags = (UF_FLAG_PUBLIC | UF_FLAG_PRIVATE);
                    u16 buildOpcode = 0;

                    std::shared_ptr<DataStore> selfPlayerUpdate = BuildPlayerCreateData(playerInitializeData.characterGuid, updateType, updateFlags, visibleFlags, lifeTimeInMS, playerFieldData, clientPositionData, buildOpcode);
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
