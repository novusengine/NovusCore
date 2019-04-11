#include "WorldDatabaseCache.h"
#include <Database/DatabaseConnector.h>
#include <Database/PreparedStatement.h>

WorldDatabaseCache::WorldDatabaseCache()
{
}
WorldDatabaseCache::~WorldDatabaseCache()
{
}

void WorldDatabaseCache::Load()
{
    std::shared_ptr<DatabaseConnector> connector;
    bool result = DatabaseConnector::Borrow(DATABASE_TYPE::WORLDSERVER, connector);
    assert(result);

    amy::result_set resultSet;
    connector->Query("SELECT * FROM item_template;", resultSet);

    if (resultSet.affected_rows() > 0)
    {
        for (auto row : resultSet)
        {
            ItemTemplate itemTemplate(this);
            itemTemplate.entry = row[0].GetU32();
            itemTemplate.itemClass = row[1].GetU32();
            itemTemplate.itemSubClass = row[2].GetU32();
            itemTemplate.soundOverrideSubclass = row[3].GetI32();
            itemTemplate. name = row[4].GetString();
            itemTemplate.displayId = row[5].GetU32();
            itemTemplate.quality = row[6].GetU32();
            itemTemplate.flags = row[7].GetU32();
            itemTemplate.flagsExtra = row[8].GetU32();
            itemTemplate.buyPrice = row[9].GetI32();
            itemTemplate.sellPrice = row[10].GetU32();
            itemTemplate.inventoryType = row[11].GetU32();
            itemTemplate.allowableClass = row[12].GetI32();
            itemTemplate.allowableRace = row[13].GetI32();
            itemTemplate.itemLevel = row[14].GetU32();
            itemTemplate.requiredLevel = row[15].GetU32();
            itemTemplate.requiredSkill = row[16].GetU32();
            itemTemplate.requiredSkillRank = row[17].GetU32();
            itemTemplate.requiredSpell = row[18].GetU32();
            itemTemplate.requiredHonorRank = row[19].GetU32();
            itemTemplate.requiredCityRank = row[20].GetU32();
            itemTemplate.requiredReputationFaction = row[21].GetU32();
            itemTemplate.requiredReputationRank = row[22].GetU32();
            itemTemplate.maxCount = row[23].GetI32();
            itemTemplate.stackable = row[24].GetI32();
            itemTemplate.containerSlots = row[25].GetU32();
            itemTemplate.statsCount = row[26].GetU32();
            itemTemplate.statInfo[0] = ItemStatInfo(row[27].GetU32(), row[28].GetI32());
            itemTemplate.statInfo[1] = ItemStatInfo(row[29].GetU32(), row[30].GetI32());
            itemTemplate.statInfo[2] = ItemStatInfo(row[31].GetU32(), row[32].GetI32());
            itemTemplate.statInfo[3] = ItemStatInfo(row[33].GetU32(), row[34].GetI32());
            itemTemplate.statInfo[4] = ItemStatInfo(row[35].GetU32(), row[36].GetI32());
            itemTemplate.statInfo[5] = ItemStatInfo(row[37].GetU32(), row[38].GetI32());
            itemTemplate.statInfo[6] = ItemStatInfo(row[39].GetU32(), row[40].GetI32());
            itemTemplate.statInfo[7] = ItemStatInfo(row[41].GetU32(), row[42].GetI32());
            itemTemplate.statInfo[8] = ItemStatInfo(row[43].GetU32(), row[44].GetI32());
            itemTemplate.statInfo[9] = ItemStatInfo(row[45].GetU32(), row[46].GetI32());
            itemTemplate.scalingStatDistribution = row[47].GetU32();
            itemTemplate.scalingStatValue = row[48].GetU32();
            itemTemplate.damageInfo[0] = ItemDamageInfo(row[49].GetF32(), row[50].GetF32(), row[51].GetU32());
            itemTemplate.damageInfo[1] = ItemDamageInfo(row[52].GetF32(), row[53].GetF32(), row[54].GetU32());
            itemTemplate.resistances[0] = row[55].GetU32();
            itemTemplate.resistances[1] = row[56].GetU32();
            itemTemplate.resistances[2] = row[57].GetU32();
            itemTemplate.resistances[3] = row[58].GetU32();
            itemTemplate.resistances[4] = row[59].GetU32();
            itemTemplate.resistances[5] = row[60].GetU32();
            itemTemplate.resistances[6] = row[61].GetU32();
            itemTemplate.attackSpeed = row[62].GetU32();
            itemTemplate.ammoType = row[63].GetU32();
            itemTemplate.rangeModifier = row[64].GetF32();
            itemTemplate.spellInfo[0] = ItemSpellInfo(row[65].GetU32(), row[66].GetU32(), row[67].GetI32(), row[68].GetI32(), row[69].GetU32(), row[70].GetI32());
            itemTemplate.spellInfo[1] = ItemSpellInfo(row[71].GetU32(), row[72].GetU32(), row[73].GetI32(), row[74].GetI32(), row[75].GetU32(), row[76].GetI32());
            itemTemplate.spellInfo[2] = ItemSpellInfo(row[77].GetU32(), row[78].GetU32(), row[79].GetI32(), row[80].GetI32(), row[81].GetU32(), row[82].GetI32());
            itemTemplate.spellInfo[3] = ItemSpellInfo(row[83].GetU32(), row[84].GetU32(), row[85].GetI32(), row[86].GetI32(), row[87].GetU32(), row[88].GetI32());
            itemTemplate.spellInfo[4] = ItemSpellInfo(row[89].GetU32(), row[90].GetU32(), row[91].GetI32(), row[92].GetI32(), row[93].GetU32(), row[94].GetI32());
            itemTemplate.bindType = row[95].GetU32();
            itemTemplate. description = row[96].GetString();
            itemTemplate.pageTextId = row[97].GetU32();
            itemTemplate.pageLanguageId = row[98].GetU32();
            itemTemplate.pageTextureId = row[99].GetU32();
            itemTemplate.startQuest = row[100].GetU32();
            itemTemplate.lockId = row[101].GetU32();
            itemTemplate.materialSound = row[102].GetI32();
            itemTemplate.weaponSheath = row[103].GetU32();
            itemTemplate.randomPropertyId = row[104].GetI32();
            itemTemplate.randomSuffixId = row[105].GetI32();
            itemTemplate.shieldBlock = row[106].GetU32();
            itemTemplate.itemSetId = row[107].GetU32();
            itemTemplate.itemDurability = row[108].GetU32();
            itemTemplate.restrictUseToAreaId = row[109].GetU32();
            itemTemplate.restrictUseToMapId = row[110].GetU32();
            itemTemplate.bagFamilyBitmask = row[111].GetU32();
            itemTemplate.toolCategoryId = row[112].GetU32();
            itemTemplate.socketInfo[0] = ItemSocketInfo(row[113].GetU32(), row[114].GetU32());
            itemTemplate.socketInfo[1] = ItemSocketInfo(row[115].GetU32(), row[116].GetU32());
            itemTemplate.socketInfo[2] = ItemSocketInfo(row[117].GetU32(), row[118].GetU32());
            itemTemplate.socketBonusId = row[119].GetU32();
            itemTemplate.gemPropertiesId = row[120].GetU32();
            itemTemplate.requiredDisenchantSkill = row[121].GetU32();
            itemTemplate.armorDamageModifier = row[122].GetF32();
            itemTemplate.limitedDuration = row[123].GetU32();
            itemTemplate.itemLimitCategory = row[124].GetU32();
            itemTemplate.holidayId = row[125].GetU32();

            _itemTemplateCache[itemTemplate.entry] = itemTemplate;
        }
    }
}
void WorldDatabaseCache::LoadAsync()
{
}

void WorldDatabaseCache::Save()
{
}
void WorldDatabaseCache::SaveAsync()
{
}


bool WorldDatabaseCache::GetItemTemplate(u32 itemEntry, ItemTemplate& output)
{
    auto cache = _itemTemplateCache.find(itemEntry);
    if (cache != _itemTemplateCache.end())
    {
        _accessMutex.lock_shared();
        ItemTemplate itemTemplate = cache->second;
        _accessMutex.unlock_shared();

        output = itemTemplate;
        return true;
    }
    
    return false;
}