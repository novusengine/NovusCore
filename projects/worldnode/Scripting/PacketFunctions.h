#pragma once
#include <Utils/DebugHandler.h>
#include <Config/ConfigHandler.h>
#include "AngelBinder.h"

#include "ScriptEngine.h"

namespace PacketFunctions
{
	
}

void RegisterPacketFunctions(AB_NAMESPACE_QUALIFIER Engine* engine)
{
	engine->asEngine()->SetDefaultNamespace("Packet");
	/*AB_NAMESPACE_QUALIFIER Exporter::Export(*engine)
		[
			AB_NAMESPACE_QUALIFIER Exporter::Functions()
			.def("HelloWorld", &PacketFunctions::HelloWorld)
			.def("SendLoginChallenge", &PacketFunctions::SendLoginChallenge)
		];*/

	engine->asEngine()->SetDefaultNamespace("");
}