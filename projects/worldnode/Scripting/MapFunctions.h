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

#include "MapHooks.h"

#include "ScriptEngine.h"
#include <entt.hpp>

class AngelScriptMap
{
public:
    AngelScriptMap() {}

    AngelScriptMap(u32 mapId)
    {
        _mapId = mapId;
    }

public:
    // Data storage
    void GetData(std::string& key, void* ref, int typeId) const;
    void SetData(std::string& key, void* ref, int typeId) const;

    u32 GetId() { return _mapId; }
    std::string GetName() const;

    f32 GetHeight(Vector2& pos) const;

private:
    u32 _mapId;
};

namespace GlobalFunctions
{
    inline void RegisterMapCallback(u32 callbackId, asIScriptFunction* callback)
    {
        MapHooks::Register(static_cast<MapHooks::Hooks>(callbackId), callback);
    }
} // namespace GlobalFunctions

inline void RegisterMapFunctions(AB_NAMESPACE_QUALIFIER Engine* engine)
{
    // Register Map type
    engine->asEngine()->RegisterObjectType("Map", sizeof(AngelScriptMap), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<AngelScriptMap>());
    engine->RegisterObjectHash<AngelScriptMap*>("Map"_h);

    // Register functions for Map type
    engine->asEngine()->RegisterObjectMethod("Map", "void GetData(string key, ?&out)", asMETHOD(AngelScriptMap, GetData), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Map", "void SetData(string key, ?&in)", asMETHOD(AngelScriptMap, SetData), asCALL_THISCALL);

    engine->asEngine()->RegisterObjectMethod("Map", "uint32 GetId()", asMETHOD(AngelScriptMap, GetId), asCALL_THISCALL);
    engine->asEngine()->RegisterObjectMethod("Map", "string GetName()", asMETHOD(AngelScriptMap, GetName), asCALL_THISCALL);

    engine->asEngine()->RegisterObjectMethod("Map", "float GetHeight(Vector2)", asMETHOD(AngelScriptMap, GetHeight), asCALL_THISCALL);

    // Register hooks
    engine->asEngine()->RegisterFuncdef("void MapCallback(Map)");
    engine->asEngine()->RegisterGlobalFunction("void RegisterMapCallback(uint32 id, MapCallback @cb)", asFUNCTION(GlobalFunctions::RegisterMapCallback), asCALL_CDECL);
}
