#pragma once
#include <NovusTypes.h>

enum class HighGuid
{
    Item = 0x4000,
    Container = 0x4000,
    Player = 0x0000,
    GameObject = 0xF110,
    Transport = 0xF120,
    Unit = 0xF130,
    Pet = 0xF140,
    Vehicle = 0xF150,
    DynamicObject = 0xF100,
    Corpse = 0xF101,
    MoTransport = 0x1FC0,
    Instance = 0x1F40,
    Group = 0x1F50,
};

class ObjectGuid
{
public:
    ObjectGuid() : _objectGuid(0) {}
    ObjectGuid(HighGuid highPart, u32 counter) : _objectGuid(u64(counter) | (u64(highPart) << 48)) {}
    ObjectGuid(HighGuid highPart, u32 entry, u32 counter) : _objectGuid(u64(counter) | u64(entry) << 24 | (u64(highPart) << 48)) {}

    u64 GetGuid() const { return _objectGuid; }
    HighGuid GetHighType() const { return HighGuid((_objectGuid >> 48) & 0xFFFF); }
    u32 GetEntry() const { return u32((_objectGuid >> 24) & 0xFFFFFF); }
    u32 GetCounter() const { return HasEntry() ? u32(_objectGuid & 0xFFFFFF) : u32(_objectGuid & 0xFFFFFFFF); }

    bool HasEntry() const
    {
        switch (GetHighType())
        {
        case HighGuid::Item:
        case HighGuid::GameObject:
        case HighGuid::Transport:
        case HighGuid::Unit:
        case HighGuid::Pet:
        case HighGuid::Vehicle:
            return true;
        }

        return false;
    }

    bool IsPlayer() const { return GetHighType() == HighGuid::Player; }
    bool IsItem() const { return GetHighType() == HighGuid::Item; }
    bool IsUnit() const { return GetHighType() == HighGuid::Unit; }

    operator u64() const { return _objectGuid; }
    bool operator!() const { return _objectGuid == 0; }
    bool operator==(ObjectGuid const& objectGuid) const { return _objectGuid == objectGuid; }
    bool operator!=(ObjectGuid const& objectGuid) const { return _objectGuid != objectGuid; }

private:
    u64 _objectGuid;
};