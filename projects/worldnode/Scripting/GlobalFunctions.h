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

void RegisterGlobalFunctions(AngelBinder::Engine* engine)
{
	// Register*Callback functions need to be registered manually since the binder does not support it
	engine->asEngine()->RegisterFuncdef("void PlayerCallback(string, uint8)");
	engine->asEngine()->RegisterGlobalFunction("void RegisterPlayerCallback(uint32 id, PlayerCallback @cb)", asFUNCTION(GlobalFunctions::RegisterPlayerCallback), asCALL_CDECL);

	AngelBinder::Exporter::Export(*engine)
		[
			AngelBinder::Exporter::Functions()
			.def("Print", &GlobalFunctions::Print)
		];
}