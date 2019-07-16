#include "NetworkPacketSystem.h"
#include <NovusTypes.h>
#include <Networking/Opcode/Opcode.h>
#include <Database/Cache/CharacterDatabaseCache.h>

#include "../../NovusEnums.h"
#include "../../Utils/CharacterUtils.h"


#include "../Components/ItemFieldDataComponent.h"
#include "../Components/ItemInitializeComponent.h"
#include "../Components/UnitFieldDataComponent.h"
#include "../Components/UnitInitializeComponent.h"
#include "../Components/Singletons/SingletonComponent.h"
#include "../Components/Singletons/WorldDatabaseCacheSingleton.h"

namespace EntityInitializeSystem
{
void Update(entt::registry& registry)
{
    WorldDatabaseCacheSingleton& worldDatabase = registry.ctx<WorldDatabaseCacheSingleton>();

    auto itemView = registry.view<ItemInitializeComponent, ItemFieldDataComponent>();
    itemView.each([&worldDatabase](const auto, ItemInitializeComponent& itemInitializeData, ItemFieldDataComponent& itemFieldData) {
        ItemTemplate itemTemplate;
        if (worldDatabase.cache->GetItemTemplate(itemInitializeData.itemGuid.GetEntry(), itemTemplate))
        {
            itemFieldData.ResetFields();

            itemFieldData.SetGuidValue(OBJECT_FIELD_GUID, itemInitializeData.itemGuid);
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

    auto unitView = registry.view<UnitInitializeComponent, UnitFieldDataComponent>();
    unitView.each([](const auto, UnitInitializeComponent& unitInitializeData, UnitFieldDataComponent& unitFieldData) {
        unitFieldData.ResetFields();

        unitFieldData.SetGuidValue(OBJECT_FIELD_GUID, unitInitializeData.unitGuid);
        unitFieldData.SetFieldValue<u32>(OBJECT_FIELD_TYPE, 0x9); // Object Type Unit (Item, Unit)
        unitFieldData.SetFieldValue<u32>(OBJECT_FIELD_ENTRY, 25);
        unitFieldData.SetFieldValue<f32>(OBJECT_FIELD_SCALE_X, 1.0f);

        unitFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, 1, 0);
        unitFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, 1, 1);
        unitFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, 1, 2);
        unitFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_0, 1, 3);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_HEALTH, 60);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER1, 0);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER2, 0);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER3, 100);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER4, 100);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER5, 0);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER6, 8);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_POWER7, 1000);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXHEALTH, 60);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER1, 0);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER2, 1000);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER3, 100);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER4, 100);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER5, 0);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER6, 8);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_MAXPOWER7, 1000);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_LEVEL, 80);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_FACTIONTEMPLATE, 35);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_FLAGS, 0x00000008);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_FLAGS_2, 0x00000800);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_BASEATTACKTIME + 0, 2900);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_BASEATTACKTIME + 1, 2000);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_RANGEDATTACKTIME, 0);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_BOUNDINGRADIUS, 0.208000f);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_COMBATREACH, 1.5f);

        u32 displayId = unitInitializeData.unitGuid.GetEntry() == 25 ? 31007 : 30721;

        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_DISPLAYID, displayId);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_NATIVEDISPLAYID, displayId);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_MOUNTDISPLAYID, 0);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_MINDAMAGE, 9.007143f);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_MAXDAMAGE, 11.007143f);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_MINOFFHANDDAMAGE, 0);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_MAXOFFHANDDAMAGE, 0);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_BYTES_1, 0);
        unitFieldData.SetFieldValue<f32>(UNIT_MOD_CAST_SPEED, 1);

        for (i32 i = 0; i < 5; i++)
        {
            unitFieldData.SetFieldValue<u32>(UNIT_FIELD_STAT0 + i, 20);
            unitFieldData.SetFieldValue<i32>(UNIT_FIELD_POSSTAT0 + i, 0);
            unitFieldData.SetFieldValue<i32>(UNIT_FIELD_NEGSTAT0 + i, 0);
        }

        for (i32 i = 0; i < 7; i++)
        {
            unitFieldData.SetFieldValue<u32>(UNIT_FIELD_RESISTANCES + i, 0);
            unitFieldData.SetFieldValue<i32>(UNIT_FIELD_RESISTANCEBUFFMODSPOSITIVE + i, 0);
            unitFieldData.SetFieldValue<i32>(UNIT_FIELD_RESISTANCEBUFFMODSNEGATIVE + i, 0);
        }
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_STAT0, 42);

        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_BASE_MANA, 0);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_BASE_HEALTH, 20);
        unitFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0);
        unitFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0, 1);
        unitFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0, 2);
        unitFieldData.SetFieldValue<u8>(UNIT_FIELD_BYTES_2, 0, 3);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_ATTACK_POWER, 29);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_ATTACK_POWER_MODS, 0);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_ATTACK_POWER_MULTIPLIER, 1.0f);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_RANGED_ATTACK_POWER, 0);
        unitFieldData.SetFieldValue<u32>(UNIT_FIELD_RANGED_ATTACK_POWER_MODS, 0);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_RANGED_ATTACK_POWER_MULTIPLIER, 1.0f);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_MINRANGEDDAMAGE, 0);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_MAXRANGEDDAMAGE, 0);
        unitFieldData.SetFieldValue<f32>(UNIT_FIELD_HOVERHEIGHT, 1);
    });
}
} // namespace EntityInitializeSystem
