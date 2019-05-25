#include "PlayerFunctions.h"

PlayerPositionComponent& AngelScriptPlayer::GetPositionComponent()
{
    return _registry->get<PlayerPositionComponent>(_entityId);
}

const PlayerPositionComponent& AngelScriptPlayer::GetPositionComponentConst()
{
    return _registry->get<PlayerPositionComponent>(_entityId);
}