#pragma once
#include <Utils/DebugHandler.h>
#include "AngelBinder.h"

#include "PlayerHooks.h"

namespace GlobalFunctions
{
	inline void RegisterPlayerCallback(u32 callbackId, asIScriptFunction* callback)
	{
		NC_LOG_MESSAGE("Register Callback!");
        PlayerHooks::Register(static_cast<PlayerHooks::Hooks>(callbackId), callback);
	}

	inline void Print(std::string& message)
	{
		NC_LOG_MESSAGE("[Script]: %s", message.c_str());
	}
}

void RegisterGlobalFunctions(AB_NAMESPACE_QUALIFIER Engine* engine)
{
	// Register*Callback functions need to be registered manually since the binder does not support it
	engine->asEngine()->RegisterFuncdef("void PlayerCallback(string, uint8)");
	engine->asEngine()->RegisterGlobalFunction("void RegisterPlayerCallback(uint32 id, PlayerCallback @cb)", asFUNCTION(GlobalFunctions::RegisterPlayerCallback), asCALL_CDECL);
    //engine->asEngine()->RegisterGlobalFunction("void Print(string message)", asFUNCTION(GlobalFunctions::Print), asCALL_CDECL);

	AB_NAMESPACE_QUALIFIER Exporter::Export(*engine)
		[
			AB_NAMESPACE_QUALIFIER Exporter::Functions()
			.def("Print", &GlobalFunctions::Print)
		];
}