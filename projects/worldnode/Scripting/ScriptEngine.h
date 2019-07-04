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

private:
private:
    static thread_local AB_NAMESPACE_QUALIFIER Engine* _scriptEngine;

    //static std::function<void(AB_NAMESPACE_QUALIFIER Engine*)> const& _registerFunction;
    static func_t* _registerFunction;
    static asio::io_service* _ioService;
    static std::vector<NovusConnection*> _novusConnections;
};