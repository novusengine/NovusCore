#include "PlayerFunctions.h"

#include "../Utils/CharacterUtils.h"
#include "../ECS/Components/Singletons/ScriptSingleton.h"
#include "../ECS/Components/ScriptDataStorageComponent.h"
#include "../ECS/Components/PlayerFieldDataComponent.h"

#include <Utils/StringUtils.h>
#include "../Utils/ServiceLocator.h"

void AngelScriptPlayer::GetData(std::string& key, void* ref, int typeId) const
{
    u32 keyHash = StringUtils::fnv1a_32(key.c_str(), key.size());
    ScriptDataStorageComponent& dataStorageComponent = ServiceLocator::GetMainRegistry()->get<ScriptDataStorageComponent>(_entityId);

    size_t size = 0;
    switch (typeId)
    {
    // 8 bits
    case asTYPEID_BOOL:
    case asTYPEID_INT8:
    case asTYPEID_UINT8:
        size = 1;
        break;
    // 16 bits
    case asTYPEID_INT16:
    case asTYPEID_UINT16:
        size = 2;
        break;
    // 32 bits
    case asTYPEID_INT32:
    case asTYPEID_UINT32:
    case asTYPEID_FLOAT:
        size = 4;
        break;
    // 64 bits
    case asTYPEID_INT64:
    case asTYPEID_UINT64:
    case asTYPEID_DOUBLE:
        size = 8;
        break;
    default:
        assert(false); // Unsupported datatype
        break;
    }

    memcpy(ref, &dataStorageComponent.data[keyHash], size);
}

void AngelScriptPlayer::SetData(std::string& key, void* ref, int typeId) const
{
    u32 keyHash = StringUtils::fnv1a_32(key.c_str(), key.size());
    ScriptDataStorageComponent& dataStorageComponent = ServiceLocator::GetMainRegistry()->get<ScriptDataStorageComponent>(_entityId);

    u64 value;

    switch (typeId)
    {
    // 8 bits
    case asTYPEID_BOOL:
    case asTYPEID_INT8:
    case asTYPEID_UINT8:
        value = *static_cast<u8*>(ref);
        dataStorageComponent.data[keyHash] = value;
        break;
    // 16 bits
    case asTYPEID_INT16:
    case asTYPEID_UINT16:
        value = *static_cast<u16*>(ref);
        dataStorageComponent.data[keyHash] = value;
        break;
    // 32 bits
    case asTYPEID_INT32:
    case asTYPEID_UINT32:
    case asTYPEID_FLOAT:
        value = *static_cast<u32*>(ref);
        dataStorageComponent.data[keyHash] = value;
        break;
    // 64 bits
    case asTYPEID_INT64:
    case asTYPEID_UINT64:
    case asTYPEID_DOUBLE:
        value = *static_cast<u64*>(ref);
        dataStorageComponent.data[keyHash] = value;
        break;
    default:
        assert(false); // Unsupported datatype
        break;
    }
}

u32 AngelScriptPlayer::GetMapId() const
{
    return ServiceLocator::GetMainRegistry()->get<PlayerPositionComponent>(_entityId).mapId;
}

u32 AngelScriptPlayer::GetAdtId() const
{
    return ServiceLocator::GetMainRegistry()->get<PlayerPositionComponent>(_entityId).adtId;
}

Vector3 AngelScriptPlayer::GetPosition() const
{
    PlayerPositionComponent& positionComponent = ServiceLocator::GetMainRegistry()->get<PlayerPositionComponent>(_entityId);
    return Vector3(positionComponent.movementData.position.x, positionComponent.movementData.position.y, positionComponent.movementData.position.z);
}

void AngelScriptPlayer::SetPosition(Vector3 pos, bool immediate)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerPositionComponent& positionComponent = registry->get<PlayerPositionComponent>(_entityId);

    if (immediate)
    {
        positionComponent.movementData.position.x = pos.x;
        positionComponent.movementData.position.y = pos.y;
        positionComponent.movementData.position.z = pos.z;
        CharacterUtils::InvalidatePosition(registry, _entityId);
    }
    else
    {
        u32 entityId = _entityId;

        registry->ctx<ScriptSingleton>().AddTransaction([&positionComponent, registry, pos, entityId]() {
            positionComponent.movementData.position.x = pos.x;
            positionComponent.movementData.position.y = pos.y;
            positionComponent.movementData.position.z = pos.z;
            CharacterUtils::InvalidatePosition(registry, entityId);
        });
    }
}

f32 AngelScriptPlayer::GetOrientation() const
{
    return ServiceLocator::GetMainRegistry()->get<PlayerPositionComponent>(_entityId).movementData.orientation;
}

void AngelScriptPlayer::SetOrientation(f32 orientation, bool immediate)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerPositionComponent& positionComponent = registry->get<PlayerPositionComponent>(_entityId);

    if (immediate)
    {
        positionComponent.movementData.orientation = orientation;
    }
    else
    {
        registry->ctx<ScriptSingleton>().AddTransaction([&positionComponent, orientation]() {
            positionComponent.movementData.orientation = orientation;
        });
    }
}

i32 AngelScriptPlayer::GetDisplayId() const
{
    return ServiceLocator::GetMainRegistry()->get<PlayerFieldDataComponent>(_entityId).GetFieldValue<i32>(UNIT_FIELD_DISPLAYID);
}

void AngelScriptPlayer::SetDisplayId(i32 displayId, bool immediate)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerFieldDataComponent& playerFieldData = registry->get<PlayerFieldDataComponent>(_entityId);

    if (immediate)
    {
        playerFieldData.SetFieldValue<i32>(UNIT_FIELD_DISPLAYID, displayId);
    }
    else
    {
        registry->ctx<ScriptSingleton>().AddTransaction([&playerFieldData, displayId]() {
            playerFieldData.SetFieldValue<i32>(UNIT_FIELD_DISPLAYID, displayId);
        });
    }
}

