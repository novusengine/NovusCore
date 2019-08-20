#include "CharacterPacketUtils.h"
#include "../../NovusEnums.h"
#include "../../MessageHandler.h"
#include "../../WorldNodeHandler.h"
#include "../../ECS/Components/Singletons/SingletonComponent.h"
#include "../../ECS/Components/Singletons/DBCDatabaseCacheSingleton.h"
#include "../../ECS/Components/Singletons/CharacterDatabaseCacheSingleton.h"
#include "../../ECS/Components/Singletons/WorldDatabaseCacheSingleton.h"
#include "../../ECS/Components/Singletons/MapSingleton.h"
#include "../../ECS/Components/PlayerConnectionComponent.h"
#include "../../ECS/Components/PlayerPositionComponent.h"
#include "../../ECS/Components/PlayerUpdateDataComponent.h"
#include "../../ECS/Components/PlayerFieldDataComponent.h"
#include "../../ECS/Components/AuraListComponent.h"

#include "../../Scripting/PlayerFunctions.h"
#include "../../Scripting/SpellFunctions.h"
#include "../../Utils/ServiceLocator.h"
#include "../../Utils/BotUtils.h"

#include <Database/Cache/DBCDatabaseCache.h>
#include <Database/Cache/CharacterDatabaseCache.h>
#include <Database/Cache/WorldDatabaseCache.h>
#include <tracy/Tracy.hpp>
#include <Utils/DebugHandler.h>

void CharacterPacketUtils::RegisterCharacterPacketHandlers()
{
    MessageHandler* messageHandler = MessageHandler::Instance();

    messageHandler->SetMessageHandler(Opcode::CMSG_SET_ACTIVE_MOVER, HandleSetActiveMover);
    messageHandler->SetMessageHandler(Opcode::CMSG_STANDSTATECHANGE, HandleStandStateChange);
    messageHandler->SetMessageHandler(Opcode::CMSG_SET_SELECTION, HandleSetSelection);
    messageHandler->SetMessageHandler(Opcode::CMSG_NAME_QUERY, HandleNameQuery);
    messageHandler->SetMessageHandler(Opcode::CMSG_ITEM_QUERY_SINGLE, HandleItemQuerySingle);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_STOP, HandleItemQuerySingle);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_STOP, HandleItemQuerySingle);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_STOP, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_STOP_STRAFE, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_STOP_TURN, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_STOP_PITCH, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_FORWARD, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_BACKWARD, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_STRAFE_LEFT, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_STRAFE_RIGHT, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_TURN_LEFT, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_TURN_RIGHT, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_PITCH_UP, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_PITCH_DOWN, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_ASCEND, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_STOP_ASCEND, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_DESCEND, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_START_SWIM, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_STOP_SWIM, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_FALL_LAND, HandleMove);
    messageHandler->SetMessageHandler(Opcode::CMSG_MOVE_FALL_RESET, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_JUMP, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_SET_FACING, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_SET_PITCH, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_SET_RUN_MODE, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_SET_WALK_MODE, HandleMove);
    messageHandler->SetMessageHandler(Opcode::CMSG_MOVE_SET_FLY, HandleMove);
    messageHandler->SetMessageHandler(Opcode::CMSG_MOVE_CHNG_TRANSPORT, HandleMove);
    messageHandler->SetMessageHandler(Opcode::MSG_MOVE_HEARTBEAT, HandleMove);
    messageHandler->SetMessageHandler(Opcode::CMSG_MESSAGECHAT, HandleChatMessage);
    messageHandler->SetMessageHandler(Opcode::CMSG_ATTACKSWING, HandleAttackSwing);
    messageHandler->SetMessageHandler(Opcode::CMSG_ATTACKSTOP, HandleAttackStop);
    messageHandler->SetMessageHandler(Opcode::CMSG_SETSHEATHED, HandleSetSheathed);
    messageHandler->SetMessageHandler(Opcode::CMSG_TEXT_EMOTE, HandleTextEmote);
    messageHandler->SetMessageHandler(Opcode::CMSG_CAST_SPELL, HandleCastSpell);
    messageHandler->SetMessageHandler(Opcode::CMSG_CANCEL_AURA, HandleCancelAura);
    messageHandler->SetMessageHandler(Opcode::CMSG_CONTACT_LIST, HandleContactList);
}

