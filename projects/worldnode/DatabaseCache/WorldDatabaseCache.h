#pragma once
#include "BaseDatabaseCache.h"
#include <robin_hood.h>
#include <Networking/ByteBuffer.h>

// item_template table in DB
class WorldDatabaseCache;
struct ItemStatInfo
{
    ItemStatInfo() {}
    ItemStatInfo(u32 type, i32 value) : statType(type), statValue(value) {}

    u32 statType = 0;
    i32 statValue = 0;
};
struct ItemDamageInfo
{
    ItemDamageInfo() {}
    ItemDamageInfo(f32 min, f32 max, u32 type) : damageMin(min), damageMax(max), damageType(type) {}

    f32 damageMin = 0;
    f32 damageMax = 0;
    u32 damageType = 0;
};
struct ItemSpellInfo
{
    ItemSpellInfo() {}
    ItemSpellInfo(u32 id, u32 trigger, i32 charges, i32 cooldown, u32 category, i32 categoryCooldown) : spellId(id), spellTrigger(trigger), spellCharges(charges), spellCooldown(cooldown), spellCategory(category), spellCategoryCooldown(categoryCooldown) {}

    u32 spellId = 0;
    u32 spellTrigger = 0;
    i32 spellCharges = 0;
    i32 spellCooldown = -1;
    u32 spellCategory = 0;
    i32 spellCategoryCooldown = -1;
};
struct ItemSocketInfo
{
    ItemSocketInfo() {}
    ItemSocketInfo(u32 type, u32 amount) : socketType(type), socketAmount(amount) {}

    u32 socketType = 0;
    u32 socketAmount = 0;
};
struct ItemTemplate
{
    ItemTemplate() {}
    ItemTemplate(WorldDatabaseCache* cache) { _cache = cache; }
    ItemTemplate(const ItemTemplate& data)
    {
        entry = data.entry;
        itemClass = data.itemClass;
        itemSubClass = data.itemSubClass;
        soundOverrideSubclass = data.soundOverrideSubclass;
        name = data.name;
        displayId = data.displayId;
        quality = data.quality;
        flags = data.flags;
        flagsExtra = data.flagsExtra;
        buyPrice = data.buyPrice;
        sellPrice = data.sellPrice;
        inventoryType = data.inventoryType;
        allowableClass = data.allowableClass;
        allowableRace = data.allowableRace;
        itemLevel = data.itemLevel;
        requiredLevel = data.requiredLevel;
        requiredSkill = data.requiredSkill;
        requiredSkillRank = data.requiredSkillRank;
        requiredSpell = data.requiredSpell;
        requiredHonorRank = data.requiredHonorRank;
        requiredCityRank = data.requiredCityRank;
        requiredReputationFaction = data.requiredReputationFaction;
        requiredReputationRank = data.requiredReputationRank;
        maxCount = data.maxCount;
        stackable = data.stackable;
        containerSlots = data.containerSlots;
        statsCount = data.statsCount;
        for (u32 i = 0; i < 10; i++)
        {
            statInfo[i] = data.statInfo[i];
        }
        scalingStatDistribution = data.scalingStatDistribution;
        scalingStatValue = data.scalingStatValue;
        for (u32 i = 0; i < 2; i++)
        {
            damageInfo[i] = data.damageInfo[i];
        }
        for (u32 i = 0; i < 7; i++)
        {
            resistances[i] = data.resistances[i];
        }
        attackSpeed = data.attackSpeed;
        ammoType = data.ammoType;
        rangeModifier = data.rangeModifier;
        for (u32 i = 0; i < 5; i++)
        {
            spellInfo[i] = data.spellInfo[i];
        }
        bindType = data.bindType;
        description = data.description;
        pageTextId = data.pageTextId;
        pageLanguageId = data.pageLanguageId;
        pageTextureId = data.pageTextureId;
        startQuest = data.startQuest;
        lockId = data.lockId;
        materialSound = data.materialSound;
        weaponSheath = data.weaponSheath;
        randomPropertyId = data.randomPropertyId;
        randomSuffixId = data.randomSuffixId;
        shieldBlock = data.shieldBlock;
        itemSetId = data.itemSetId;
        itemDurability = data.itemDurability;
        restrictUseToAreaId = data.restrictUseToAreaId;
        restrictUseToMapId = data.restrictUseToMapId;
        bagFamilyBitmask = data.bagFamilyBitmask;
        toolCategoryId = data.toolCategoryId;
        for (u32 i = 0; i < 3; i++)
        {
            socketInfo[i] = data.socketInfo[i];
        }
        socketBonusId = data.socketBonusId;
        gemPropertiesId = data.gemPropertiesId;
        requiredDisenchantSkill = data.requiredDisenchantSkill;
        armorDamageModifier = data.armorDamageModifier;
        limitedDuration = data.limitedDuration;
        itemLimitCategory = data.itemLimitCategory;
        holidayId = data.holidayId;
        _packet = data._packet;
        _cache = data._cache;
    }
    std::shared_ptr<ByteBuffer> GetQuerySinglePacket() { return _packet; }

    u32 entry;
    u32 itemClass;
    u32 itemSubClass;
    i32 soundOverrideSubclass;
    std::string name;
    u32 displayId;
    u32 quality;
    u32 flags;
    u32 flagsExtra;
    i32 buyPrice;
    u32 sellPrice;
    u32 inventoryType;
    i32 allowableClass;
    i32 allowableRace;
    u32 itemLevel;
    u32 requiredLevel;
    u32 requiredSkill;
    u32 requiredSkillRank;
    u32 requiredSpell;
    u32 requiredHonorRank;
    u32 requiredCityRank;
    u32 requiredReputationFaction;
    u32 requiredReputationRank;
    i32 maxCount;
    i32 stackable;
    u32 containerSlots;
    u32 statsCount;
    ItemStatInfo statInfo[10];
    u32 scalingStatDistribution;
    u32 scalingStatValue;
    ItemDamageInfo damageInfo[2];
    u32 resistances[7];
    u32 attackSpeed;
    u32 ammoType;
    f32 rangeModifier;
    ItemSpellInfo spellInfo[5];
    u32 bindType;
    std::string description;
    u32 pageTextId;
    u32 pageLanguageId;
    u32 pageTextureId;
    u32 startQuest;
    u32 lockId;
    i32 materialSound;
    u32 weaponSheath;
    i32 randomPropertyId;
    i32 randomSuffixId;
    u32 shieldBlock;
    u32 itemSetId;
    u32 itemDurability;
    u32 restrictUseToAreaId;
    u32 restrictUseToMapId;
    u32 bagFamilyBitmask;
    u32 toolCategoryId;
    ItemSocketInfo socketInfo[3];
    u32 socketBonusId;
    u32 gemPropertiesId;
    u32 requiredDisenchantSkill;
    f32 armorDamageModifier;
    u32 limitedDuration;
    u32 itemLimitCategory;
    u32 holidayId;

private:
    friend WorldDatabaseCache;
    std::shared_ptr<ByteBuffer> _packet;
    WorldDatabaseCache* _cache;
};

class WorldDatabaseCache : BaseDatabaseCache
{
public:
    WorldDatabaseCache();
    ~WorldDatabaseCache();

    void Load() override;
    void LoadAsync() override;
    void Save() override;
    void SaveAsync() override;

    // Item Template cache
    bool GetItemTemplate(u32 itemEntry, ItemTemplate& output);

private:
    friend ItemTemplate;

    robin_hood::unordered_map<u32, ItemTemplate> _itemTemplateCache;
};