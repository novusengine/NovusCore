/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/
#pragma once

#include "BaseSocket.h"

#include <asio.hpp>
#include <asio/placeholders.hpp>
#include <Utils/ByteBuffer.h>

class BaseSocket : public std::enable_shared_from_this<BaseSocket>
{
public:
    BaseSocket(asio::ip::tcp::socket* socket) : _isClosed(false), _socket(socket), _handleRead(nullptr) { Init(); }       
    BaseSocket(asio::ip::tcp::socket* socket, std::function<void(void)> handleRead) : _isClosed(false), _socket(socket), _handleRead(handleRead) { Init(); }
    ~BaseSocket() { }

    void Init()
    {
        _receiveBuffer = ByteBuffer::Borrow<4096>();
        _sendBuffer = ByteBuffer::Borrow<4096>();
    }

    std::shared_ptr<ByteBuffer> GetReceiveBuffer() { return _receiveBuffer; }
    void _internalRead(asio::error_code errorCode, size_t bytesRead)
    {
        if (errorCode)
        {
            Close(errorCode);
            return;
        }

        _receiveBuffer->WrittenData += bytesRead;
        _handleRead();
    }
    void _internalWrite(asio::error_code errorCode, std::size_t bytesWritten)
    {
        if (errorCode)
        {
            Close(errorCode);
        }
    }
    void AsyncRead()
    {
        if (!_socket->is_open())
            return;

        _receiveBuffer->Reset();
        _socket->async_read_some(asio::buffer(_receiveBuffer->GetWritePointer(), _receiveBuffer->GetRemainingSpace()),
            std::bind(&BaseSocket::_internalRead, this, std::placeholders::_1, std::placeholders::_2));
    }
    void Send(ByteBuffer* buffer)
    {
        if (!buffer->IsEmpty() || buffer->IsFull())
        {
            _socket->async_write_some(asio::buffer(buffer->GetInternalData(), buffer->WrittenData),
                std::bind(&BaseSocket::_internalWrite, this, std::placeholders::_1, std::placeholders::_2));
        }
    }

    bool IsClosed() { return _isClosed; }
    void Close(asio::error_code error)
    {
        _socket->close();
        _isClosed = true;
    }
    asio::ip::tcp::socket* socket()
    {
        return _socket;
    }
    void SetReadHandler(std::function<void(void)> handleRead)
    {
        _handleRead = handleRead;
    }
private:
    std::shared_ptr<ByteBuffer> _receiveBuffer;
    std::shared_ptr<ByteBuffer> _sendBuffer;

    bool _isClosed = false;
    asio::ip::tcp::socket* _socket;
    std::function<void(void)> _handleRead;
};