bool CharacterPacketUtils::HandleSetActiveMover(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::SetActiveMover", tracy::Color::Orange2)

        std::shared_ptr<ByteBuffer>
            timeSync = ByteBuffer::Borrow<4>();
    timeSync->PutU32(0);

    playerConnectionComponent->socket->SendPacket(timeSync.get(), Opcode::SMSG_TIME_SYNC_REQ);
    return true;
}

bool CharacterPacketUtils::HandleStandStateChange(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::StandStateChange", tracy::Color::Orange2)

        u32 standState = 0;
    packet->data->GetU32(standState);

    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerFieldDataComponent& playerFieldDataComponent = registry->get<PlayerFieldDataComponent>(playerConnectionComponent->entityId);

    playerFieldDataComponent.SetFieldValue<u8>(UNIT_FIELD_BYTES_1, static_cast<u8>(standState));

    std::shared_ptr<ByteBuffer> standStateChange = ByteBuffer::Borrow<1>();
    standStateChange->PutU8(static_cast<u8>(standState));

    playerConnectionComponent->socket->SendPacket(standStateChange.get(), Opcode::SMSG_STANDSTATE_UPDATE);
    return true;
}

bool CharacterPacketUtils::HandleSetSelection(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::SetSelection", tracy::Color::Orange2)

        u64 selectedGuid = 0;
    packet->data->GetGuid(selectedGuid);

    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerFieldDataComponent& playerFieldDataComponent = registry->get<PlayerFieldDataComponent>(playerConnectionComponent->entityId);

    playerFieldDataComponent.SetGuidValue(UNIT_FIELD_TARGET, selectedGuid);
    return true;
}

bool CharacterPacketUtils::HandleNameQuery(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::NameQuery", tracy::Color::Orange2)

        u64 guid;
    packet->data->GetU64(guid);

    entt::registry* registry = ServiceLocator::GetMainRegistry();
    CharacterDatabaseCacheSingleton& characterDatabaseCacheSingleton = registry->ctx<CharacterDatabaseCacheSingleton>();

    std::shared_ptr<ByteBuffer> nameQuery = ByteBuffer::Borrow<30>();
    nameQuery->PutGuid(guid);

    CharacterInfo characterInfo;
    if (characterDatabaseCacheSingleton.cache->GetCharacterInfo(guid, characterInfo))
    {
        nameQuery->PutU8(0); // Name Unknown (0 = false, 1 = true);
        nameQuery->PutString(characterInfo.name);
        nameQuery->PutU8(0);
        nameQuery->PutU8(characterInfo.race);
        nameQuery->PutU8(characterInfo.gender);
        nameQuery->PutU8(characterInfo.classId);
    }
    else
    {
        nameQuery->PutU8(1); // Name Unknown (0 = false, 1 = true);
        nameQuery->PutString("Unknown");
        nameQuery->PutU8(0);
        nameQuery->PutU8(0);
        nameQuery->PutU8(0);
        nameQuery->PutU8(0);
    }
    nameQuery->PutU8(0);

    playerConnectionComponent->socket->SendPacket(nameQuery.get(), Opcode::SMSG_NAME_QUERY_RESPONSE);
    return true;
}

bool CharacterPacketUtils::HandleItemQuerySingle(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    u32 itemEntry = 0;
    packet->data->GetU32(itemEntry);

    entt::registry* registry = ServiceLocator::GetMainRegistry();
    WorldDatabaseCacheSingleton& worldDatabaseCacheSingleton = registry->ctx<WorldDatabaseCacheSingleton>();

    std::shared_ptr<ByteBuffer> itemQuery;
    ItemTemplate itemTemplate;
    if (!worldDatabaseCacheSingleton.cache->GetItemTemplate(itemEntry, itemTemplate))
    {
        itemQuery = ByteBuffer::Borrow<4>();
        itemQuery->PutU32(itemEntry | 0x80000000);
        playerConnectionComponent->socket->SendPacket(itemQuery.get(), Opcode::SMSG_ITEM_QUERY_SINGLE_RESPONSE);
    }
    else
    {
        playerConnectionComponent->socket->SendPacket(itemTemplate.GetQuerySinglePacket().get(), Opcode::SMSG_ITEM_QUERY_SINGLE_RESPONSE);
    }

    return true;
}

