#pragma once
#include <Utils/DebugHandler.h>
#include "AngelBinder.h"
#include <entt.hpp>

#include "../ECS/Components/PlayerUpdateDataComponent.h"
#include "../ECS/Components/PlayerPositionComponent.h"

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
}

void RegisterGlobalFunctions(AB_NAMESPACE_QUALIFIER Engine* engine)
{
	// Register*Callback functions need to be registered manually since the binder does not support it

    //engine->asEngine()->RegisterGlobalFunction("void Print(string message)", asFUNCTION(GlobalFunctions::Print), asCALL_CDECL);
    //engine->asEngine()->RegisterGlobalFunction("PlayerPositionComponent@ GetComponent<PlayerPositionComponent>(uint32 entity)", asFUNCTION(GlobalFunctions::GetPlayerPositionComponent), asCALL_CDECL);

	AB_NAMESPACE_QUALIFIER Exporter::Export(*engine)
		[
			AB_NAMESPACE_QUALIFIER Exporter::Functions()
			.def("Print", &GlobalFunctions::Print)
		];
}

