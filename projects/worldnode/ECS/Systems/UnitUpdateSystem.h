/*
    MIT License

    Copyright (c) 2018-2019 NovusCore

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/
#pragma once
#include <algorithm>
#include <NovusTypes.h>
#include <entt.hpp>
#include <Networking/ByteBuffer.h>

#include "../../NovusEnums.h"

#include "../Components/UnitStatsComponent.h"
#include "../Components/Singletons/SingletonComponent.h"

#include <tracy/Tracy.hpp>

namespace UnitUpdateSystem
{
void Update(entt::registry& registry)
{
    SingletonComponent& singleton = registry.ctx<SingletonComponent>();

    auto view = registry.view<UnitStatsComponent>();
    view.each([&singleton](const auto, UnitStatsComponent& unitStatsComponent) {
        // Handle rage
        {
            f32 rage = unitStatsComponent.currentPower[POWER_RAGE];
            // TODO: Add this to a config file maybe?
            f32 rageDecayRate = 1.0f;

            f32 decay = rageDecayRate * singleton.deltaTime;
            rage -= decay;  

            rage = Math::Max(0.0f, rage);
            
            if (static_cast<i32>(rage) != static_cast<i32>(unitStatsComponent.currentPower[POWER_RAGE]))
            {
                unitStatsComponent.powerIsDirty[POWER_RAGE] = true;
            }
            unitStatsComponent.currentPower[POWER_RAGE] = rage;
        }
        // Handle Energy
        {
            f32 energy = unitStatsComponent.currentPower[POWER_ENERGY];
            // TODO: Add this to a config file maybe?
            f32 energyRegainRate = 10.0f;

            energy = Math::Min(unitStatsComponent.maxPower[POWER_ENERGY], energy + (energyRegainRate * singleton.deltaTime));
            
            if (static_cast<i32>(energy) != static_cast<i32>(unitStatsComponent.currentPower[POWER_ENERGY]))
            {
                unitStatsComponent.powerIsDirty[POWER_ENERGY] = true;
            }
            unitStatsComponent.currentPower[POWER_ENERGY] = energy;
        }
    });
}
} // namespace UnitUpdateSystem