bool CharacterPacketUtils::HandleMove(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::Passthrough", tracy::Color::Orange2)

        entt::registry* registry = ServiceLocator::GetMainRegistry();
    SingletonComponent& singletonComponent = registry->ctx<SingletonComponent>();
    PlayerPositionComponent& playerPositionComponent = registry->get<PlayerPositionComponent>(playerConnectionComponent->entityId);
    PlayerUpdateDataComponent& playerUpdateDataComponent = registry->get<PlayerUpdateDataComponent>(playerConnectionComponent->entityId);

    u8 opcodeIndex = CharacterUtils::GetLastMovementTimeIndexFromOpcode(packet->opcode);
    u32 opcodeTime = playerPositionComponent.lastMovementOpcodeTime[opcodeIndex];

    u64 guid = 0;
    MovementData movementData;
    packet->data->GetGuid(guid);
    playerPositionComponent.ReadMovementData(packet->data, movementData);

    // Find time offset
    if (playerPositionComponent.initialServerTime == 0)
    {
        playerPositionComponent.initialServerTime = static_cast<u32>(singletonComponent.lifeTimeInMS);
        playerPositionComponent.initialClientTime = movementData.gameTime;
    }

    if (movementData.gameTime > opcodeTime)
    {
        playerPositionComponent.lastMovementOpcodeTime[opcodeIndex] = movementData.gameTime;

        PositionUpdateData positionUpdateData;
        positionUpdateData.opcode = packet->opcode;
        positionUpdateData.movementFlags = movementData.movementFlags;
        positionUpdateData.movementFlagsExtra = movementData.movementFlagsExtra;
        positionUpdateData.gameTime = static_cast<u32>(singletonComponent.lifeTimeInMS);
        positionUpdateData.fallTime = movementData.fallTime;

        playerPositionComponent.movementData.position = movementData.position;
        playerPositionComponent.movementData.orientation = movementData.orientation;

        positionUpdateData.position = movementData.position;
        positionUpdateData.orientation = movementData.orientation;

        playerUpdateDataComponent.positionUpdateData.push_back(positionUpdateData);
    }

    return true;
}
bool CharacterPacketUtils::HandleChatMessage(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::MessageChat", tracy::Color::Orange2)

        entt::registry* registry = ServiceLocator::GetMainRegistry();

    PlayerUpdateDataComponent& playerUpdateData = registry->get<PlayerUpdateDataComponent>(playerConnectionComponent->entityId);
    PlayerFieldDataComponent& playerFieldData = registry->get<PlayerFieldDataComponent>(playerConnectionComponent->entityId);

    u32 msgType;
    u32 msgLang;

    packet->data->GetU32(msgType);
    packet->data->GetU32(msgLang);

    if (msgType >= CHAT_MSG_TYPE_MAX)
    {
        // Client tried to use invalid type
        return true;
    }

    if (msgLang == LANG_UNIVERSAL && msgType != CHAT_MSG_AFK && msgType != CHAT_MSG_DND)
    {
        // Client tried to send a message in universal language. (While it not being afk or dnd)
        return true;
    }

    if (msgType == CHAT_MSG_AFK || msgType == CHAT_MSG_DND)
    {
        // We don't want to send this message to any client.
        return true;
    }

    std::string msgOutput;
    switch (msgType)
    {
    case CHAT_MSG_SAY:
    case CHAT_MSG_YELL:
    case CHAT_MSG_EMOTE:
    case CHAT_MSG_TEXT_EMOTE:
    {
        packet->data->GetString(msgOutput);
        break;
    }

    default:
    {
        NC_LOG_MESSAGE("Account(%u), Character(%u) sent unhandled message type %u", playerConnectionComponent->accountGuid, playerConnectionComponent->characterGuid, msgType);
        break;
    }
    }

    // Max Message Size is 255
    if (msgOutput.size() > 255)
        return true;

    if (msgOutput == "health")
    {
        std::shared_ptr<ByteBuffer> byteBuffer = ByteBuffer::Borrow<13>();
        /*byteBuffer->PutGuid(playerConnection.characterGuid);
                    byteBuffer->PutU32(0);
                    playerConnection.socket->SendPacket(byteBuffer.get(), Opcode::SMSG_HEALTH_UPDATE);
                    */
        playerFieldData.SetFieldValue<u32>(UNIT_FIELD_HEALTH, 0);

        byteBuffer->Reset();
        byteBuffer->PutGuid(playerConnectionComponent->characterGuid);
        byteBuffer->PutU8(1);
        byteBuffer->PutU32(750);
        playerConnectionComponent->socket->SendPacket(byteBuffer.get(), Opcode::SMSG_POWER_UPDATE);
    }

    if (msgOutput == "bot")
    {
        PlayerPositionComponent& playerPositionComponent = registry->get<PlayerPositionComponent>(playerConnectionComponent->entityId);
        BotUtils::CreateBot(playerPositionComponent.mapId, playerPositionComponent.movementData.position, playerPositionComponent.movementData.orientation);
    }

    /* Build Packet */
    ChatUpdateData chatUpdateData;
    chatUpdateData.chatType = msgType;
    chatUpdateData.language = msgLang;
    chatUpdateData.sender = playerConnectionComponent->characterGuid;
    chatUpdateData.message = msgOutput;
    chatUpdateData.handled = false;
    playerUpdateData.chatUpdateData.push_back(chatUpdateData);

    // Call OnPlayerChat script hooks
    AngelScriptPlayer asPlayer(playerConnectionComponent->entityId);
    PlayerHooks::CallHook(PlayerHooks::Hooks::HOOK_ONPLAYERCHAT, &asPlayer, msgOutput);
    return true;
}

