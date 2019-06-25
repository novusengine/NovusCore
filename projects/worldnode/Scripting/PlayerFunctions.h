#pragma once
#include <Utils/DebugHandler.h>
#include <Config/ConfigHandler.h>
#include "AngelBinder.h"

#include "PlayerHooks.h"

#include "ScriptEngine.h"
#include <entt.hpp>

#include "../ECS/Components/PlayerUpdateDataComponent.h"
#include "../ECS/Components/PlayerPositionComponent.h"

class AngelScriptPlayer
{
public:
    AngelScriptPlayer(){}
    AngelScriptPlayer(u32 entityId, entt::registry* registry)
    {
        _entityId = entityId;
        _registry = registry;
    }

public:
    

private:
    u32 _entityId;
    entt::registry* _registry;
};

namespace GlobalFunctions
{
    inline void RegisterPlayerCallback(u32 callbackId, asIScriptFunction* callback)
    {
        NC_LOG_MESSAGE("Register Callback!");
        PlayerHooks::Register(static_cast<PlayerHooks::Hooks>(callbackId), callback);
    }
}

inline void RegisterComponents(AB_NAMESPACE_QUALIFIER Engine* engine)
{
    engine->asEngine()->RegisterObjectType("PlayerPositionComponent", 0, asOBJ_REF | asOBJ_NOCOUNT);
    engine->asEngine()->RegisterObjectProperty("PlayerPositionComponent", "const uint32 mapId", asOFFSET(PlayerPositionComponent, mapId));
    engine->asEngine()->RegisterObjectProperty("PlayerPositionComponent", "const float x", asOFFSET(PlayerPositionComponent, x));
    engine->asEngine()->RegisterObjectProperty("PlayerPositionComponent", "const float y", asOFFSET(PlayerPositionComponent, y));
    engine->asEngine()->RegisterObjectProperty("PlayerPositionComponent", "const float z", asOFFSET(PlayerPositionComponent, z));
    engine->asEngine()->RegisterObjectProperty("PlayerPositionComponent", "const float orientation", asOFFSET(PlayerPositionComponent, orientation));
    engine->asEngine()->RegisterObjectProperty("PlayerPositionComponent", "const uint32 adtId", asOFFSET(PlayerPositionComponent, adtId));
}

inline void RegisterPlayerFunctions(AB_NAMESPACE_QUALIFIER Engine* engine)
{
    RegisterComponents(engine);

    // Register Player type
    engine->asEngine()->RegisterObjectType("Player", sizeof(AngelScriptPlayer), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<AngelScriptPlayer>());

    // Register functions for Player type
    //engine->asEngine()->RegisterObjectMethod("Player", "PlayerPositionComponent@ GetPositionComponent()", asMETHOD(AngelScriptPlayer, GetPositionComponent), asCALL_THISCALL);
    //engine->asEngine()->RegisterObjectMethod("Player", "const PlayerPositionComponent@ GetPositionComponentConst()", asMETHOD(AngelScriptPlayer, GetPositionComponentConst), asCALL_THISCALL);

    // Register hooks
    engine->asEngine()->RegisterFuncdef("void PlayerCallback(Player)");
    engine->asEngine()->RegisterGlobalFunction("void RegisterPlayerCallback(uint32 id, PlayerCallback @cb)", asFUNCTION(GlobalFunctions::RegisterPlayerCallback), asCALL_CDECL);

}
