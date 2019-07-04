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
#include "../Components/Singletons/PlayerUpdatesQueueSingleton.h"

#include <tracy/Tracy.hpp>

namespace PlayerUpdateSystem
{
void Update(entt::registry& registry)
{
    PlayerUpdatesQueueSingleton& playerUpdatesQueue = registry.ctx<PlayerUpdatesQueueSingleton>();
    PlayerPacketQueueSingleton& playerPacketQueue = registry.ctx<PlayerPacketQueueSingleton>();

    auto view = registry.view<PlayerConnectionComponent, PlayerUpdateDataComponent>();
    view.each([playerUpdatesQueue](const auto, PlayerConnectionComponent& playerConnection, PlayerUpdateDataComponent& playerUpdateData) {
        for (MovementPacket movementPacket : playerUpdatesQueue.playerMovementPacketQueue)
        {
            if (playerConnection.characterGuid != movementPacket.characterGuid)
            {
                playerConnection.socket->SendPacket(movementPacket.data.get(), movementPacket.opcode);
            }
        }

        for (ChatPacket chatPacket : playerUpdatesQueue.playerChatPacketQueue)
        {
            playerConnection.socket->SendPacket(chatPacket.data.get(), Opcode::SMSG_MESSAGECHAT);
        }
    });

    PacketQueueData packet;
    while (playerPacketQueue.packetQueue->try_dequeue(packet))
    {
        packet.connection->SendPacket(packet.data.get(), packet.opcode);
    }

    if (!playerUpdatesQueue.playerMovementPacketQueue.empty())
        playerUpdatesQueue.playerMovementPacketQueue.clear();

    if (!playerUpdatesQueue.playerChatPacketQueue.empty())
        playerUpdatesQueue.playerChatPacketQueue.clear();
}
} // namespace PlayerUpdateSystem
