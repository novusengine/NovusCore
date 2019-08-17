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

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerFieldDataComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"

#include <tracy/Tracy.hpp>

namespace PlayerUpdateSystem
{
    void Update(entt::registry& registry)
    {
        PlayerPacketQueueSingleton& playerPacketQueue = registry.ctx<PlayerPacketQueueSingleton>();

        PacketQueueData packet;
        while (playerPacketQueue.packetQueue->try_dequeue(packet))
        {
            packet.connection->SendPacket(packet.data.get(), packet.opcode);
        }

        auto playerView = registry.view<PlayerConnectionComponent, UnitStatsComponent>();
        playerView.each([&registry](const auto, PlayerConnectionComponent& playerConnectionComponent, UnitStatsComponent& unitStatsComponent) 
        {
            if (unitStatsComponent.healthIsDirty)
            {
                // Create HEALTH_UPDATE packet
                std::shared_ptr<ByteBuffer> byteBuffer = ByteBuffer::Borrow<12>();
                byteBuffer->PutGuid(playerConnectionComponent.characterGuid);
                byteBuffer->PutI32(static_cast<i32>(unitStatsComponent.currentHealth));

                CharacterUtils::SendPacketToGridPlayers(&registry, playerConnectionComponent.entityId, byteBuffer, Opcode::SMSG_HEALTH_UPDATE);

                unitStatsComponent.healthIsDirty = false;
            }

            for (int i = 0; i < POWER_COUNT; i++)
            {
                if (unitStatsComponent.powerIsDirty[i])
                {
                    // Create POWER_UPDATE packet
                    std::shared_ptr<ByteBuffer> byteBuffer = ByteBuffer::Borrow<13>();
                    byteBuffer->PutGuid(playerConnectionComponent.characterGuid);
                    byteBuffer->PutU8(i);

                    f32 power = unitStatsComponent.currentPower[i];

                    // When updating the client on units rage values, we have to multiply our internal value with 10.
                    // This is necessary because the client represents their powers as ints, and 1 rage in the visible bar equals 10 "client rage"
                    if (i == POWER_RAGE)
                        power *= 10.0f;

                    byteBuffer->PutI32(static_cast<i32>(power));

                    CharacterUtils::SendPacketToGridPlayers(&registry, playerConnectionComponent.entityId, byteBuffer, Opcode::SMSG_POWER_UPDATE);

                    unitStatsComponent.powerIsDirty[i] = false;
                }
            }
        });
    }
} // namespace PlayerUpdateSystem
