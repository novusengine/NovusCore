#pragma once
#include <NovusTypes.h>
#include "Connection.h"
#include <Utils/Message.h>
#include <Utils/DebugHandler.h>

#include "InputQueue.h"
#include "MessageHandler.h"

void Connection::Start()
{
    try
    {
        _baseSocket->socket()->connect(asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 3724));
    }
    catch (std::exception e)
    {

        Message packetMessage;
        packetMessage.code = MSG_IN_PRINT;
        packetMessage.message = new std::string("Failed to connect to: 127.0.0.1:3724");

        InputQueue::PassMessage(packetMessage);
    }
    _baseSocket->AsyncRead();
}

void Connection::HandleRead()
{
    std::shared_ptr<ByteBuffer> buffer = _baseSocket->GetReceiveBuffer();

    while (buffer->GetActiveSize())
    {
        u32 opcode = 0;
        u16 size = 0;

        buffer->GetU32(opcode);
        buffer->GetU16(size);

        if (opcode > OpcodeCount || size > 8192)
        {
            _baseSocket->Close(asio::error::shut_down);
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
        buffer->ReadData += size;

        Message packetMessage;
        packetMessage.code = MSG_IN_NET_PACKET;
        packetMessage.object = netPacket;

        InputQueue::PassMessage(packetMessage);
    }

    _baseSocket->AsyncRead();
}