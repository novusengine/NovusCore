#include "ScriptEngine.h"
#include <Utils/DebugHandler.h>

thread_local AngelBinder::Engine* ScriptEngine::_scriptEngine = nullptr;
asio::io_service* ScriptEngine::_ioService = nullptr;
std::vector<NovusConnection*> ScriptEngine::_novusConnections;

void Placeholder(AngelBinder::Engine* engine) {}
func_t* ScriptEngine::_registerFunction = nullptr;

void PrintScriptOutput(AngelBinder::Engine*, std::string msg) 
{
	NC_LOG_MESSAGE("[Script]: %s", msg.c_str());
}

AngelBinder::Engine* ScriptEngine::GetScriptEngine()
{
	if (_scriptEngine == nullptr)
	{
		_scriptEngine = new AngelBinder::Engine(PrintScriptOutput);
		_registerFunction(_scriptEngine);
		_scriptEngine->asEngine()->SetEngineProperty(asEP_DISALLOW_GLOBAL_VARS, true);
	}

	return _scriptEngine;
}

AngelBinder::Context* ScriptEngine::GetScriptContext()
{
	return GetScriptEngine()->getContext();
}

void ScriptEngine::RegisterNovusConnection(NovusConnection* connection)
{
	_novusConnections.push_back(connection);
}