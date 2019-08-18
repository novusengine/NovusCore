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
#include <Math/Vector3.h>
#include <Utils/DebugHandler.h>
#include <Config/ConfigHandler.h>
#include "AngelBinder.h"

#include "PlayerHooks.h"

#include "ScriptEngine.h"

#include "../ECS/Components/PlayerUpdateDataComponent.h"
#include "../ECS/Components/PlayerPositionComponent.h"

class AngelScriptMap;

class AngelScriptPlayer
{
public:
    AngelScriptPlayer() {}
    AngelScriptPlayer(u32 entityId)
    {
        _entityId = entityId;
    }

public:
    // Data storage
    void GetData(std::string& key, void* ref, int typeId) const;
    void SetData(std::string& key, void* ref, int typeId) const;

    // Position
    u32 GetMapId() const;
    u32 GetAdtId() const;

    Vector3 GetPosition() const;
    void SetPosition(Vector3 pos, bool immediate);

    f32 GetOrientation() const;
    void SetOrientation(f32 orientation, bool immediate);

    i32 GetDisplayId() const;
    void SetDisplayId(i32 displayId, bool immediate);
    i32 GetNativeDisplayId() const;
    void SetNativeDisplayId(i32 displayId, bool immediate);
    i32 GetMountDisplayId() const;
    void SetMountDisplayId(i32 displayId, bool immediate);

    f32 GetHealth() const;
    f32 GetMaxHealth() const;
    void ModHealth(f32 modifier, bool immediate);
    void SetHealth(f32 health, bool immediate);
    
    f32 GetPower(u8 powerId) const;
    f32 GetMaxPower(u8 powerId) const;
    void ModPower(u8 powerId, f32 modifier, bool immediate);
    void SetPower(u8 powerId, f32 power, bool immediate);
    
    void SetFlyMode(bool state, bool immediate);
    void SetSpeed(f32 state, bool immediate);

    // Chat
    void SendChatMessage(std::string msg);

private:
    u32 _entityId;
};

namespace GlobalFunctions
{
inline void RegisterPlayerCallback(u32 callbackId, asIScriptFunction* callback)
{
    PlayerHooks::Register(static_cast<PlayerHooks::Hooks>(callbackId), callback);
}
} // namespace GlobalFunctions

inline void RegisterPlayerFunctions(AB_NAMESPACE_QUALIFIER Engine* engine)
{
    // Register Player type
    engine->asEngine()->RegisterObjectType("Player", sizeof(AngelScriptPlayer), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<AngelScriptPlayer>());
    engine->RegisterObjectHash<AngelScriptPlayer*>("Player"_h);

    // Register functions for Player type
    engine->asEngine()->RegisterObjectMethod("Player", "void GetData(string key, ?&out)", asMETHOD(AngelScriptPlayer, GetData), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetData(string key, ?&in)", asMETHOD(AngelScriptPlayer, SetData), asCALL_THISCALL);

    engine->asEngine()->RegisterObjectMethod("Player", "uint32 GetMapId()", asMETHOD(AngelScriptPlayer, GetMapId), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "uint32 GetAdtId()", asMETHOD(AngelScriptPlayer, GetAdtId), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "Vector3 GetPosition()", asMETHOD(AngelScriptPlayer, GetPosition), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetPosition(Vector3, bool = false)", asMETHOD(AngelScriptPlayer, SetPosition), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "float GetOrientation()", asMETHOD(AngelScriptPlayer, GetOrientation), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetOrientation(float, bool = false)", asMETHOD(AngelScriptPlayer, SetOrientation), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "int32 GetDisplayId()", asMETHOD(AngelScriptPlayer, GetDisplayId), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetDisplayId(int32, bool = false)", asMETHOD(AngelScriptPlayer, SetDisplayId), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "int32 GetNativeDisplayId()", asMETHOD(AngelScriptPlayer, GetNativeDisplayId), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetNativeDisplayId(int32, bool = false)", asMETHOD(AngelScriptPlayer, SetNativeDisplayId), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "int32 GetMountDisplayId()", asMETHOD(AngelScriptPlayer, GetMountDisplayId), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetMountDisplayId(int32, bool = false)", asMETHOD(AngelScriptPlayer, SetMountDisplayId), asCALL_THISCALL);

    engine->asEngine()->RegisterObjectMethod("Player", "float GetHealth()", asMETHOD(AngelScriptPlayer, GetHealth), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "float GetMaxHealth()", asMETHOD(AngelScriptPlayer, GetMaxHealth), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void ModHealth(float, bool = false)", asMETHOD(AngelScriptPlayer, ModHealth), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetHealth(float, bool = false)", asMETHOD(AngelScriptPlayer, SetHealth), asCALL_THISCALL);

    engine->asEngine()->RegisterObjectMethod("Player", "float GetPower(uint8)", asMETHOD(AngelScriptPlayer, GetPower), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "float GetMaxPower(uint8)", asMETHOD(AngelScriptPlayer, GetMaxPower), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void ModPower(uint8, float, bool = false)", asMETHOD(AngelScriptPlayer, ModPower), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetPower(uint8, float, bool = false)", asMETHOD(AngelScriptPlayer, SetPower), asCALL_THISCALL);

    engine->asEngine()->RegisterObjectMethod("Player", "void SetFlyMode(bool, bool = false)", asMETHOD(AngelScriptPlayer, SetFlyMode), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Player", "void SetSpeed(float, bool = false)", asMETHOD(AngelScriptPlayer, SetSpeed), asCALL_THISCALL);

    engine->asEngine()->RegisterObjectMethod("Player", "void SendChatMessage(string)", asMETHOD(AngelScriptPlayer, SendChatMessage), asCALL_THISCALL);

    // Register hooks
    engine->asEngine()->RegisterFuncdef("void PlayerCallback(Player)");
    engine->asEngine()->RegisterFuncdef("void PlayerChatCallback(Player, string)");
    engine->asEngine()->RegisterGlobalFunction("void RegisterPlayerCallback(uint32 id, PlayerCallback @cb)", asFUNCTION(GlobalFunctions::RegisterPlayerCallback), asCALL_CDECL);
    engine->asEngine()->RegisterGlobalFunction("void RegisterPlayerCallback(uint32 id, PlayerChatCallback @cb)", asFUNCTION(GlobalFunctions::RegisterPlayerCallback), asCALL_CDECL);
}
