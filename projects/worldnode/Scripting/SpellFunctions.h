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

#include "SpellHooks.h"

#include "ScriptEngine.h"
#include <entt.hpp>

class AngelScriptSpellTemplate
{
public:
    AngelScriptSpellTemplate() {}

public:
    // Data storage
    void GetData(std::string& key, void* ref, int typeId) const;
    void SetData(std::string& key, void* ref, int typeId) const;

private:
};

class AngelScriptSpell
{
public:
    AngelScriptSpell() {}

public:
    // Data storage
    void GetData(std::string& key, void* ref, int typeId) const;
    void SetData(std::string& key, void* ref, int typeId) const;

    AngelScriptSpellTemplate* GetTemplate() { return _template; }

private:
    AngelScriptSpellTemplate* _template;
};

namespace GlobalFunctions
{
    inline void RegisterSpellCallback(u32 callbackId, asIScriptFunction* callback)
    {
        SpellHooks::Register(static_cast<SpellHooks::Hooks>(callbackId), callback);
    }
} // namespace GlobalFunctions

inline void RegisterSpellFunctions(AB_NAMESPACE_QUALIFIER Engine* engine)
{
    // Register Spell types
    engine->asEngine()->RegisterObjectType("SpellTemplate", sizeof(AngelScriptSpellTemplate), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<AngelScriptSpellTemplate>());
    engine->RegisterObjectHash<AngelScriptSpellTemplate*>("SpellTemplate"_h);
    engine->asEngine()->RegisterObjectType("Spell", sizeof(AngelScriptSpell), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<AngelScriptSpell>());
    engine->RegisterObjectHash<AngelScriptSpell*>("Spell"_h);

    // Register functions for Player type
    engine->asEngine()->RegisterObjectMethod("Spell", "void GetData(string key, ?&out)", asMETHOD(AngelScriptSpell, GetData), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Spell", "void SetData(string key, ?&in)", asMETHOD(AngelScriptSpell, SetData), asCALL_THISCALL);

    engine->asEngine()->RegisterObjectMethod("Spell", "SpellTemplate GetTemplate()", asMETHOD(AngelScriptSpell, GetTemplate), asCALL_THISCALL);

    // Register hooks
    engine->asEngine()->RegisterFuncdef("void SpellCastCallback(Player, Spell)");
    engine->asEngine()->RegisterGlobalFunction("void RegisterSpellCallback(uint32 id, SpellCastCallback @cb)", asFUNCTION(GlobalFunctions::RegisterSpellCallback), asCALL_CDECL);
}
