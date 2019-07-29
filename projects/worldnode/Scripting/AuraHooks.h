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
#include "ScriptEngine.h"
#include "AngelBinder.h"
#include <array>
#include <memory>
#include <robin_hood.h>

class AuraHooks
{
public:
    enum Hooks
    {
        HOOK_ON_AURA_APPLIED,
        HOOK_ON_AURA_REMOVED,

        COUNT
    };

    inline static void Register(Hooks hook, asIScriptFunction* func)
    {
        _hooks[hook].push_back(func);
    }

    inline static std::vector<asIScriptFunction*>& GetHooks(Hooks hook)
    {
        return _hooks[hook];
    }

    template <typename... Args>
    inline static void CallHook(Hooks hook, Args... args)
    {
        ScriptEngine::CallHook<Hooks>(_hooks, hook, args...);
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

class AuraEffectHooks
{
public:
    enum Hooks
    {
        HOOK_ON_AURA_EFFECT_APPLIED,
        HOOK_ON_AURA_EFFECT_REMOVED,

        COUNT
    };

    inline static void Register(Hooks hook, u32 effectId, asIScriptFunction* func)
    {
#ifdef NC_Debug
        auto itr = _hooks.find(effectId);
        if (itr == _hooks.end())
        {
            NC_LOG_FATAL("Attempted to register AuraEffectHook (%u), for non-existant EffectID (%u)", hook, effectId);
        }
#endif

        _hooks[effectId][hook].push_back(func);
    }

    inline static std::vector<asIScriptFunction*>& GetHooks(Hooks hook, u32 effectId)
    {
#ifdef NC_Debug
        auto itr = _hooks.find(effectId);
        if (itr == _hooks.end())
        {
            NC_LOG_FATAL("Attempted to get AuraEffectHook (%u), for non-existant EffectID (%u)", hook, effectId);
        }
#endif

        return _hooks[effectId][hook];
    }
    inline static robin_hood::unordered_map<u32, std::array<std::vector<asIScriptFunction*>, Hooks::COUNT>>& GetEffectMap()
    {
        return _hooks;
    }

    template <typename... Args>
    inline static void CallHook(Hooks hook, u32 effectId, Args... args)
    {
#ifdef NC_Debug
        auto itr = _hooks.find(effectId);
        if (itr == _hooks.end())
        {
            NC_LOG_FATAL("Attempted to call AuraEffectHook (%u), for non-existant EffectID (%u)", hook, effectId);
        }
#endif

        ScriptEngine::CallHook<Hooks>(_hooks[effectId], hook, args...);
    }

    inline static void ClearHooks()
    {
        for (auto& itr : _hooks)
        {
            for (auto& vec : itr.second)
            {
                for (auto function : vec)
                {
                    function->Release();
                }
                vec.clear();
            }
        }
    }

private:
    static robin_hood::unordered_map<u32, std::array<std::vector<asIScriptFunction*>, Hooks::COUNT>> _hooks;
};