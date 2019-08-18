#pragma once

struct NetPacket;
struct PlayerConnectionComponent;
class CharacterPacketUtils
{
public:
    static void RegisterCharacterPacketHandlers();

private:
    CharacterPacketUtils() {}
};