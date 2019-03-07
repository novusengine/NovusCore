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

#include <asio\ip\tcp.hpp>
#include <Networking\BaseSocket.h>
#include <Networking\Opcode\Opcode.h>
#include <Cryptography\StreamCrypto.h>
#include <Cryptography\BigNumber.h>
#include <Cryptography\SHA1.h>
#include <random>

class RelayConnection : public Common::BaseSocket
{
public:
    RelayConnection(asio::ip::tcp::socket* socket) : Common::BaseSocket(socket), accountGuid(0), accountName(), _headerBuffer(), _packetBuffer()
    {
        _seed = (uint32_t)rand();
        _headerBuffer.Resize(sizeof(Common::ClientPacketHeader));
        sessionKey = new BigNumber();
    }

    bool Start() override;
    void HandleRead() override;
    void SendPacket(Common::ByteBuffer& buffer, Common::Opcode opcode);

    bool HandleHeaderRead();
    bool HandlePacketRead();

    void HandleAuthSession();

    template<size_t T>
    inline void convert(char *val)
    {
        std::swap(*val, *(val + T - 1));
        convert<T - 2>(val + 1);
    }

    template<> inline void convert<0>(char *) { }
    template<> inline void convert<1>(char *) { } // ignore central byte

    inline void apply(uint16_t *val)
    {
        convert<sizeof(uint16_t)>((char *)(val));
    }
    inline void EndianConvertReverse(uint16_t& val) { apply(&val); }

    uint64_t accountGuid;
    uint64_t characterGuid;
    std::string accountName;
    BigNumber* sessionKey;

    Common::ByteBuffer _headerBuffer;
    Common::ByteBuffer _packetBuffer;

    uint32_t _seed;
    StreamCrypto _streamCrypto;
};