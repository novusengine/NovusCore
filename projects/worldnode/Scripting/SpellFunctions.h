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
    engine->asEngine()->RegisterObjectType("SpellTemplate", sizeof(AngelScriptSpell), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<AngelScriptSpellTemplate>());
    engine->asEngine()->RegisterObjectType("Spell", sizeof(AngelScriptSpell), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<AngelScriptSpell>());

    // Register functions for Player type
    engine->asEngine()->RegisterObjectMethod("Spell", "void GetData(string key, ?&out)", asMETHOD(AngelScriptSpell, GetData), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Spell", "void SetData(string key, ?&in)", asMETHOD(AngelScriptSpell, SetData), asCALL_THISCALL);

    engine->asEngine()->RegisterObjectMethod("Spell", "SpellTemplate GetTemplate()", asMETHOD(AngelScriptSpell, GetTemplate), asCALL_THISCALL);

    // Register hooks
    engine->asEngine()->RegisterFuncdef("void SpellCastCallback(Player, Spell)");
    engine->asEngine()->RegisterGlobalFunction("void RegisterSpellCallback(uint32 id, SpellCastCallback @cb)", asFUNCTION(GlobalFunctions::RegisterSpellCallback), asCALL_CDECL);
}
