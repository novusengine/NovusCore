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
#include <Cryptography\BigNumber.h>
#include <Cryptography\SHA1.h>
#include <Cryptography\StreamCrypto.h>
#include <unordered_map>

enum NodeCommand
{
    NODE_CHALLENGE              = 0x00,
    NODE_PROOF                  = 0x01
};
enum NodeStatus
{
    NODESTATUS_CHALLENGE        = 0,
    NODESTATUS_PROOF            = 1,
    NODESTATUS_AUTHED           = 2,
    NODESTATUS_CLOSED           = 3
};

#pragma pack(push, 1)
struct cNodeChallenge
{
    uint8_t   command;
    uint8_t   version1;
    uint8_t   version2;
    uint8_t   version3;
    uint16_t  build;
};

struct cNodeProof
{
    uint8_t   command;
    uint8_t   unk;
};
#pragma pack(pop)

struct NodeMessageHandler;
class NodeSession : public Common::BaseSocket
{
public:
    static std::unordered_map<uint8_t, NodeMessageHandler> InitMessageHandlers();

    NodeSession(asio::ip::tcp::socket* socket) : Common::BaseSocket(socket), _status(NODESTATUS_CHALLENGE), _crypto()
    {
        _crypto = new StreamCrypto();
        _key = new BigNumber();
        _key->SetUInt32(100);
    }

    bool Start() override;
    void HandleRead() override;

    bool HandleCommandChallenge();
    bool HandleCommandProof();

    NodeStatus _status;

    private:
    StreamCrypto* _crypto;
    BigNumber* _key;
};

#pragma pack(push, 1)
struct NodeMessageHandler
{
    NodeStatus status;
    size_t packetSize;
    bool (NodeSession::*handler)();
};
#pragma pack(pop)