bool CharacterPacketUtils::HandleAttackSwing(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::AttackSwing", tracy::Color::Orange2)

        entt::registry* registry = ServiceLocator::GetMainRegistry();

    u64 attackGuid;
    packet->data->GetU64(attackGuid);

    std::shared_ptr<ByteBuffer> attackBuffer = ByteBuffer::Borrow<50>();
    attackBuffer->PutU64(playerConnectionComponent->characterGuid);
    attackBuffer->PutU64(attackGuid);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, attackBuffer, Opcode::SMSG_ATTACKSTART);

    attackBuffer->Reset();
    attackBuffer->PutU32(0);
    attackBuffer->PutGuid(playerConnectionComponent->characterGuid);
    attackBuffer->PutGuid(attackGuid);
    attackBuffer->PutU32(5);
    attackBuffer->PutU32(0);
    attackBuffer->PutU8(1);

    attackBuffer->PutU32(1);
    attackBuffer->PutF32(5);
    attackBuffer->PutU32(5);

    attackBuffer->PutU8(0);
    attackBuffer->PutU32(0);
    attackBuffer->PutU32(0);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, attackBuffer, Opcode::SMSG_ATTACKERSTATEUPDATE);

    return true;
}

bool CharacterPacketUtils::HandleAttackStop(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::AttackStop", tracy::Color::Orange2)

        entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerFieldDataComponent& playerFieldData = registry->get<PlayerFieldDataComponent>(playerConnectionComponent->entityId);

    u64 attackGuid = playerFieldData.GetFieldValue<u64>(UNIT_FIELD_TARGET);

    std::shared_ptr<ByteBuffer> attackStop = ByteBuffer::Borrow<20>();
    attackStop->PutGuid(playerConnectionComponent->characterGuid);
    attackStop->PutGuid(attackGuid);
    attackStop->PutU32(0);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, attackStop, Opcode::SMSG_ATTACKSTOP);

    return true;
}

bool CharacterPacketUtils::HandleSetSheathed(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::SetSheathed", tracy::Color::Orange2)

        entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerFieldDataComponent& playerFieldData = registry->get<PlayerFieldDataComponent>(playerConnectionComponent->entityId);

    u32 state;
    packet->data->GetU32(state);

    playerFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, static_cast<u8>(state));
    return true;
}

