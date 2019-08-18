#pragma once

struct NetPacket;
struct PlayerConnectionComponent;
class AccountPacketUtils
{
public:
    static void RegisterAccountPacketHandlers();
    static bool HandleReadyForAccountDataTimes(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleRequestAccountData(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleUpdateAccountData(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);

private:
    AccountPacketUtils() {}
};