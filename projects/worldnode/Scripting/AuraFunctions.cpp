#include "AuraFunctions.h"
#include "../Game/Aura.h"

void AngelScriptAura::GetData(std::string& /*key*/, void* /*ref*/, int /*typeId*/) const
{
    /*u32 keyHash = StringUtils::fnv1a_32(key.c_str(), key.size());
    ScriptDataStorageComponent& dataStorageComponent = _registry->get<ScriptDataStorageComponent>(_entityId);

    size_t size = 0;
    switch (typeId)
    {
    // 8 bits
    case asTYPEID_BOOL:
    case asTYPEID_INT8:
    case asTYPEID_UINT8:
        size = 1;
        break;
    // 16 bits
    case asTYPEID_INT16:
    case asTYPEID_UINT16:
        size = 2;
        break;
    // 32 bits
    case asTYPEID_INT32:
    case asTYPEID_UINT32:
    case asTYPEID_FLOAT:
        size = 4;
        break;
    // 64 bits
    case asTYPEID_INT64:
    case asTYPEID_UINT64:
    case asTYPEID_DOUBLE:
        size = 8;
        break;
    default:
        assert(false); // Unsupported datatype
        break;
    }

    memcpy(ref, &dataStorageComponent.data[keyHash], size);*/
}

void AngelScriptAura::SetData(std::string& /*key*/, void* /*ref*/, int /*typeId*/) const
{
    /*u32 keyHash = StringUtils::fnv1a_32(key.c_str(), key.size());
    ScriptDataStorageComponent& dataStorageComponent = _registry->get<ScriptDataStorageComponent>(_entityId);

    u64 value;

    switch (typeId)
    {
    // 8 bits
    case asTYPEID_BOOL:
    case asTYPEID_INT8:
    case asTYPEID_UINT8:
        value = *static_cast<u8*>(ref);
        dataStorageComponent.data[keyHash] = value;
        break;
    // 16 bits
    case asTYPEID_INT16:
    case asTYPEID_UINT16:
        value = *static_cast<u16*>(ref);
        dataStorageComponent.data[keyHash] = value;
        break;
    // 32 bits
    case asTYPEID_INT32:
    case asTYPEID_UINT32:
    case asTYPEID_FLOAT:
        value = *static_cast<u32*>(ref);
        dataStorageComponent.data[keyHash] = value;
        break;
    // 64 bits
    case asTYPEID_INT64:
    case asTYPEID_UINT64:
    case asTYPEID_DOUBLE:
        value = *static_cast<u64*>(ref);
        dataStorageComponent.data[keyHash] = value;
        break;
    default:
        assert(false); // Unsupported datatype
        break;
    }*/
}

i32 AngelScriptAura::GetMiscValueA() const
{
    Aura* aura = GetAura();
    return aura->spellData.EffectMiscValueA[aura->effectIndex];
}
i32 AngelScriptAura::GetValue() const
{
    Aura* aura = GetAura();
    return aura->spellData.EffectBasePoints[aura->effectIndex];
}
u32 AngelScriptAura::GetSpellId() const
{
    Aura* aura = GetAura();
    return aura->spellData.Id;
}