i32 AngelScriptPlayer::GetNativeDisplayId() const
{
    return ServiceLocator::GetMainRegistry()->get<PlayerFieldDataComponent>(_entityId).GetFieldValue<i32>(UNIT_FIELD_NATIVEDISPLAYID);
}

void AngelScriptPlayer::SetNativeDisplayId(i32 displayId, bool immediate)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerFieldDataComponent& playerFieldData = registry->get<PlayerFieldDataComponent>(_entityId);

    if (immediate)
    {
        playerFieldData.SetFieldValue<i32>(UNIT_FIELD_NATIVEDISPLAYID, displayId);
    }
    else
    {
        registry->ctx<ScriptSingleton>().AddTransaction([&playerFieldData, displayId]() {
            playerFieldData.SetFieldValue<i32>(UNIT_FIELD_NATIVEDISPLAYID, displayId);
        });
    }
}

i32 AngelScriptPlayer::GetMountDisplayId() const
{
    return ServiceLocator::GetMainRegistry()->get<PlayerFieldDataComponent>(_entityId).GetFieldValue<i32>(UNIT_FIELD_MOUNTDISPLAYID);
}

void AngelScriptPlayer::SetMountDisplayId(i32 displayId, bool immediate)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerFieldDataComponent& playerFieldData = registry->get<PlayerFieldDataComponent>(_entityId);

    if (immediate)
    {
        playerFieldData.SetFieldValue<i32>(UNIT_FIELD_MOUNTDISPLAYID, displayId);
    }
    else
    {
        registry->ctx<ScriptSingleton>().AddTransaction([&playerFieldData, displayId]() {
            playerFieldData.SetFieldValue<i32>(UNIT_FIELD_MOUNTDISPLAYID, displayId);
        });
    }
}

void AngelScriptPlayer::SetFlyMode(bool state, bool immediate)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerConnectionComponent& playerConnection = registry->get<PlayerConnectionComponent>(_entityId);

    if (immediate)
    {
        std::shared_ptr<ByteBuffer> flyMode = ByteBuffer::Borrow<12>();
        CharacterUtils::BuildFlyModePacket(playerConnection.characterGuid, flyMode);
        playerConnection.socket->SendPacket(flyMode.get(), state ? Opcode::SMSG_MOVE_SET_CAN_FLY : Opcode::SMSG_MOVE_UNSET_CAN_FLY);
    }
    else
    {
        registry->ctx<ScriptSingleton>().AddTransaction([&playerConnection, state]() {
            std::shared_ptr<ByteBuffer> flyMode = ByteBuffer::Borrow<12>();
            CharacterUtils::BuildFlyModePacket(playerConnection.characterGuid, flyMode);
            playerConnection.socket->SendPacket(flyMode.get(), state ? Opcode::SMSG_MOVE_SET_CAN_FLY : Opcode::SMSG_MOVE_UNSET_CAN_FLY);
        });
    }
}

void AngelScriptPlayer::SetSpeed(f32 speed, bool immediate)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerConnectionComponent& playerConnection = registry->get<PlayerConnectionComponent>(_entityId);

    if (speed <= 0)
    {
        speed = 0;
    }
    else
    {
        speed /= 100;
    }

    if (immediate)
    {
        std::shared_ptr<ByteBuffer> speedChange = ByteBuffer::Borrow<12>();

        CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_WALK_SPEED_CHANGE, speedChange);
        CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_WALK_SPEED_CHANGE);
        speedChange->Reset();

        CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_RUN_SPEED_CHANGE, speedChange);
        CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_RUN_SPEED_CHANGE);
        speedChange->Reset();

        CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE, speedChange);
        CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE);
        speedChange->Reset();

        CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE, speedChange);
        CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE);
        speedChange->Reset();

        CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE, speedChange);
        CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE);
        speedChange->Reset();

        CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE, speedChange);
        CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE);
        speedChange->Reset();

        CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE, speedChange);
        CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE);
    }
    else
    {
        registry->ctx<ScriptSingleton>().AddTransaction([registry, &playerConnection, speed]() {
            std::shared_ptr<ByteBuffer> speedChange = ByteBuffer::Borrow<12>();

            CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_WALK_SPEED_CHANGE, speedChange);
            CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_WALK_SPEED_CHANGE);
            speedChange->Reset();

            CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_RUN_SPEED_CHANGE, speedChange);
            CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_RUN_SPEED_CHANGE);
            speedChange->Reset();

            CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE, speedChange);
            CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_RUN_BACK_SPEED_CHANGE);
            speedChange->Reset();

            CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE, speedChange);
            CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_SWIM_SPEED_CHANGE);
            speedChange->Reset();

            CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE, speedChange);
            CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_SWIM_BACK_SPEED_CHANGE);
            speedChange->Reset();

            CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE, speedChange);
            CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_FLIGHT_SPEED_CHANGE);
            speedChange->Reset();

            CharacterUtils::BuildSpeedChangePacket(playerConnection.characterGuid, speed, Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE, speedChange);
            CharacterUtils::SendPacketToGridPlayers(registry, playerConnection.entityId, speedChange, Opcode::SMSG_FORCE_FLIGHT_BACK_SPEED_CHANGE);
        });
    }
}

void AngelScriptPlayer::SendChatMessage(std::string msg)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerConnectionComponent& connectionComponent = registry->get<PlayerConnectionComponent>(_entityId);

    registry->ctx<ScriptSingleton>().AddTransaction([&connectionComponent, msg]() {
        connectionComponent.SendChatNotification(msg);
    });
}