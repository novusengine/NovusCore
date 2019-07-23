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
#include "Any.h"
#include "ScriptEngine.h"
#include "AngelBinder.h"
#include <array>
#include <memory>

class AuraHooks
{
public:
    enum Hooks
    {
        HOOK_ON_AURA_APPLIED,

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