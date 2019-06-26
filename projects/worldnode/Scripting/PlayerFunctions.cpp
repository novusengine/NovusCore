#include "PlayerFunctions.h"

#include "../Utils/CharacterUtils.h"

#include "../ECS/Components/Singletons/ScriptSingleton.h"

u32 AngelScriptPlayer::GetMapId() const
{
    return _registry->get<PlayerPositionComponent>(_entityId).mapId;
}

u32 AngelScriptPlayer::GetAdtId() const
{
    return _registry->get<PlayerPositionComponent>(_entityId).adtId;
}

Vector3 AngelScriptPlayer::GetPosition() const
{
    PlayerPositionComponent& positionComponent = _registry->get<PlayerPositionComponent>(_entityId);
    return Vector3(positionComponent.x, positionComponent.y, positionComponent.z);
}

void AngelScriptPlayer::SetPosition(Vector3 pos, bool immediate)
{
    PlayerPositionComponent& positionComponent = _registry->get<PlayerPositionComponent>(_entityId);

    if (immediate)
    {
        positionComponent.x = pos.x;
        positionComponent.y = pos.y;
        positionComponent.z = pos.z;
        CharacterUtils::InvalidatePosition(_registry, _entityId);
    }
    else
    {
        entt::registry* registry = _registry;
        u32 entityId = _entityId;

        _registry->ctx<ScriptSingleton>().AddTransaction([&positionComponent, &registry, pos, entityId]() {
            positionComponent.x = pos.x;
            positionComponent.y = pos.y;
            positionComponent.z = pos.z;
            CharacterUtils::InvalidatePosition(registry, entityId);
            });
    }
}

f32 AngelScriptPlayer::GetOrientation() const
{
    return _registry->get<PlayerPositionComponent>(_entityId).orientation;
}

void AngelScriptPlayer::SetOrientation(f32 orientation, bool immediate)
{
    PlayerPositionComponent& positionComponent = _registry->get<PlayerPositionComponent>(_entityId);

    if (immediate)
    {
        positionComponent.orientation = orientation;
    }
    else
    {
        _registry->ctx<ScriptSingleton>().AddTransaction([&positionComponent, orientation]() {
            positionComponent.orientation = orientation;
            });
    }
}