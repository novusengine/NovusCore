#pragma once
#include <NovusTypes.h>
#include <entt.hpp>
#include <Networking/ByteBuffer.h>

#include "../Connections/NovusConnection.h"
#include "../Components/ConnectionComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"

namespace PlayerUpdateDataSystem
{
    using namespace std::chrono;
    const steady_clock::time_point ApplicationStartTime = steady_clock::now();

    enum ObjectUpdateType : u8
    {
        UPDATETYPE_VALUES               = 0,
        UPDATETYPE_MOVEMENT             = 1,
        UPDATETYPE_CREATE_OBJECT        = 2,
        UPDATETYPE_CREATE_OBJECT2       = 3,
        UPDATETYPE_OUT_OF_RANGE_OBJECTS = 4,
        UPDATETYPE_NEAR_OBJECTS         = 5
    };
    enum ObjectUpdateFlag : u16
    {
        UPDATEFLAG_NONE                 = 0x0000,
        UPDATEFLAG_SELF                 = 0x0001,
        UPDATEFLAG_TRANSPORT            = 0x0002,
        UPDATEFLAG_HAS_TARGET           = 0x0004,
        UPDATEFLAG_UNKNOWN              = 0x0008,
        UPDATEFLAG_LOWGUID              = 0x0010,
        UPDATEFLAG_LIVING               = 0x0020,
        UPDATEFLAG_STATIONARY_POSITION  = 0x0040,
        UPDATEFLAG_VEHICLE              = 0x0080,
        UPDATEFLAG_POSITION             = 0x0100,
        UPDATEFLAG_ROTATION             = 0x0200
    };
    enum UpdatefieldFlags
    {
        UF_FLAG_NONE                    = 0x000,
        UF_FLAG_PUBLIC                  = 0x001,
        UF_FLAG_PRIVATE                 = 0x002,
        UF_FLAG_OWNER                   = 0x004,
        UF_FLAG_UNUSED1                 = 0x008,
        UF_FLAG_ITEM_OWNER              = 0x010,
        UF_FLAG_SPECIAL_INFO            = 0x020,
        UF_FLAG_PARTY_MEMBER            = 0x040,
        UF_FLAG_UNUSED2                 = 0x080,
        UF_FLAG_DYNAMIC                 = 0x100
    };

