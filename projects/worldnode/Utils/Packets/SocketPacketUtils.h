#pragma once

struct NetPacket;
struct PlayerConnectionComponent;
class SocketPacketUtils
{
public:
    static void RegisterSocketPacketHandlers();
    static bool HandleLogoutRequest(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);

private:
    SocketPacketUtils() {}
};