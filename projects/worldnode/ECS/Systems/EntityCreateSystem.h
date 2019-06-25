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

#include "../../Message.h"
#include "../../DatabaseCache/CharacterDatabaseCache.h"

#include "../Components/ItemInitializeComponent.h"
#include "../Components/ItemDataComponent.h"
#include "../Components/ItemFieldDataComponent.h"
#include "../Components/UnitInitializeComponent.h"
#include "../Components/UnitDataComponent.h"
#include "../Components/UnitFieldDataComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/EntityCreateQueueSingleton.h"
#include "../Components/Singletons/CharacterDatabaseCacheSingleton.h"

namespace EntityCreateSystem
{
    void Update(entt::registry &registry)
    {
        EntityCreateQueueSingleton& entityCreateQueue = registry.ctx<EntityCreateQueueSingleton>();

        EntityCreationRequest entityCreationRequest;
        while (entityCreateQueue.newEntityQueue->try_dequeue(entityCreationRequest))
        {
            u32 entity = registry.create();

            if (entityCreationRequest.typeId == TYPEID_ITEM)
            {
                ItemCreationInformation* itemCreationInformation = reinterpret_cast<ItemCreationInformation*>(entityCreationRequest.typeInformation);

                ObjectGuid itemGuid(HighGuid::Item, itemCreationInformation->entryId, itemCreationInformation->lowGuid);
                registry.assign<ItemDataComponent>(entity, entity, itemGuid, itemCreationInformation->bagSlot, itemCreationInformation->bagPosition, itemCreationInformation->characterGuid);
                registry.assign<ItemInitializeComponent>(entity, itemCreationInformation->characterEntityId, itemGuid, itemCreationInformation->bagSlot, itemCreationInformation->bagPosition, itemCreationInformation->characterGuid);

                registry.assign<ItemFieldDataComponent>(entity);

                delete itemCreationInformation;
            }
            else if (entityCreationRequest.typeId == TYPEID_UNIT)
            {
                EntityCreationInformation* unitCreationInformation = entityCreationRequest.typeInformation;

                ObjectGuid unitGuid(HighGuid::Unit, unitCreationInformation->entryId, unitCreationInformation->lowGuid);
                registry.assign<UnitDataComponent>(entity, entity, unitGuid);
                registry.assign<UnitInitializeComponent>(entity, entity, unitGuid);

                registry.assign<UnitFieldDataComponent>(entity);

                delete unitCreationInformation;
            }
        }
    }
}
