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

#include "NodeSession.h"
#include <Networking\ByteBuffer.h>

std::unordered_map<uint8_t, NodeMessageHandler> NodeSession::InitMessageHandlers()
{
    std::unordered_map<uint8_t, NodeMessageHandler> messageHandlers;

    messageHandlers[NODE_CHALLENGE]             =   { NODESTATUS_CHALLENGE,         sizeof(cNodeChallenge),    &NodeSession::HandleCommandChallenge };
    messageHandlers[NODE_PROOF]                 =   { NODESTATUS_PROOF,             sizeof(cNodeProof),        &NodeSession::HandleCommandProof     };

    return messageHandlers;
}
std::unordered_map<uint8_t, NodeMessageHandler> const MessageHandlers = NodeSession::InitMessageHandlers();

bool NodeSession::Start()
{
    AsyncRead();
    return true;
}

void NodeSession::HandleRead()
{
    Common::ByteBuffer& byteBuffer = GetByteBuffer();

    while (byteBuffer.GetActualSize())
    {
        if (_status == NODESTATUS_PROOF || _status == NODESTATUS_AUTHED)
        {
            _crypto->Decrypt(byteBuffer.GetReadPointer(), 2);
        }
        uint8_t command = byteBuffer.GetDataPointer()[0];

        auto itr = MessageHandlers.find(command);
        if (itr == MessageHandlers.end())
        {
            std::cout << "Received unhandled message" << std::endl;
            byteBuffer.Clean();
            break;
        }

        // Client attempted incorrect auth step
        if (_status != itr->second.status)
        {
            _socket->close();
            return;
        }

        uint16_t size = uint16_t(itr->second.packetSize);
        if (byteBuffer.GetActualSize() < size)
            break;

        if (!(*this.*itr->second.handler)())
        {
            _socket->close();
            return;
        }

        byteBuffer.ReadBytes(size);
    }

    AsyncRead();
}

bool NodeSession::HandleCommandChallenge()
{
    std::cout << "Received NodeChallenge" << std::endl;
    _status = NODESTATUS_CLOSED;
    cNodeChallenge* nodeChallenge = reinterpret_cast<cNodeChallenge*>(GetByteBuffer().GetReadPointer());

    Common::ByteBuffer packet;
    packet.Write(0); // NODESTATUS_CHALLENGE

    if (nodeChallenge->version1 == 3 && nodeChallenge->version2 == 3 && nodeChallenge->version3 == 5 && nodeChallenge->build == 12340)
    {
        _status = NODESTATUS_PROOF;
        packet.Write(0);   
        _crypto->SetupServer(_key);
    }
    else
    {
        packet.Write(1);
        Send(packet);
        return false;
    }

    Send(packet);
    return true;
}

bool NodeSession::HandleCommandProof()
{
    std::cout << "Received NodeProof" << std::endl;
    _status = NODESTATUS_CLOSED;
    cNodeProof* nodeProof = reinterpret_cast<cNodeProof*>(GetByteBuffer().GetReadPointer());


    return true;
}