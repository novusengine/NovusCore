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
#include <functional>
#include <memory>
#include <vector>
#include <Utils/SharedPool.h>
#include "AngelBinder.h"
#include <asio.hpp>
#include <any>

typedef void(func_t)(AB_NAMESPACE_QUALIFIER Engine*);

class NovusConnection;
class AngelScriptSpell;
class ScriptEngine
{
public:
    static func_t* GetRegisterFunction()
    {
        return _registerFunction;
    }
    static void SetRegisterFunction(func_t* func) { _registerFunction = func; }

    static AB_NAMESPACE_QUALIFIER Engine* GetScriptEngine();
    static AB_NAMESPACE_QUALIFIER Context* GetScriptContext();
    static asio::io_service* GetIOService() { return _ioService; }
    static void SetIOService(asio::io_service* service) { _ioService = service; }

    static void RegisterNovusConnection(NovusConnection* connection);

    template <typename Hooks, typename... Args>
    inline static void CallHook(std::array<std::vector<asIScriptFunction*>, Hooks::COUNT>& hooks, Hooks id, Args... args)
    {
        std::vector<std::any> arguments = {args...};

        for (auto function : hooks[id])
        {
            AB_NAMESPACE_QUALIFIER Context* context = ScriptEngine::GetScriptContext();
            if (context)
            {
                context->prepare(function);

                assert(arguments.size() == function->GetParamCount());

                int i = 0;
                for (auto argument : arguments)
                {
                    int paramTypeId;
                    function->GetParam(i++, &paramTypeId);

                    const char* paramName = nullptr;
                    if (paramTypeId <= asTYPEID_DOUBLE)
                    {
                        switch (paramTypeId)
                        {
                        case asTYPEID_BOOL:
                            paramName = "bool";
                            break;
                        case asTYPEID_INT8:
                            paramName = "i8";
                            break;
                        case asTYPEID_INT16:
                            paramName = "i16";
                            break;
                        case asTYPEID_INT32:
                            paramName = "i32";
                            break;
                        case asTYPEID_INT64:
                            paramName = "i64";
                            break;
                        case asTYPEID_UINT8:
                            paramName = "u8";
                            break;
                        case asTYPEID_UINT16:
                            paramName = "u16";
                            break;
                        case asTYPEID_UINT32:
                            paramName = "u32";
                            break;
                        case asTYPEID_UINT64:
                            paramName = "u64";
                            break;
                        case asTYPEID_FLOAT:
                            paramName = "f32";
                            break;
                        case asTYPEID_DOUBLE:
                            paramName = "f64";
                            break;
                        default:
                            paramName = "void";
                            break;
                        }
                    }
                    else
                    {
                        paramName = ScriptEngine::GetScriptEngine()->asEngine()->GetTypeInfoById(paramTypeId)->GetName();
                    }

                    std::string paramNameStr = {paramName};
                    size_t paramTypeHash = ScriptEngine::GetScriptEngine()->GetTypeHash(StringUtils::fnv1a_32(paramNameStr.c_str(), paramNameStr.length()));

                    size_t argTypeHash = argument.type().hash_code();

                    if (paramTypeHash != argTypeHash)
                    {
                        std::string argName = argument.type().name();
                        NC_LOG_FATAL("When trying to call function '%s', argument %u expects type %s but gets type %s", function->GetName(), i, paramNameStr.c_str(), argName.c_str());
                    }

#ifdef NC_Debug
                    //volatile std::string paramName = any::GetTypeName(paramTypeId);
                    //volatile std::string argName = any::GetTypeName(argTypeId);
#endif

                    assert(paramTypeHash == argTypeHash);

                    if (!argument.has_value())
                        continue;

                    if (argTypeHash == typeid(u8).hash_code())
                        context->setByte(std::any_cast<u8>(argument));
                    else if (argTypeHash == typeid(u16).hash_code())
                        context->setWord(std::any_cast<u16>(argument));
                    else if (argTypeHash == typeid(u32).hash_code())
                        context->setDWord(std::any_cast<u32>(argument));
                    else if (argTypeHash == typeid(u64).hash_code())
                        context->setQWord(std::any_cast<u64>(argument));
                    else if (argTypeHash == typeid(i8).hash_code())
                        context->setByte(std::any_cast<i8>(argument));
                    else if (argTypeHash == typeid(i16).hash_code())
                        context->setWord(std::any_cast<i16>(argument));
                    else if (argTypeHash == typeid(i32).hash_code())
                        context->setDWord(std::any_cast<i32>(argument));
                    else if (argTypeHash == typeid(i64).hash_code())
                        context->setQWord(std::any_cast<i64>(argument));
                    else if (argTypeHash == typeid(bool).hash_code())
                        context->setBool(std::any_cast<bool>(argument));
                    else if (argTypeHash == typeid(std::string).hash_code())
                    {
                        std::string string = std::any_cast<std::string>(argument);
                        context->setObject(&string);
                        break;
                    }
                    else if (argTypeHash == typeid(AngelScriptPlayer*).hash_code())
                        context->setObject(std::any_cast<AngelScriptPlayer*>(argument));
                    else if (argTypeHash == typeid(AngelScriptSpell*).hash_code())
                        context->setObject(std::any_cast<AngelScriptSpell*>(argument));
                    else if (argTypeHash == typeid(AngelScriptAura*).hash_code())
                        context->setObject(std::any_cast<AngelScriptAura*>(argument));
                    else
                        assert(false);
                }

                context->execute();
                context->release();
            }
        }
        return;
    }

private:
private:
    static thread_local AB_NAMESPACE_QUALIFIER Engine* _scriptEngine;

    //static std::function<void(AB_NAMESPACE_QUALIFIER Engine*)> const& _registerFunction;
    static func_t* _registerFunction;
    static asio::io_service* _ioService;
    static std::vector<NovusConnection*> _novusConnections;
};