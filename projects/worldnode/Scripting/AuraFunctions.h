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
#include <Utils/DebugHandler.h>
#include <Config/ConfigHandler.h>
#include "AngelBinder.h"

#include "AuraHooks.h"

#include "ScriptEngine.h"
#include <entt.hpp>

struct Aura;
class AngelScriptAura
{
public:
    AngelScriptAura(Aura* aura) : _aura(aura) {}

public:
    // Data storage
    void GetData(std::string& key, void* ref, int typeId) const;
    void SetData(std::string& key, void* ref, int typeId) const;

    Aura* GetAura() { return _aura; }

private:
    Aura* _aura;
};

namespace GlobalFunctions
{
inline void RegisterAuraCallback(u32 callbackId, asIScriptFunction* callback)
{
    AuraHooks::Register(static_cast<AuraHooks::Hooks>(callbackId), callback);
}
} // namespace GlobalFunctions

inline void RegisterAuraFunctions(AB_NAMESPACE_QUALIFIER Engine* engine)
{
    // Register Aura types
    engine->asEngine()->RegisterObjectType("Aura", sizeof(AngelScriptAura), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<AngelScriptAura>());

    // Register functions for Aura type
    engine->asEngine()->RegisterObjectMethod("Aura", "void GetData(string key, ?&out)", asMETHOD(AngelScriptAura, GetData), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Aura", "void SetData(string key, ?&in)", asMETHOD(AngelScriptAura, SetData), asCALL_THISCALL);

    // Register hooks
    engine->asEngine()->RegisterFuncdef("void AuraAppliedCallback(Player, Aura)");
    engine->asEngine()->RegisterGlobalFunction("void RegisterAuraCallback(uint32 id, AuraAppliedCallback @cb)", asFUNCTION(GlobalFunctions::RegisterAuraCallback), asCALL_CDECL);
}
