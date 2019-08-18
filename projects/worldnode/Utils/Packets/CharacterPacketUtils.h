#pragma once

struct NetPacket;
struct PlayerConnectionComponent;
class CharacterPacketUtils
{
public:
    static void RegisterCharacterPacketHandlers();

    static bool HandleSetActiveMover(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleStandStateChange(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleSetSelection(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleNameQuery(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleItemQuerySingle(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleMove(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleChatMessage(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleAttackSwing(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleAttackStop(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleSetSheathed(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleTextEmote(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleCastSpell(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleCancelAura(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);
    static bool HandleContactList(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent);

private:
    CharacterPacketUtils() {}
};