bool CharacterPacketUtils::HandleTextEmote(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::Text_emote", tracy::Color::Orange2)

        entt::registry* registry = ServiceLocator::GetMainRegistry();
    CharacterDatabaseCacheSingleton& characterDatabase = registry->ctx<CharacterDatabaseCacheSingleton>();
    DBCDatabaseCacheSingleton& dbcDatabase = registry->ctx<DBCDatabaseCacheSingleton>();

    u32 emoteTextId;
    u32 emoteSoundIndex;
    u64 targetGuid;

    packet->data->GetU32(emoteTextId);
    packet->data->GetU32(emoteSoundIndex);
    packet->data->GetU64(targetGuid);

    EmoteTextData emoteTextData;
    if (dbcDatabase.cache->GetEmoteTextData(emoteTextId, emoteTextData))
    {
        std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<36>();
        /* Play animation packet. */
        {
            //The animation shouldn't play if the player is dead. In the future we should check for that.

            buffer->PutU32(emoteTextData.animationId);
            buffer->PutU64(playerConnectionComponent->characterGuid);

            CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, buffer, Opcode::SMSG_EMOTE);
        }

        /* Emote Chat Message Packet. */
        {
            CharacterInfo targetData;
            u32 targetNameLength = 0;
            if (characterDatabase.cache->GetCharacterInfo(targetGuid, targetData))
            {
                targetNameLength = static_cast<u32>(targetData.name.size());
            }

            buffer->Reset();
            buffer->PutU64(playerConnectionComponent->characterGuid);
            buffer->PutU32(emoteTextId);
            buffer->PutU32(emoteSoundIndex);
            buffer->PutU32(targetNameLength);
            if (targetNameLength > 1)
            {
                buffer->PutString(targetData.name);
            }
            else
            {
                buffer->PutU8(0x00);
            }

            CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, buffer, Opcode::SMSG_TEXT_EMOTE);
        }
    }
    return true;
}