    Common::ByteBuffer BuildPlayerUpdateData(u64 playerGuid, u8 updateType, u16 updateFlags, u32 visibleFlags, PlayerUpdateDataComponent& playerUpdateData, PositionComponent position)
    {
        Common::ByteBuffer buffer(500);
        buffer.Write<u8>(updateType);
        buffer.AppendGuid(playerGuid);
        buffer.Write<u8>(4); // TYPEID_PLAYER

        // BuildMovementUpdate(ByteBuffer* data, uint16 flags)
        buffer.Write<u16>(updateFlags);

        if (updateFlags & UPDATEFLAG_LIVING)
        {
            u32 gameTimeMS = u32(duration_cast<milliseconds>(steady_clock::now() - ApplicationStartTime).count());
            //std::cout << "Gametime MS: " << gameTimeMS << std::endl;

            buffer.Write<u32>(0x00); // MovementFlags
            buffer.Write<u16>(0x00); // Extra MovementFlags
            buffer.Write<u32>(gameTimeMS); // Game Time
            // TaggedPosition<Position::XYZO>(pos);
            buffer.Write<f32>(position.x);
            buffer.Write<f32>(position.y);
            buffer.Write<f32>(position.z);
            buffer.Write<f32>(position.o);

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
                    buffer.Write<f32>(position.x);
                    buffer.Write<f32>(position.y);
                    buffer.Write<f32>(position.z);
                    buffer.Write<f32>(position.o);
                }
            }
        }

        Common::ByteBuffer fieldBuffer;
        fieldBuffer.Resize(5304);
        UpdateMask<1344> updateMask(PLAYER_END);

        u32* flags = UnitUpdateFieldFlags;
        u16 fieldNotifyFlags = UF_FLAG_DYNAMIC;

        for (u16 index = 0; index < PLAYER_END; ++index)
        {
            if (fieldNotifyFlags & flags[index] || ((flags[index] & visibleFlags) & UF_FLAG_SPECIAL_INFO)
                || ((updateType == 0 ? updateMask.IsSet(index) : playerUpdateData.playerFields.ReadAt<i32>(index * 4))
                    && (flags[index] & visibleFlags)))
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
        updateData.Build(tempBuffer);

        return tempBuffer;
    }

    void Update(f32 deltaTime, WorldServerHandler& worldServerHandler, NovusConnection& novusConnection, entt::registry &registry)
    {
        auto view = registry.view<ConnectionComponent, PlayerUpdateDataComponent, PositionComponent>();
        auto subView = registry.view<ConnectionComponent, PlayerUpdateDataComponent, PositionComponent>();

        view.each([&novusConnection, &worldServerHandler, subView](const auto, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData, PositionComponent& clientPositionData)
        {
            if (!clientConnection.isInitialized)
            {
                clientConnection.isInitialized = true;

                /* Build Player Data */
                u8 updateType = UPDATETYPE_CREATE_OBJECT2;
                u16 updateFlag = UPDATEFLAG_NONE;
                u32 visibleFlags = UF_FLAG_PUBLIC;

                updateFlag |= UPDATEFLAG_SELF;
                updateFlag |= UPDATEFLAG_LIVING;
                updateFlag |= UPDATEFLAG_STATIONARY_POSITION;
                visibleFlags |= UF_FLAG_PRIVATE;
                
                Common::ByteBuffer playerUpdateData = BuildPlayerUpdateData(clientConnection.characterGuid, updateType, updateFlag, visibleFlags, clientUpdateData, clientPositionData);

                NovusHeader novusHeader;
                novusHeader.CreateForwardHeader(clientConnection.accountGuid, Common::Opcode::SMSG_COMPRESSED_UPDATE_OBJECT, playerUpdateData.GetActualSize());

                Common::ByteBuffer sentUpdatePacket;
                novusHeader.AddTo(sentUpdatePacket);
                sentUpdatePacket.Append(playerUpdateData);
                novusConnection.SendPacket(sentUpdatePacket);
            }
            else
            {
                subView.each([&novusConnection, &worldServerHandler, &clientConnection, &clientUpdateData](const auto, ConnectionComponent& unitUpdateConnection, PlayerUpdateDataComponent& unitUpdateData, PositionComponent& unitPositionData)
                {
                    if (clientConnection.characterGuid != unitUpdateConnection.characterGuid)
                    {
                        if (std::find(clientUpdateData.visibleGuids.begin(), clientUpdateData.visibleGuids.end(), unitUpdateConnection.characterGuid) == clientUpdateData.visibleGuids.end())
                        {
                            /* Build Player Data */
                            u8 updateType = !unitUpdateConnection.isInitialized ? UPDATETYPE_CREATE_OBJECT2 : UPDATETYPE_CREATE_OBJECT;
                            u16 updateFlag = UPDATEFLAG_NONE;
                            u32 visibleFlags = UF_FLAG_PUBLIC;

                            // Living should be checked through UnitStatusComponent
                            updateFlag |= UPDATEFLAG_LIVING;
                            updateFlag |= UPDATEFLAG_STATIONARY_POSITION;

                            Common::ByteBuffer playerUpdateData = BuildPlayerUpdateData(unitUpdateConnection.characterGuid, updateType, updateFlag, visibleFlags, unitUpdateData, unitPositionData);

                            NovusHeader novusHeader;
                            novusHeader.CreateForwardHeader(clientConnection.accountGuid, Common::Opcode::SMSG_COMPRESSED_UPDATE_OBJECT, playerUpdateData.GetActualSize());

                            Common::ByteBuffer sentUpdatePacket;
                            novusHeader.AddTo(sentUpdatePacket);
                            sentUpdatePacket.Append(playerUpdateData);
                            novusConnection.SendPacket(sentUpdatePacket);

                            clientUpdateData.visibleGuids.push_back(unitUpdateConnection.characterGuid);
                        }
                    }
                });
            }
        });

        /* Reset the updateMask so we don't write twice */
        /*view.each([&novusConnection, &worldServerHandler, subView](const auto, ConnectionComponent& clientConnection, PlayerUpdateDataComponent& clientUpdateData, PositionComponent& clientPositionData)
        {
            clientUpdateData.updateMask.Reset();
        });*/
    }
}