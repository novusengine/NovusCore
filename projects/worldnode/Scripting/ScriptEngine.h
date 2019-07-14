#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <Utils/SharedPool.h>
#include "AngelBinder.h"
#include <asio.hpp>

typedef void(func_t)(AB_NAMESPACE_QUALIFIER Engine*);

class NovusConnection;
class ScriptEngine
{
public:
    static func_t* GetRegisterFunction() { return _registerFunction; }
    static void SetRegisterFunction(func_t* func) { _registerFunction = func; }

    static AB_NAMESPACE_QUALIFIER Engine* GetScriptEngine();
    static AB_NAMESPACE_QUALIFIER Context* GetScriptContext();
    static asio::io_service* GetIOService() { return _ioService; }
    static void SetIOService(asio::io_service* service) { _ioService = service; }

    static void RegisterNovusConnection(NovusConnection* connection);

    template <typename Hooks, typename... Args>
    inline static void CallHook(std::array<std::vector<asIScriptFunction*>, Hooks::COUNT>& hooks, Hooks id, Args... args)
    {
        std::vector<any> arguments = {args...};

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
                    size_t argTypeId = argument.GetType();

                    if (paramTypeId != argTypeId)
                    {
                        NC_LOG_FATAL("When trying to call function '%s', argument %u expects type %s but gets type %s", function->GetName(), i, any::GetTypeName(paramTypeId), any::GetTypeName(argTypeId));
                    }

                    assert(paramTypeId == argTypeId);

                    switch (argTypeId)
                    {
                    case any::U8:
                        context->setByte(argument.GetU8());
                        break;
                    case any::U16:
                        context->setWord(argument.GetU16());
                        break;
                    case any::U32:
                        context->setDWord(argument.GetU32());
                        break;
                    case any::U64:
                        context->setQWord(argument.GetU64());
                        break;
                    case any::I8:
                        context->setByte(argument.GetI8());
                        break;
                    case any::I16:
                        context->setWord(argument.GetI16());
                        break;
                    case any::I32:
                        context->setDWord(argument.GetI32());
                        break;
                    case any::I64:
                        context->setQWord(argument.GetI64());
                        break;
                    case any::F32:
                        context->setFloat(argument.GetF32());
                        break;
                    case any::F64:
                        context->setDouble(argument.GetF64());
                        break;
                    case any::Bool:
                        context->setBool(argument.GetBool());
                        break;
                    case any::String:
                        context->setObject(&argument.GetString());
                        break;
                    case any::Player:
                        context->setObject(argument.GetPlayer());
                        break;
                    case any::Spell:
                        context->setObject(argument.GetSpell());
                        break;
                    }
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