bool CharacterPacketUtils::HandleCastSpell(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::CastSpell", tracy::Color::Orange2)

        entt::registry* registry = ServiceLocator::GetMainRegistry();
    SingletonComponent& singleton = registry->ctx<SingletonComponent>();
    MapSingleton& mapSingleton = registry->ctx<MapSingleton>();
    DBCDatabaseCacheSingleton& dbcDatabase = registry->ctx<DBCDatabaseCacheSingleton>();
    PlayerPositionComponent& playerPositionComponent = registry->get<PlayerPositionComponent>(playerConnectionComponent->entityId);

    u32 spellId = 0, targetFlags = 0;
    u8 castCount = 0, castFlags = 0;

    packet->data->GetU8(castCount);
    packet->data->GetU32(spellId);
    packet->data->GetU8(castFlags);
    packet->data->GetU32(targetFlags);

    SpellData spellData;
    if (!dbcDatabase.cache->GetSpellData(spellId, spellData))
        return true;

    // As far as I can tell, the client expects SMSG_SPELL_START followed by SMSG_SPELL_GO.
    std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<512>();

    // Call OnSpellCast script hooks
    AngelScriptSpellTemplate asSpellTemplate;
    AngelScriptSpell asSpell(&asSpellTemplate);
    AngelScriptPlayer asPlayer(playerConnectionComponent->entityId);
    SpellHooks::CallHook(SpellHooks::Hooks::HOOK_ON_SPELL_BEGIN_CAST, &asPlayer, &asSpell);

    AuraListComponent& auraList = registry->get<AuraListComponent>(playerConnectionComponent->entityId);
    for (i32 i = 0; i < SPELL_EFFECTS_COUNT; i++)
    {
        SpellEffectHooks::CallHook(SpellEffectHooks::Hooks::HOOK_ON_SPELL_EFFECT_BEFORE_HIT, spellData.Effect[i], &asPlayer, &asSpell);

        if (spellData.Effect[i] == SPELL_EFFECT_LEAP)
        {
            f32 tempHeight = playerPositionComponent.movementData.position.z;
            u32 dest = 20;

            for (u32 i = 0; i < 20; i++)
            {
                f32 newPositionX = playerPositionComponent.movementData.position.x + i * Math::Cos(playerPositionComponent.movementData.orientation);
                f32 newPositionY = playerPositionComponent.movementData.position.y + i * Math::Sin(playerPositionComponent.movementData.orientation);
                Vector2 newPos(newPositionX, newPositionY);
                f32 height = mapSingleton.maps[playerPositionComponent.mapId].GetHeight(newPos);
                f32 deltaHeight = Math::Abs(tempHeight - height);

                if (deltaHeight <= 2.0f || (i == 0 && deltaHeight <= 20))
                {
                    dest = i;
                    tempHeight = height;
                }
            }

            if (dest == 20)
            {
                buffer->Reset();
                buffer->PutU8(castCount);
                buffer->PutU32(spellId);
                buffer->PutU8(173); // SPELL_FAILED_TRY_AGAIN

                playerConnectionComponent->socket->SendPacket(buffer.get(), Opcode::SMSG_CAST_FAILED);
                return true;
            }

            /*
                            Adding 2.0f to the final height will solve 90%+ of issues where we fall through the terrain, remove this to fully test blink's capabilities.
                            This also introduce the bug where after a blink, you might appear a bit over the ground and fall down.
                        */
            f32 newPositionX = playerPositionComponent.movementData.position.x + dest * Math::Cos(playerPositionComponent.movementData.orientation);
            f32 newPositionY = playerPositionComponent.movementData.position.y + dest * Math::Sin(playerPositionComponent.movementData.orientation);
            Vector2 newPos(newPositionX, newPositionY);
            //f32 height = mapSingleton.maps[playerPositionData.mapId].GetHeight(newPos);

            //playerPositionData.movementData.position.x = newPositionX;
            //playerPositionData.movementData.position.y = newPositionY;
            //playerPositionData.movementData.position.z = height;
            //CharacterUtils::InvalidatePosition(singleton, playerConnection, playerPositionData);
        }
        else if (spellData.Effect[i] == SPELL_EFFECT_APPLY_AURA)
        {
            if (!auraList.HasAura(spellData.Id))
                auraList.ApplyAura(playerConnectionComponent->characterGuid, spellData);
        }

        SpellEffectHooks::CallHook(SpellEffectHooks::Hooks::HOOK_ON_SPELL_EFFECT_AFTER_HIT, spellData.Effect[i], &asPlayer, &asSpell);
    }

    SpellHooks::CallHook(SpellHooks::Hooks::HOOK_ON_SPELL_FINISH_CAST, &asPlayer, &asSpell); // TODO: Move this to a proper place after we have support for non-instant spells - MPursche

    buffer->Reset();
    buffer->PutGuid(playerConnectionComponent->characterGuid);
    buffer->PutGuid(playerConnectionComponent->characterGuid);
    buffer->PutU8(0); // CastCount
    buffer->PutU32(spellId);
    buffer->PutU32(0x00000002);
    buffer->PutU32(0);
    buffer->PutU32(0);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, buffer, Opcode::SMSG_SPELL_START);

    buffer->Reset();
    buffer->PutGuid(playerConnectionComponent->characterGuid);
    buffer->PutGuid(playerConnectionComponent->characterGuid);
    buffer->PutU8(0); // CastCount
    buffer->PutU32(spellId);
    buffer->PutU32(0x00000100);
    buffer->PutU32(static_cast<u32>(singleton.lifeTimeInMS));

    buffer->PutU8(1);                                         // Affected Targets
    buffer->PutU64(playerConnectionComponent->characterGuid); // Target GUID
    buffer->PutU8(0);                                         // Resisted Targets

    if (targetFlags == 0) // SELF
    {
        targetFlags = 0x02; // UNIT
    }
    buffer->PutU32(targetFlags); // Target Flags
    buffer->PutU8(0);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, buffer, Opcode::SMSG_SPELL_GO);

    buffer->Reset();
    buffer->PutGuid(playerConnectionComponent->characterGuid);
    buffer->PutU8(0); // CastCount
    buffer->PutU32(spellId);
    buffer->PutU8(0); // Result
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, buffer, Opcode::SMSG_SPELL_FAILURE);
    CharacterUtils::SendPacketToGridPlayers(registry, playerConnectionComponent->entityId, buffer, Opcode::SMSG_SPELL_FAILED_OTHER);

    return true;
}

bool CharacterPacketUtils::HandleCancelAura(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::CancelAura", tracy::Color::Orange2)

        entt::registry* registry = ServiceLocator::GetMainRegistry();

    u32 spellId = 0;
    packet->data->GetU32(spellId);

    AuraListComponent& auraList = registry->get<AuraListComponent>(playerConnectionComponent->entityId);
    auraList.RemoveAura(spellId);
    return true;
}

bool CharacterPacketUtils::HandleContactList(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    return true;
}