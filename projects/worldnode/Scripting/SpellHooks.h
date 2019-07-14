#pragma once
#include <Utils/DebugHandler.h>
#include "Any.h"
#include "ScriptEngine.h"
#include "AngelBinder.h"
#include <array>
#include <memory>

class SpellHooks
{
public:
    enum Hooks
    {
        HOOK_ONSPELLCAST,

        COUNT
    };

    inline static void Register(Hooks id, asIScriptFunction* func)
    {
        _hooks[id].push_back(func);
    }

    inline static std::vector<asIScriptFunction*>& GetHooks(Hooks id)
    {
        return _hooks[id];
    }

    template <typename... Args>
    inline static void CallHook(Hooks id, Args... args)
    {
        ScriptEngine::CallHook(_hooks, id, args...);
    }

    inline static void ClearHooks()
    {
        for (size_t i = 0; i < Hooks::COUNT; i++)
        {
            for (auto function : _hooks[i])
            {
                function->Release();
            }
            _hooks[i].clear();
        }
    }

private:
    static std::array<std::vector<asIScriptFunction*>, Hooks::COUNT> _hooks;
};