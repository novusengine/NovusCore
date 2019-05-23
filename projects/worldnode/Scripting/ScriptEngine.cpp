#include "ScriptEngine.h"
#include <Utils/DebugHandler.h>

thread_local AB_NAMESPACE_QUALIFIER Engine* ScriptEngine::_scriptEngine = nullptr;
asio::io_service* ScriptEngine::_ioService = nullptr;
std::vector<NovusConnection*> ScriptEngine::_novusConnections;

void Placeholder(AB_NAMESPACE_QUALIFIER Engine* engine) {}
func_t* ScriptEngine::_registerFunction = nullptr;

void PrintScriptOutput(AB_NAMESPACE_QUALIFIER Engine*, std::string msg) 
{
	NC_LOG_MESSAGE("[Script]: %s", msg.c_str());
}

AB_NAMESPACE_QUALIFIER Engine* ScriptEngine::GetScriptEngine()
{
	if (_scriptEngine == nullptr)
	{
		_scriptEngine = new AB_NAMESPACE_QUALIFIER Engine(PrintScriptOutput);
		_registerFunction(_scriptEngine);
		_scriptEngine->asEngine()->SetEngineProperty(asEP_DISALLOW_GLOBAL_VARS, true);
	}

	return _scriptEngine;
}

AB_NAMESPACE_QUALIFIER Context* ScriptEngine::GetScriptContext()
{
	return GetScriptEngine()->getContext();
}

void ScriptEngine::RegisterNovusConnection(NovusConnection* connection)
{
	_novusConnections.push_back(connection);
}