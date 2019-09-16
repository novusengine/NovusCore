#pragma once
#include "Connection.h"
#include <Utils/Message.h>
#include <Utils/DebugHandler.h>

#include "InputQueue.h"
#include "MessageHandler.h"

void Connection::Start()
{
    /*_socket->connect(asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 3724));

    cAuthLogonChallenge challenge("admin");
    u32 challengeSize = 34 + 5;

    std::shared_ptr<ByteBuffer> data = ByteBuffer::Borrow<8192>();
    data->Size = challenge.size;
    std::memcpy(data->GetInternalData(), &challenge, challengeSize);
    data->WrittenData += challengeSize;

    Send(data.get());*/

    AsyncRead();
}

void Connection::HandleRead()
{
    std::shared_ptr<ByteBuffer> buffer = GetReceiveBuffer();

    while (buffer->GetActiveSize())
    {
        u32 opcode = 0;
        u16 size = 0;

        buffer->GetU32(opcode);
        buffer->GetU16(size);

        if (opcode > OpcodeCount || size > 8192)
        {
            Close(asio::error::shut_down);
            return;
        }

        // Handle NetPacket
        NetPacket* netPacket = new NetPacket();
        netPacket->opcode = opcode;
        netPacket->data = ByteBuffer::Borrow<8192>();
        netPacket->data->Size = size;
        netPacket->data->WrittenData = size;
        netPacket->data->IsOwner = true;
        std::memcpy(netPacket->data->GetInternalData(), buffer->GetReadPointer(), size);

        Message packetMessage;
        packetMessage.code = MSG_IN_NET_PACKET;
        packetMessage.object = netPacket;

        InputQueue::PassMessage(packetMessage);

        buffer->ReadData += size;
    }

    AsyncRead();
}