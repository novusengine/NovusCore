#include "SocketPacketUtils.h"
#include "../../MessageHandler.h"
#include "../../WorldNodeHandler.h"
#include "../../ECS/Components/Singletons/SingletonComponent.h"
#include "../../ECS/Components/PlayerConnectionComponent.h"

#include "../../Utils/ServiceLocator.h"
#include <tracy/Tracy.hpp>

void SocketPacketUtils::RegisterSocketPacketHandlers()
{
    MessageHandler* messageHandler = MessageHandler::Instance();

    messageHandler->SetMessageHandler(Opcode::CMSG_LOGOUT_REQUEST, HandleLogoutRequest);
}

bool SocketPacketUtils::HandleLogoutRequest(NetPacket* packet, PlayerConnectionComponent* playerConnectionComponent)
{
    ZoneScopedNC("Packet::LogoutRequest", tracy::Color::Orange2)
        // Here we need to Redirect the client back to Realmserver. The Realmserver will send SMSG_LOGOUT_COMPLETE
    SingletonComponent& singletonComponent = ServiceLocator::GetMainRegistry()->ctx<SingletonComponent>();
    
    i32 ip = singletonComponent.worldNodeHandler->realmserverAddress;
    i16 port = singletonComponent.worldNodeHandler->realmserverPort;

    std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<30>();
    buffer->PutI32(ip);
    buffer->PutI16(port);
    buffer->PutI32(0); // unk
#pragma warning(push)
#pragma warning(disable : 4312)
    HMACH hmac(40, playerConnectionComponent->socket->sessionKey.BN2BinArray(20).get());
    hmac.UpdateHash((u8*)&ip, 4);
    hmac.UpdateHash((u8*)&port, 2);
    hmac.Finish();
    buffer->PutBytes(hmac.GetData(), 20);
#pragma warning(pop)

    playerConnectionComponent->socket->SendPacket(buffer.get(), Opcode::SMSG_REDIRECT_CLIENT);
    return true;
}
