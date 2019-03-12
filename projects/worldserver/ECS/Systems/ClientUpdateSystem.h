#pragma once
#include <NovusTypes.h>
#include <entt.hpp>

#include "../Components/PositionComponent.h"
#include "../Components/UnitStatusComponent.h"

namespace ClientUpdateSystem
{
	void Update(f32 deltaTime, entt::registry<u32> &registry) 
    {
		auto view = registry.view<PositionComponent>();

		view.each([](const auto, auto &pos) {
			pos.x += 1;
			pos.y += 1;
		});
	}
}
