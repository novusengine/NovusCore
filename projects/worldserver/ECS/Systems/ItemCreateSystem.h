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
#include <NovusTypes.h>
#include <entt.hpp>
#include <Networking/ByteBuffer.h>
#include "../Message.h"

#include "../DatabaseCache/CharacterDatabaseCache.h"

#include "../Connections/NovusConnection.h"
#include "../Components/ItemInitializeComponent.h"
#include "../Components/ItemDataComponent.h"
#include "../Components/ItemFieldDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/ItemCreateQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"

namespace ItemCreateSystem
{
    void Update(entt::registry &registry)
    {
		SingletonComponent& singleton = registry.ctx<SingletonComponent>();
        ItemCreateQueueSingleton& createItemQueue = registry.ctx<ItemCreateQueueSingleton>();
        CharacterDatabaseCacheSingleton& characterDatabase = registry.ctx<CharacterDatabaseCacheSingleton>();

        ItemCreationInformation itemCreationInformation;
        while (createItemQueue.newItemQueue->try_dequeue(itemCreationInformation))
        {
            u32 entity = registry.create();

			ObjectGuid itemGuid(HighGuid::Item, itemCreationInformation.itemEntry, itemCreationInformation.lowGuid);
            registry.assign<ItemDataComponent>(entity, entity, itemGuid, itemCreationInformation.bagSlot, itemCreationInformation.bagPosition, itemCreationInformation.accountGuid, itemCreationInformation.characterGuid);
            registry.assign<ItemInitializeComponent>(entity, itemCreationInformation.clientEntityGuid, itemGuid, itemCreationInformation.bagSlot, itemCreationInformation.bagPosition, itemCreationInformation.accountGuid, itemCreationInformation.characterGuid);

            registry.assign<ItemFieldDataComponent>(entity);
        }
    }
}