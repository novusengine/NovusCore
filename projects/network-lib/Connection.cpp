#pragma once
#include "Connection.h"

void Connection::Start()
{

}

void Connection::HandleRead()
{
    std::shared_ptr<ByteBuffer> buffer = GetReceiveBuffer();

    AsyncRead();
}