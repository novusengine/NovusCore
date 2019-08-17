#include <NovusTypes.h>
#include <Networking/ByteBuffer.h>
#include <Database\DatabaseConnector.h>
#include "../../NovusEnums.h"
#include "../../MessageHandler.h"
#include "../../RealmHandler.h"
#include "../../Connections/RealmConnection.h"

namespace SocketPacketUtils
{
bool HandleAuthSession(RealmConnection* connection)
{
    // Should the content of HandleAuthSession be pushed in here?
    if (connection->account == 0)
    {
        connection->HandleAuthSession();
        return true;
    }

    return false;
}
bool HandleRealmSplit(RealmConnection* connection)
{
    ByteBuffer& buffer = connection->GetPacketBuffer();

    u32 unk = 0;
    buffer.GetU32(unk);

    std::shared_ptr<ByteBuffer> realmSplit = ByteBuffer::Borrow<32>();
    realmSplit->PutU32(unk);
    realmSplit->PutU32(0x0); // split states: 0x0 realm normal, 0x1 realm split, 0x2 realm split pending
    realmSplit->PutString("01/01/01");

    connection->SendPacket(realmSplit.get(), Opcode::SMSG_REALM_SPLIT);
    return true;
}
bool HandlePing(RealmConnection* connection)
{
    ByteBuffer& buffer = connection->GetPacketBuffer();

    u32 ping = 0, latency = 0;
    buffer.GetU32(ping);
    buffer.GetU32(latency);

    // Here we can figure out the time since last ping to ensure that the client is checking in at the proper frequency

    std::shared_ptr<ByteBuffer> responseBuffer = ByteBuffer::Borrow<4>();
    responseBuffer->PutU32(ping);
    connection->SendPacket(responseBuffer.get(), Opcode::SMSG_PONG);
    return true;
}
bool HandleKeepAlive(RealmConnection* connection)
{
    return true;
}
bool HandleConnectToFailed(RealmConnection* connection)
{
    std::shared_ptr<DatabaseConnector> connector = nullptr;
    if (!DatabaseConnector::Borrow(DATABASE_TYPE::CHARSERVER, connector))
    {
        connection->Close(asio::error::interrupted);
        return false;
    }

    PreparedStatement stmt("UPDATE characters SET online=0 WHERE guid={u};");
    stmt.Bind(connection->characterGuid);
    connector->Execute(stmt);

    std::shared_ptr<ByteBuffer> loginFailed = ByteBuffer::Borrow<1>();
    loginFailed->PutU8(EnterWorldResults::ENTER_FAILED_WORLDSERVER_DOWN);
    connection->SendPacket(loginFailed.get(), Opcode::SMSG_CHARACTER_LOGIN_FAILED);

    connection->enteringWorld = false;
    return true;
}
bool HandleSuspendCommsAck(RealmConnection* connection)
{
    /*
    u32 response = 0;
    _packetBuffer.GetU32(response);
    */

    return true;
}

void RegisterSocketPacketHandlers()
{
    MessageHandler* messageHandler = MessageHandler::Instance();

    messageHandler->SetMessageHandler(Opcode::CMSG_AUTH_SESSION, HandleAuthSession);
    messageHandler->SetMessageHandler(Opcode::CMSG_REALM_SPLIT, HandlePing);
    messageHandler->SetMessageHandler(Opcode::CMSG_PING, HandlePing);
    messageHandler->SetMessageHandler(Opcode::CMSG_KEEP_ALIVE, HandleKeepAlive);
    messageHandler->SetMessageHandler(Opcode::CMSG_CONNECT_TO_FAILED, HandleConnectToFailed);
    messageHandler->SetMessageHandler(Opcode::CMSG_SUSPEND_COMMS_ACK, HandleSuspendCommsAck);
}
} // namespace SocketPacketUtils