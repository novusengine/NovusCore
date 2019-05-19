#pragma once
#include <Utils/DebugHandler.h>
#include <Config/ConfigHandler.h>
#include "AngelBinder.h"

#include "ScriptEngine.h"

namespace PacketFunctions
{
	
}

void RegisterPacketFunctions(AngelBinder::Engine* engine)
{
	engine->asEngine()->SetDefaultNamespace("Packet");
	/*AngelBinder::Exporter::Export(*engine)
		[
			AngelBinder::Exporter::Functions()
			.def("HelloWorld", &PacketFunctions::HelloWorld)
			.def("SendLoginChallenge", &PacketFunctions::SendLoginChallenge)
		];*/

	engine->asEngine()->SetDefaultNamespace("");
}