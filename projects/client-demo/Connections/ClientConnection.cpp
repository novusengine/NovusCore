#include "ClientConnection.h"
#include <Networking/ByteBuffer.h>

#pragma pack(push, 1)
struct ClientLogonChallenge
{
    u8 command;
    u8 error;
    u16 size;
    u8 gameName[4];
    u8 gameVersion1;
    u8 gameVersion2;
    u8 gameVersion3;
    u16 gameBuild;
    u8 clientPlatform[4];
    u8 clientOS[4];
    u8 clientCountry[4];
    u32 timezoneBias;
    u32 clientIP;
    u8 usernameLength;
    u8 usernamePointer[1];
};

struct ClientLogonProof
{
    u8 command;
    u8 ephemeralKey[32];
    u8 m1[20];
    u8 crcHash[20];
    u8 numberOfKeys;
    u8 securityFlags;
};

struct ClientReconnectProof
{
    u8 cmd;
    u8 r1[16];
    u8 r2[20];
    u8 r3[20];
    u8 numberOfKeys;
};
#pragma pack(pop)

std::array<u8, 16> versionChallenge = {{0xBA, 0xA3, 0x1E, 0x99, 0xA0, 0x0B, 0x21, 0x57, 0xFC, 0x37, 0x3F, 0xB3, 0x69, 0xCD, 0xD2, 0xF1}};
#define MAX_REALM_COUNT 256

robin_hood::unordered_map<u8, ClientMessageHandler> ClientConnection::InitMessageHandlers()
{
    robin_hood::unordered_map<u8, ClientMessageHandler> messageHandlers;

    messageHandlers[AUTH_RECONNECT_PROOF] = {STATUS_RECONNECT_PROOF, sizeof(ClientReconnectProof), 1, &ClientConnection::HandleCommandReconnectProof};
    messageHandlers[AUTH_REALMSERVER_LIST] = {STATUS_AUTHED, 5, 3, &ClientConnection::HandleCommandRealmserverList};

    return messageHandlers;
}
robin_hood::unordered_map<u8, ClientMessageHandler> const messageHandlers = ClientConnection::InitMessageHandlers();
robin_hood::unordered_map<u8, RealmServerData> ClientConnection::realmServerList;
std::mutex ClientConnection::realmServerListMutex;

bool ClientConnection::Start()
{
    AsyncRead();
    return true;
}

void ClientConnection::HandleRead()
{
    std::shared_ptr<ByteBuffer> buffer = GetReceiveBuffer();

    while (u32 activeSize = buffer->GetActiveSize())
    {
        u8 command = buffer->GetInternalData()[0];

        auto itr = messageHandlers.find(command);
        if (itr == messageHandlers.end() || _status != itr->second.status)
        {
            _socket->close();
            return;
        }

        u16 size = static_cast<u16>(itr->second.packetSize);
        if (activeSize < size)
            break;

        if (!(*this.*itr->second.handler)())
        {
            _socket->close();
            return;
        }

        buffer->ReadData += size;
    }

    AsyncRead();
}

bool ClientConnection::HandleCommandReconnectProof()
{
    return true;
}
bool ClientConnection::HandleCommandRealmserverList()
{
    return true;
}