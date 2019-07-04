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
#include <tracy\Tracy.hpp>
#include <Networking/ByteBuffer.h>

#include "../../NovusEnums.h"
#include "../../Utils/UpdateData.h"
#include "../../Utils/UpdateMask.h"

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerPositionComponent.h"
#include "../Components/PlayerInitializeComponent.h"
#include "../Components/PlayerFieldDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/MapSingleton.h"
#include "../Components/Singletons/EntityCreateQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"

#include "../../Scripting/PlayerFunctions.h"

namespace PlayerBuildDataSystem
{
std::shared_ptr<ByteBuffer> PlayerBuildInitialData(u64 characterGuid, u8 updateType, u16 updateFlags, u32 visibleFlags, u32 lifeTimeInMS, PlayerFieldDataComponent& playerFieldData, PlayerPositionComponent position, u16& opcode)
{
    ZoneScopedNC("PlayerBuildInitialData", tracy::Color::Yellow2)

        std::shared_ptr<ByteBuffer>
            buffer = ByteBuffer::Borrow<8192>();
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
            - FieldBuffer : ByteBuffer (Specifies the updated EntityFields)
        */

    buffer->PutU8(updateType);
    buffer->PutGuid(characterGuid);

    buffer->PutU8(TYPEID_PLAYER);

    buffer->PutU16(updateFlags);
    buffer->PutU32(0x00);         // MovementFlags
    buffer->PutU16(0x00);         // Extra MovementFlags
    buffer->PutU32(lifeTimeInMS); // Game Time
    buffer->Put<Vector3>(position.position);
    buffer->PutF32(position.orientation);

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

    std::shared_ptr<ByteBuffer> fieldbuffer = ByteBuffer::Borrow<5304>();
    UpdateMask<1344> updateMask(PLAYER_END);

    u32* flags = UnitUpdateFieldFlags;
    i32 fieldDataValue = 0;

    {
        ZoneScopedNC("PlayerBuildInitialData::Loop", tracy::Color::Yellow2) for (u16 index = 0; index < PLAYER_END; index++)
        {
            playerFieldData.playerFields->Get<i32>(fieldDataValue, index * 4);
            if (UPDATEFIELD_FLAG_PUBLIC & flags[index] || (fieldDataValue && (flags[index] & visibleFlags)))
            {
                updateMask.SetBit(index);

                if (index == UNIT_FIELD_FLAGS)
                {
                    u32 appendValue = 0;
                    playerFieldData.playerFields->Get<u32>(appendValue, UNIT_FIELD_FLAGS * 4);

                    // If the player is a gamemaster, |= UNIT_FLAG_NOT_SELECTABLE

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
    }

    {
        ZoneScopedNC("PlayerBuildInitialData::GetBlocks", tracy::Color::Yellow2)
            buffer->PutU8(updateMask.GetBlocks());
        updateMask.AddTo(buffer.get());
        buffer->PutBytes(fieldbuffer->GetInternalData(), fieldbuffer->WrittenData);
    }

    UpdateData updateData;
    {
        ZoneScopedNC("PlayerBuildInitialData::AddBlock", tracy::Color::Yellow2)
            updateData.AddBlock(buffer.get());
    }

    std::shared_ptr<ByteBuffer> tempBuffer = ByteBuffer::Borrow<8192>();
    {
        ZoneScopedNC("PlayerBuildInitialData::Build", tracy::Color::Yellow2)
            updateData.Build(tempBuffer.get(), opcode);
    }

    return tempBuffer;
}
void PlayerBuildDataBlock(u64 characterGuid, u32 visibleFlags, PlayerFieldDataComponent& playerFieldData)
{
    ZoneScopedNC("PlayerBuildUpdateData", tracy::Color::Yellow2)

        std::shared_ptr<ByteBuffer>
            buffer = ByteBuffer::Borrow<8192>();
    /*
            Buffer Structure

            Start Core Structure
            - UpdateType : u8 (Specifies if the Data we are sending is a value update or a new object creation)
            - CharacterGuid : u64 (A compact GUID of the entity we are sending this update for)

            End Core Structure
            - BlockCount : u8 (Specifies the amount of update blocks in this update packet)
            - BlockMask : BlockCount x u32 (Specifies the update mask for the given block)
            - FieldBuffer : ByteBuffer (Specifies the updated EntityFields)
        */

    buffer->PutU8(UPDATETYPE_VALUES);
    buffer->PutGuid(characterGuid);

    std::shared_ptr<ByteBuffer> fieldbuffer = ByteBuffer::Borrow<5304>();
    UpdateMask<1344> updateMask(PLAYER_END);

    u32* flags = UnitUpdateFieldFlags;

    {
        ZoneScopedNC("PlayerBuildUpdateData::Loop", tracy::Color::Yellow2) for (u16 index = 0; index < PLAYER_END; index++)
        {
            if (UPDATEFIELD_FLAG_PUBLIC & flags[index] || ((flags[index] & visibleFlags) & UPDATEFIELD_FLAG_SPECIAL_INFO) ||
                (playerFieldData.changesMask.IsSet(index) && (flags[index] & visibleFlags)))
            {
                updateMask.SetBit(index);

                if (index == UNIT_FIELD_FLAGS)
                {
                    u32 appendValue = 0;
                    playerFieldData.playerFields->Get<u32>(appendValue, UNIT_FIELD_FLAGS * 4);

                    // If the player is a gamemaster, |= UNIT_FLAG_NOT_SELECTABLE

                    fieldbuffer->PutU32(appendValue);
                }
                else if (index == UNIT_FIELD_AURASTATE)
                {
                    // Here we should check the aurastate for our target. As an example, sending the correct value here is what would allow us to cast Conflagurate
                    u32 auraState = 0;
                    /*playerFieldData.playerFields->Get<u32>(auraState, UNIT_FIELD_AURASTATE * 4);
                            auraState &= ~(((1 << (14 - 1)) | (1 << (16 - 1))));*/

                    fieldbuffer->PutU32(auraState);
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
    }

    {
        ZoneScopedNC("PlayerBuildUpdateData::GetBlocks", tracy::Color::Yellow2)
            buffer->PutU8(updateMask.GetBlocks());
        updateMask.AddTo(buffer.get());
        buffer->PutBytes(fieldbuffer->GetInternalData(), fieldbuffer->WrittenData);
    }

    {
        ZoneScopedNC("PlayerBuildUpdateData::AddBlock", tracy::Color::Yellow2)
            playerFieldData.updateData.AddBlock(buffer.get());
    }
}

void Update(entt::registry& registry)
{
    SingletonComponent& singleton = registry.ctx<SingletonComponent>();
    MapSingleton& mapSingleton = registry.ctx<MapSingleton>();
    u32 lifeTimeInMS = static_cast<u32>(singleton.lifeTimeInMS);

    auto buildInitialDataView = registry.view<PlayerInitializeComponent, PlayerFieldDataComponent, PlayerPositionComponent>();
    if (!buildInitialDataView.empty())
    {
        EntityCreateQueueSingleton& entityCreateQueue = registry.ctx<EntityCreateQueueSingleton>();
        CharacterDatabaseCacheSingleton& characterDatabase = registry.ctx<CharacterDatabaseCacheSingleton>();

        buildInitialDataView.each([&registry, &entityCreateQueue, &characterDatabase, lifeTimeInMS](const auto, PlayerInitializeComponent& playerInitializeComponent, PlayerFieldDataComponent& playerFieldData, PlayerPositionComponent& playerPositionData) {
            /* Build Self Packet, must be sent immediately */
            u8 updateType = UPDATETYPE_CREATE_OBJECT2;
            u16 updateFlags = (UPDATEFLAG_SELF | UPDATEFLAG_LIVING | UPDATEFLAG_STATIONARY_POSITION);
            u32 visibleFlags = (UPDATEFIELD_FLAG_PUBLIC | UPDATEFIELD_FLAG_SELF);
            u16 buildOpcode = 0;

            std::shared_ptr<ByteBuffer> update = PlayerBuildInitialData(playerInitializeComponent.characterGuid, updateType, updateFlags, visibleFlags, lifeTimeInMS, playerFieldData, playerPositionData, buildOpcode);
            playerInitializeComponent.socket->SendPacket(update.get(), buildOpcode);

            // Call OnPlayerLogin script hooks
            AngelScriptPlayer asPlayer(playerInitializeComponent.entityId, &registry);
            PlayerHooks::CallHook(PlayerHooks::Hooks::HOOK_ONPLAYERLOGIN, &asPlayer);

            robin_hood::unordered_map<u32, CharacterItemData> characterItemData;
            if (characterDatabase.cache->GetCharacterItemData(playerInitializeComponent.characterGuid, characterItemData))
            {
                EntityCreationRequest entityCreationRequest;
                entityCreationRequest.typeId = TYPEID_ITEM;

                for (auto itr : characterItemData)
                {
                    CharacterItemData characterItemData = itr.second;

                    ItemCreationInformation* itemCreationInformation = ItemCreationInformation::Create(characterItemData.lowGuid, characterItemData.itemEntry, characterItemData.bagSlot, characterItemData.bagPosition, playerInitializeComponent.entityId, playerInitializeComponent.characterGuid);
                    entityCreationRequest.typeInformation = itemCreationInformation;
                    entityCreateQueue.newEntityQueue->enqueue(entityCreationRequest);

                    ObjectGuid itemGuid(HighGuid::Item, itemCreationInformation->entryId, itemCreationInformation->lowGuid);
                    playerFieldData.SetGuidValue(PLAYER_FIELD_PACK_SLOT_1 + (itemCreationInformation->bagPosition - 23) * 2, itemGuid);
                }
            }
        });

        registry.reset<PlayerInitializeComponent>();
    }

    auto buildUpdateDataView = registry.view<PlayerConnectionComponent, PlayerFieldDataComponent, PlayerPositionComponent, PlayerUpdateDataComponent>();
    buildUpdateDataView.each([&registry, &mapSingleton, lifeTimeInMS](const auto, PlayerConnectionComponent& playerConnection, PlayerFieldDataComponent& playerFieldData, PlayerPositionComponent& playerPositionData, PlayerUpdateDataComponent& playerUpdateData) {
        playerFieldData.updateData.ResetBlocks();

        Vector2 position = Vector2(playerPositionData.position.x, playerPositionData.position.y);
        u32 mapId = playerPositionData.mapId;

        u16 adtId;
        if (mapSingleton.maps[mapId].GetAdtIdFromWorldPosition(position, adtId))
        {
            if (adtId != playerPositionData.adtId)
            {
                u32 entityId = playerConnection.entityId;
                u64 characterGuid = playerConnection.characterGuid;

                std::vector<u32>& playerList = mapSingleton.maps[mapId].playersInAdts[playerPositionData.adtId];
                if (playerPositionData.adtId != INVALID_ADT)
                {
                    auto iterator = std::find(playerList.begin(), playerList.end(), entityId);
                    assert(iterator != playerList.end());
                    playerList.erase(iterator);

                    for (u32 entity : playerList)
                    {
                        PlayerConnectionComponent& currentConnection = registry.get<PlayerConnectionComponent>(entity);
                        PlayerUpdateDataComponent& currentUpdateData = registry.get<PlayerUpdateDataComponent>(entity);
                        PlayerFieldDataComponent& currentFieldData = registry.get<PlayerFieldDataComponent>(entity);

                        u64 currentCharacterGuid = currentConnection.characterGuid;

                        auto iterator = std::find(currentUpdateData.visibleGuids.begin(), currentUpdateData.visibleGuids.end(), characterGuid);
                        if (iterator != currentUpdateData.visibleGuids.end())
                        {
                            currentUpdateData.visibleGuids.erase(iterator);
                            currentFieldData.updateData.AddInvalidGuid(characterGuid);
                        }

                        auto iterator_2 = std::find(playerUpdateData.visibleGuids.begin(), playerUpdateData.visibleGuids.end(), currentCharacterGuid);
                        if (iterator_2 != playerUpdateData.visibleGuids.end())
                        {
                            playerUpdateData.visibleGuids.erase(iterator_2);
                            playerFieldData.updateData.AddInvalidGuid(currentCharacterGuid);
                        }
                    }

                    playerConnection.SendChatNotification("[DEBUG] Old ADT: %u has %u players", playerPositionData.adtId, playerList.size());
                }

                playerPositionData.adtId = adtId;

                mapSingleton.maps[mapId].playersInAdts[adtId].push_back(entityId);
                playerConnection.SendChatNotification("[DEBUG] New ADT: %u has %u players", adtId, mapSingleton.maps[mapId].playersInAdts[adtId].size());
            }

            std::vector<u32>& playerList = mapSingleton.maps[mapId].playersInAdts[playerPositionData.adtId];
            for (u32 entity : playerList)
            {
                if (playerConnection.entityId == entity)
                {
                    if (!playerFieldData.changesMask.Any())
                        continue;

                    u32 visibleFlags = (UPDATEFIELD_FLAG_PUBLIC | UPDATEFIELD_FLAG_SELF);

                    PlayerBuildDataBlock(playerConnection.characterGuid, visibleFlags, playerFieldData);
                    playerFieldData.changesMask.Reset();
                }
                else
                {
                    PlayerConnectionComponent& currentConnection = registry.get<PlayerConnectionComponent>(entity);
                    PlayerFieldDataComponent& currentFieldData = registry.get<PlayerFieldDataComponent>(entity);
                    u32 visibleFlags = UPDATEFIELD_FLAG_PUBLIC;

                    auto position = std::find(playerUpdateData.visibleGuids.begin(), playerUpdateData.visibleGuids.end(), currentConnection.characterGuid);
                    if (position != playerUpdateData.visibleGuids.end())
                    {
                        if (!currentFieldData.changesMask.Any())
                            continue;

                        PlayerBuildDataBlock(currentConnection.characterGuid, visibleFlags, currentFieldData);
                    }
                    else
                    {
                        u8 updateType = UPDATETYPE_CREATE_OBJECT2;
                        u16 updateFlags = (UPDATEFLAG_LIVING | UPDATEFLAG_STATIONARY_POSITION);
                        u16 buildOpcode = 0;

                        PlayerPositionComponent& currentPositionData = registry.get<PlayerPositionComponent>(entity);

                        std::shared_ptr<ByteBuffer> update = PlayerBuildInitialData(currentConnection.characterGuid, updateType, updateFlags, visibleFlags, lifeTimeInMS, currentFieldData, currentPositionData, buildOpcode);
                        playerConnection.socket->SendPacket(update.get(), buildOpcode);
                        playerUpdateData.visibleGuids.push_back(currentConnection.characterGuid);
                    }
                }
            }

            if (!playerFieldData.updateData.IsEmpty())
            {
                u16 buildOpcode = 0;
                std::shared_ptr<ByteBuffer> update = ByteBuffer::Borrow<8192>();
                {
                    ZoneScopedNC("BuildUpdateDataView::Build", tracy::Color::Yellow2)
                        playerFieldData.updateData.Build(update.get(), buildOpcode);
                    playerFieldData.updateData.ResetInvalidGuids();
                }

                playerConnection.socket->SendPacket(update.get(), buildOpcode);
            }
        }

        if (!playerUpdateData.positionUpdateData.empty())
        {
            ZoneScopedNC("PositionUpdate", tracy::Color::Yellow2) for (PositionUpdateData positionData : playerUpdateData.positionUpdateData)
            {
                std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<38>();
                buffer->PutGuid(playerConnection.characterGuid);
                buffer->PutU32(positionData.movementFlags);
                buffer->PutU16(positionData.movementFlagsExtra);
                buffer->PutU32(positionData.gameTime);
                buffer->Put<Vector3>(positionData.position);
                buffer->PutF32(positionData.orientation);
                buffer->PutU32(positionData.fallTime);

                CharacterUtils::SendPacketToGridPlayers(&registry, playerConnection.entityId, buffer, positionData.opcode, true);
            }

            // Clear Position Updates
            playerUpdateData.positionUpdateData.clear();
        }

        if (!playerUpdateData.chatUpdateData.empty())
        {
            ZoneScopedNC("ChatUpdate", tracy::Color::Yellow2) for (ChatUpdateData chatData : playerUpdateData.chatUpdateData)
            {
                std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<286>();

                buffer->PutU8(chatData.chatType);
                buffer->PutI32(chatData.language);
                buffer->PutU64(chatData.sender);
                buffer->PutU32(0); // Chat Flag (??)

                // This is based on chatType
                buffer->PutU64(0); // Receiver (0) for none

                buffer->PutU32(static_cast<u32>(chatData.message.length()) + 1);
                buffer->PutString(chatData.message);
                buffer->PutU8(0); // Chat Tag

                CharacterUtils::SendPacketToGridPlayers(&registry, playerConnection.entityId, buffer, Opcode::SMSG_MESSAGECHAT);
            }

            // Clear Chat Updates
            playerUpdateData.chatUpdateData.clear();
        }
    });
}
} // namespace PlayerBuildDataSystem
