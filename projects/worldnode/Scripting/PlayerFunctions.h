#pragma once
#include <Math/Vector3.h>
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
    AngelScriptPlayer() {}
    AngelScriptPlayer(u32 entityId, entt::registry* registry)
    {
        _entityId = entityId;
        _registry = registry;
    }

public:
    // Position
    u32 GetMapId() const;
    u32 GetAdtId() const;

    Vector3 GetPosition() const;
    void SetPosition(Vector3 pos, bool immediate);

    f32 GetOrientation() const;
    void SetOrientation(f32 orientation, bool immediate);

    // Chat
    void SendChatMessage(std::string msg);

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
} // namespace GlobalFunctions

inline void RegisterPlayerFunctions(AB_NAMESPACE_QUALIFIER Engine* engine)
{
    // Register Player type
    engine->asEngine()->RegisterObjectType("Player", sizeof(AngelScriptPlayer), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<AngelScriptPlayer>());

    // Register functions for Player type
    engine->asEngine()->RegisterObjectMethod("Player", "uint32 GetMapId()", asMETHOD(AngelScriptPlayer, GetMapId), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "uint32 GetAdtId()", asMETHOD(AngelScriptPlayer, GetAdtId), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "Vector3 GetPosition()", asMETHOD(AngelScriptPlayer, GetPosition), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetPosition(Vector3, bool = false)", asMETHOD(AngelScriptPlayer, SetPosition), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "float GetOrientation()", asMETHOD(AngelScriptPlayer, GetOrientation), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetOrientation(float, bool = false)", asMETHOD(AngelScriptPlayer, SetOrientation), asCALL_THISCALL);

    engine->asEngine()->RegisterObjectMethod("Player", "void SendChatMessage(string)", asMETHOD(AngelScriptPlayer, SendChatMessage), asCALL_THISCALL);

    // Register hooks
    engine->asEngine()->RegisterFuncdef("void PlayerCallback(Player)");
    engine->asEngine()->RegisterFuncdef("void PlayerChatCallback(Player, string)");
    engine->asEngine()->RegisterGlobalFunction("void RegisterPlayerCallback(uint32 id, PlayerCallback @cb)", asFUNCTION(GlobalFunctions::RegisterPlayerCallback), asCALL_CDECL);
    engine->asEngine()->RegisterGlobalFunction("void RegisterPlayerCallback(uint32 id, PlayerChatCallback @cb)", asFUNCTION(GlobalFunctions::RegisterPlayerCallback), asCALL_CDECL);
}
