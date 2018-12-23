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

#include <asio\ip\tcp.hpp>
#include <Networking\BaseSocket.h>
#include <Networking\Opcode\Opcode.h>
#include <Cryptography\StreamCrypto.h>
#include <random>

class RelaySocket : public Common::BaseSocket
{
public:
    RelaySocket(asio::ip::tcp::socket* socket) : Common::BaseSocket(socket), username(), _headerBuffer(), _packetBuffer()
    {
        _seed = (uint32_t)rand();
        _headerBuffer.Resize(sizeof(Common::ClientPacketHeader));
    }

    bool Start() override;
    void HandleRead() override;

    bool HandleHeaderRead();
    bool HandlePacketRead();

    void HandleAuthSession();

    template<size_t T>
    inline void convert(char *val)
    {
        std::swap(*val, *(val + T - 1));
    }

    inline void apply(uint16_t *val)
    {
        convert<sizeof(uint16_t)>((char *)(val));
    }
    inline void EndianConvertReverse(uint16_t& val) { apply(&val); }

    std::string username;

    Common::ByteBuffer _headerBuffer;
    Common::ByteBuffer _packetBuffer;

    uint32_t _seed;
    StreamCrypto _streamCrypto;
};