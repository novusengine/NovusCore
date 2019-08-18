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
#include <entt.hpp>
#include <NovusTypes.h>
#include <Networking/Opcode/Opcode.h>
#include <Utils/DebugHandler.h>
#include <Utils/AtomicLock.h>
#include <Math/Math.h>
#include <Math/Vector2.h>
#include <Cryptography/HMAC.h>
#include <Database/Cache/CharacterDatabaseCache.h>
#include <Database/Cache/DBCDatabaseCache.h>
#include <zlib.h>

#include "../../NovusEnums.h"
#include "../../Utils/CharacterUtils.h"
#include "../../WorldNodeHandler.h"
#include "../../Scripting/PlayerFunctions.h"
#include "../../Scripting/SpellFunctions.h"
#include "../../MessageHandler.h"

#include "../Components/PlayerConnectionComponent.h"
#include "../Components/PlayerFieldDataComponent.h"
#include "../Components/PlayerUpdateDataComponent.h"
#include "../Components/PlayerPositionComponent.h"
#include "../Components/AuraListComponent.h"

#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/PlayerDeleteQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"
#include "../Components/Singletons/WorldDatabaseCacheSingleton.h"
#include "../Components/Singletons/DBCDatabaseCacheSingleton.h"
#include "../Components/Singletons/PlayerPacketQueueSingleton.h"
#include "../Components/Singletons/MapSingleton.h"

#include <tracy/Tracy.hpp>

namespace NetworkPacketSystem
{
void Update(entt::registry& registry)
{
    SingletonComponent& singleton = registry.ctx<SingletonComponent>();
    CharacterDatabaseCacheSingleton& characterDatabase = registry.ctx<CharacterDatabaseCacheSingleton>();
    WorldDatabaseCacheSingleton& worldDatabase = registry.ctx<WorldDatabaseCacheSingleton>();
    DBCDatabaseCacheSingleton& dbcDatabase = registry.ctx<DBCDatabaseCacheSingleton>();
    PlayerPacketQueueSingleton& playerPacketQueue = registry.ctx<PlayerPacketQueueSingleton>();
    WorldNodeHandler& worldNodeHandler = *singleton.worldNodeHandler;
    MapSingleton& mapSingleton = registry.ctx<MapSingleton>();

    LockRead(SingletonComponent);
    LockRead(PlayerDeleteQueueSingleton);
    LockRead(CharacterDatabaseCacheSingleton);

    LockWrite(PlayerConnectionComponent);
    LockWrite(PlayerFieldDataComponent);
    LockWrite(PlayerUpdateDataComponent);
    LockWrite(PlayerPositionComponent);

    MessageHandler* messageHandler = MessageHandler::Instance();

    auto view = registry.view<PlayerConnectionComponent, PlayerFieldDataComponent, PlayerUpdateDataComponent, PlayerPositionComponent>();
    view.each([&registry, &singleton, &characterDatabase, &worldDatabase, &dbcDatabase, &playerPacketQueue, &worldNodeHandler, &mapSingleton, &messageHandler](const auto, PlayerConnectionComponent& playerConnection, PlayerFieldDataComponent& clientFieldData, PlayerUpdateDataComponent& playerUpdateData, PlayerPositionComponent& playerPositionData) {
        ZoneScopedNC("Connection", tracy::Color::Orange2)

        for (NetPacket& packet : playerConnection.packets)
        {
            packet.handled = messageHandler->CallHandler(&packet, &playerConnection);
        }

        /* Cull Movement Data */
        if (playerConnection.packets.size() > 0)
        {
            ZoneScopedNC("Packet::PacketClear", tracy::Color::Orange2)
                playerConnection.packets.erase(std::remove_if(playerConnection.packets.begin(), playerConnection.packets.end(), [](NetPacket& packet) {
                                                   return packet.handled;
                                               }),
                                               playerConnection.packets.end());
        }
    });
}
} // namespace NetworkPacketSystem
