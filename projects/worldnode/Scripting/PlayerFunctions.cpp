#include "PlayerFunctions.h"

#include "../Utils/CharacterUtils.h"
#include "../ECS/Components/Singletons/ScriptSingleton.h"
#include "../ECS/Components/ScriptDataStorageComponent.h"

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

void AngelScriptPlayer::SendChatMessage(std::string msg)
{
    entt::registry* registry = ServiceLocator::GetMainRegistry();
    PlayerConnectionComponent& connectionComponent = registry->get<PlayerConnectionComponent>(_entityId);

    registry->ctx<ScriptSingleton>().AddTransaction([&connectionComponent, msg]() {
        connectionComponent.SendChatNotification(msg);
    });
}