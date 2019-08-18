#pragma once

struct NetPacket;
struct PlayerConnectionComponent;
class GamemasterPacketUtils
{
public:
    static void RegisterGamemasterPacketHandlers();

    static bool HandleSetAllSpeedCheat(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleStartSwimCheat(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleStopSwimCheat(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);

    static bool HandleQueryObjectPosition(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleLevelCheat(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);

private:
    GamemasterPacketUtils() {}
};