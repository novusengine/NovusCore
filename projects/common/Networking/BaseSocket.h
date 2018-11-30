/*
# MIT License

# Copyright(c) 2018 NovusCore

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

#include <ctime>
#include <iostream>
#include <string>
#include <functional>
#include <asio.hpp>
#include <asio\placeholders.hpp>
#include "ByteBuffer.h"

namespace Common
{
    class BaseSocket : public std::enable_shared_from_this<BaseSocket>
    {
    public:
        virtual bool Start() = 0;
        virtual void HandleRead() = 0;

        asio::ip::tcp::socket* socket()
        {
            return _socket;
        }

        void Send(ByteBuffer& buffer)
        {
            if (!buffer.empty())
            {
                //printf("Sending Packet to client: size(%u)\n", buffer.GetBufferSize());
                _socket->async_write_some(asio::buffer(buffer.GetReadPointer(), buffer.GetActualSize()),
                   std::bind(&BaseSocket::HandleInternalWrite, this, std::placeholders::_1, std::placeholders::_2));
            }
        }

    protected:
        BaseSocket(asio::ip::tcp::socket* socket) : _socket(socket), _byteBuffer() 
        { 
            _byteBuffer.Resize(4096);
        }

        void AsyncRead()
        {
            // Ensure valid connection bound to the socket
            if (!_socket->is_open())
                return;

            _byteBuffer.CleanBuffer();
            _byteBuffer.RecalculateSize();
            _socket->async_read_some(asio::buffer(_byteBuffer.GetWritePointer(), _byteBuffer.GetSpaceLeft()),
                std::bind(&BaseSocket::HandleInternalRead, this, std::placeholders::_1, std::placeholders::_2));
        }
        void HandleInternalRead(asio::error_code errorCode, size_t bytes)
        {
            if (errorCode)
            {
                printf("HandleInternalRead: Error %s\n", errorCode.message().c_str());
                _socket->close();
                return;
            }

            _byteBuffer.WriteBytes(bytes);
            HandleRead();
        }        
        void HandleInternalWrite(asio::error_code error, std::size_t transferedBytes)
        {
            if (!error)
            {
            }
            else
            {
                printf("WRITE ERROR\n");
                _socket->close();
            }
        }

        ByteBuffer& GetByteBuffer() { return _byteBuffer; }
        ByteBuffer _byteBuffer;
        asio::ip::tcp::socket* _socket;
    };
}