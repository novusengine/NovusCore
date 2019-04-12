#include "PlayerConnectionSystem.h"
#include <Networking/Opcode/Opcode.h>
#include <NovusTypes.h>
#include "../NovusEnums.h"
#include "../Utils/CharacterUtils.h"

#include "../DatabaseCache/CharacterDatabaseCache.h"

#include "../Components/ItemFieldDataComponent.h"
#include "../Components/ItemInitializeComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/WorldDatabaseCacheSingleton.h"

namespace ItemInitializeSystem
{
    void Update(entt::registry &registry)
    {
        SingletonComponent& singleton = registry.ctx<SingletonComponent>();
        WorldDatabaseCacheSingleton& worldDatabase = registry.ctx<WorldDatabaseCacheSingleton>();
        NovusConnection& novusConnection = *singleton.connection;

        auto itemView = registry.view<ItemInitializeComponent, ItemFieldDataComponent>();
        itemView.each([&worldDatabase, &novusConnection](const auto, ItemInitializeComponent& itemInitializeData, ItemFieldDataComponent& itemFieldData)
        {
            ItemTemplate itemTemplate;
            if (worldDatabase.cache->GetItemTemplate(itemInitializeData.itemEntry, itemTemplate))
            {
                itemFieldData.ResetFields();

                u64 itemGuid = (static_cast<u64>(2) | (static_cast<u64>(itemTemplate.entry) << 24) | (static_cast<u64>(0x4000) << 48));

                itemFieldData.SetGuidValue(OBJECT_FIELD_GUID, itemGuid);
                itemFieldData.SetFieldValue<u32>(OBJECT_FIELD_TYPE, 0x3); // Object Type Item (Item, Object)
                itemFieldData.SetFieldValue<u32>(OBJECT_FIELD_ENTRY, itemTemplate.entry);
                itemFieldData.SetFieldValue<f32>(OBJECT_FIELD_SCALE_X, 1.0f);

                itemFieldData.SetFieldValue<u64>(ITEM_FIELD_OWNER, itemInitializeData.characterGuid);
                itemFieldData.SetFieldValue<u64>(ITEM_FIELD_CONTAINED, itemInitializeData.characterGuid);

                itemFieldData.SetFieldValue<u32>(ITEM_FIELD_STACK_COUNT, itemTemplate.stackable);
                itemFieldData.SetFieldValue<u32>(ITEM_FIELD_MAXDURABILITY, itemTemplate.itemDurability);
                itemFieldData.SetFieldValue<u32>(ITEM_FIELD_DURABILITY, itemTemplate.itemDurability);

                for (u32 i = 0; i < 5; i++)
                {
                    itemFieldData.SetFieldValue<u32>(ITEM_FIELD_SPELL_CHARGES + i, itemTemplate.spellInfo[i].spellCharges);
                }

                itemFieldData.SetFieldValue<u32>(ITEM_FIELD_DURATION, itemTemplate.limitedDuration);
                itemFieldData.SetFieldValue<u32>(ITEM_FIELD_CREATE_PLAYED_TIME, 0);
            }
        });
    }
}