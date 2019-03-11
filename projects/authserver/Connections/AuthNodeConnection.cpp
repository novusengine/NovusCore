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

#include "AuthNodeConnection.h"
#include <Networking\ByteBuffer.h>

std::unordered_map<u8, AuthNodeMessageHandler> AuthNodeConnection::InitMessageHandlers()
{
    std::unordered_map<u8, AuthNodeMessageHandler> messageHandlers;

    messageHandlers[NODE_CHALLENGE]             =   { NODESTATUS_CHALLENGE,         sizeof(AuthNodeChallenge),  &AuthNodeConnection::HandleCommandChallenge };
    messageHandlers[NODE_PROOF]                 =   { NODESTATUS_PROOF,             1,                          &AuthNodeConnection::HandleCommandProof     };

    return messageHandlers;
}
std::unordered_map<u8, AuthNodeMessageHandler> const MessageHandlers = AuthNodeConnection::InitMessageHandlers();

bool AuthNodeConnection::Start()
{
    AsyncRead();
    return true;
}

void AuthNodeConnection::HandleRead()
{
    Common::ByteBuffer& byteBuffer = GetByteBuffer();

    while (byteBuffer.GetActualSize())
    {
        // Decrypt data post CHALLENGE Status
        if (_status == NODESTATUS_PROOF || _status == NODESTATUS_AUTHED)
        {
            _crypto->Decrypt(byteBuffer.GetReadPointer(), byteBuffer.GetActualSize());
        }
        u8 command = byteBuffer.GetDataPointer()[0];

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
            Close(asio::error::shut_down);
            return;
        }

        u16 size = u16(itr->second.packetSize);
        if (byteBuffer.GetActualSize() < size)
            break;

        if (!(*this.*itr->second.handler)())
        {
            Close(asio::error::shut_down);
            return;
        }

        byteBuffer.ReadBytes(size);
    }

    AsyncRead();
}

bool AuthNodeConnection::HandleCommandChallenge()
{
    _status = NODESTATUS_CLOSED;
    AuthNodeChallenge* authNodeChallenge = reinterpret_cast<AuthNodeChallenge*>(GetByteBuffer().GetReadPointer());

    if (authNodeChallenge->version == 335 && authNodeChallenge->build == 12340)
    {
        Common::ByteBuffer packet;
        packet.Write<u8>(NODE_CHALLENGE);
        packet.Append(_key->BN2BinArray(32).get(), 32);
        _crypto->SetupServer(_key);

        _status = NODESTATUS_PROOF;
        Send(packet);
        return true;
    }
        
    return false;
}

bool AuthNodeConnection::HandleCommandProof()
{
    _status = NODESTATUS_AUTHED;

    Common::ByteBuffer packet;
    packet.Write<u8>(NODE_PROOF);
    _crypto->Encrypt(packet.GetReadPointer(), packet.GetActualSize());
    
    Send(packet);
    return true;
}