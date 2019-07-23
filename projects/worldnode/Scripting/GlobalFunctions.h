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
#include "AngelBinder.h"
#include <entt.hpp>

#include <Math/Vector2.h>
#include <Math/Vector3.h>

#include "../ECS/Components/PlayerUpdateDataComponent.h"
#include "../ECS/Components/PlayerPositionComponent.h"

class ASVector2 : public Vector2
{
public:
    ASVector2() : Vector2() { _refCount = 1; }
    ASVector2(f32 inX, f32 inY) : Vector2(inX, inY) { _refCount = 1; }
    ASVector2(f32 in) : Vector2(in) { _refCount = 1; }
    ASVector2(u8 inX, u8 inY) : Vector2(inX, inY) { _refCount = 1; }
    ASVector2(u8 in) : Vector2(in) { _refCount = 1; }
    ASVector2(u16 inX, u16 inY) : Vector2(inX, inY) { _refCount = 1; }
    ASVector2(u16 in) : Vector2(in) { _refCount = 1; }
    ASVector2(u32 inX, u32 inY) : Vector2(inX, inY) { _refCount = 1; }
    ASVector2(u32 in) : Vector2(in) { _refCount = 1; }
    ASVector2(i8 inX, i8 inY) : Vector2(inX, inY) { _refCount = 1; }
    ASVector2(i8 in) : Vector2(in) { _refCount = 1; }
    ASVector2(i16 inX, i16 inY) : Vector2(inX, inY) { _refCount = 1; }
    ASVector2(i16 in) : Vector2(in) { _refCount = 1; }
    ASVector2(i32 inX, i32 inY) : Vector2(inX, inY) { _refCount = 1; }
    ASVector2(i32 in) : Vector2(in) { _refCount = 1; }
    ASVector2(Vector2& in) : Vector2(in) { _refCount = 1; }
    ASVector2(ASVector2& in) : Vector2(in) { _refCount = 1; }

    void AddRef()
    {
        _refCount++;
    }

    void Release()
    {
        _refCount--;
        if (_refCount == 0)
            delete this;
    }

    static ASVector2* Factory()
    {
        return new ASVector2();
    }

private:
    size_t _refCount;
};

class ASVector3 : public Vector3
{
public:
    ASVector3() : Vector3() { _refCount = 1; }
    ASVector3(f32 inX, f32 inY, f32 inZ) : Vector3(inX, inY, inZ) { _refCount = 1; }
    ASVector3(f32 in) : Vector3(in) { _refCount = 1; }
    ASVector3(u8 inX, u8 inY, u8 inZ) : Vector3(inX, inY, inZ) { _refCount = 1; }
    ASVector3(u8 in) : Vector3(in) { _refCount = 1; }
    ASVector3(u16 inX, u16 inY, f32 inZ) : Vector3(inX, inY, inZ) { _refCount = 1; }
    ASVector3(u16 in) : Vector3(in) { _refCount = 1; }
    ASVector3(u32 inX, u32 inY, f32 inZ) : Vector3(inX, inY, inZ) { _refCount = 1; }
    ASVector3(u32 in) : Vector3(in) { _refCount = 1; }
    ASVector3(i8 inX, i8 inY, f32 inZ) : Vector3(inX, inY, inZ) { _refCount = 1; }
    ASVector3(i8 in) : Vector3(in) { _refCount = 1; }
    ASVector3(i16 inX, i16 inY, f32 inZ) : Vector3(inX, inY, inZ) { _refCount = 1; }
    ASVector3(i16 in) : Vector3(in) { _refCount = 1; }
    ASVector3(i32 inX, i32 inY, f32 inZ) : Vector3(inX, inY, inZ) { _refCount = 1; }
    ASVector3(i32 in) : Vector3(in) { _refCount = 1; }
    ASVector3(Vector3& in) : Vector3(in) { _refCount = 1; }
    ASVector3(Vector2& in) : Vector3(in) { _refCount = 1; }
    ASVector3(ASVector3& in) : Vector3(in) { _refCount = 1; }
    ASVector3(ASVector2& in) : Vector3(in) { _refCount = 1; }

    void AddRef()
    {
        _refCount++;
    }

    void Release()
    {
        _refCount--;
        if (_refCount == 0)
            delete this;
    }

    static ASVector3* Factory()
    {
        return new ASVector3();
    }

private:
    size_t _refCount;
};

namespace GlobalFunctions
{
entt::registry* _registry;
void SetRegistry(entt::registry* registry)
{
    _registry = registry;
}

inline void Print(std::string& message)
{
    NC_LOG_MESSAGE("[Script]: %s", message.c_str());
}
} // namespace GlobalFunctions

void RegisterGlobalFunctions(AB_NAMESPACE_QUALIFIER Engine* engine)
{
    // Register*Callback functions need to be registered manually since the binder does not support it

    //engine->asEngine()->RegisterGlobalFunction("void Print(string message)", asFUNCTION(GlobalFunctions::Print), asCALL_CDECL);
    //engine->asEngine()->RegisterGlobalFunction("PlayerPositionComponent@ GetComponent<PlayerPositionComponent>(uint32 entity)", asFUNCTION(GlobalFunctions::GetPlayerPositionComponent), asCALL_CDECL);

    AB_NAMESPACE_QUALIFIER Exporter::Export(*engine)
        [AB_NAMESPACE_QUALIFIER Exporter::Functions()
             .def("Print", &GlobalFunctions::Print)];
}
