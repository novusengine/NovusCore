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

#include "NovusConnection.h"
#include <Networking\ByteBuffer.h>
#include "..\ConnectionHandlers\ClientRelayConnectionHandler.h"

std::unordered_map<uint8_t, NovusMessageHandler> NovusConnection::InitMessageHandlers()
{
    std::unordered_map<uint8_t, NovusMessageHandler> messageHandlers;

    messageHandlers[NOVUS_CHALLENGE]             =   { NOVUSSTATUS_CHALLENGE,         sizeof(cNovusChallenge),  &NovusConnection::HandleCommandChallenge        };
    messageHandlers[NOVUS_PROOF]                 =   { NOVUSSTATUS_PROOF,             1,                        &NovusConnection::HandleCommandProof            };
    messageHandlers[NOVUS_FOWARDPACKET]          =   { NOVUSSTATUS_AUTHED,            1,                        &NovusConnection::HandleCommandForwardPacket    };

    return messageHandlers;
}
std::unordered_map<uint8_t, NovusMessageHandler> const MessageHandlers = NovusConnection::InitMessageHandlers();

bool NovusConnection::Start()
{
    AsyncRead();
    return true;
}

void NovusConnection::HandleRead()
{
    Common::ByteBuffer& byteBuffer = GetByteBuffer();

    while (byteBuffer.GetActualSize())
    {
        // Decrypt data post CHALLENGE Status
        if (_status == NOVUSSTATUS_PROOF || _status == NOVUSSTATUS_AUTHED)
        {
            _crypto->Decrypt(byteBuffer.GetReadPointer(), byteBuffer.GetActualSize());
        }
        uint8_t command = byteBuffer.GetDataPointer()[0];

        auto itr = MessageHandlers.find(command);
        if (itr == MessageHandlers.end())
        {
            std::cout << "Received HandleRead with no MessageHandler to respond." << std::endl;
            byteBuffer.Clean();
            break;
        }

        // Client attempted incorrect auth step
        if (_status != itr->second.status)
        {
            Close(asio::error::shut_down);
            return;
        }

        uint16_t size = uint16_t(itr->second.packetSize);
        if (byteBuffer.GetActualSize() < size)
            break;

        if (!(*this.*itr->second.handler)())
        {
            Close(asio::error::shut_down);
            return;
        }

        byteBuffer.ReadBytes(byteBuffer.GetActualSize());
    }

    AsyncRead();
}

bool NovusConnection::HandleCommandChallenge()
{
    _status = NOVUSSTATUS_CLOSED;
    cNovusChallenge* novusChallenge = reinterpret_cast<cNovusChallenge*>(GetByteBuffer().GetReadPointer());

    if (novusChallenge->version == 335 && novusChallenge->build == 12340)
    {
        _type = novusChallenge->type;

        Common::ByteBuffer packet;
        packet.Write<uint8_t>(NOVUS_CHALLENGE);
        packet.Append(_key->BN2BinArray(32).get(), 32);
        _crypto->SetupServer(_key);

        _status = NOVUSSTATUS_PROOF;
        Send(packet);
        return true;
    }
        
    return false;
}

bool NovusConnection::HandleCommandProof()
{
    _status = NOVUSSTATUS_AUTHED;

    Common::ByteBuffer packet;
    packet.Write<uint8_t>(NOVUS_PROOF);
    _crypto->Encrypt(packet.GetReadPointer(), packet.GetActualSize());
    
    Send(packet);
    return true;
}

bool NovusConnection::HandleCommandForwardPacket()
{
    Common::ByteBuffer& byteBuffer = GetByteBuffer();
    byteBuffer.ReadBytes(1); // Skip NovusCommand

    uint64_t accountGuid = 0;
    uint16_t clientOpcode = 0;
    byteBuffer.Read(&accountGuid, 8);
    byteBuffer.Read(&clientOpcode, 2);

    std::cout << "Received HandleCommandForwardPacket(" << accountGuid << "," << clientOpcode << "," << byteBuffer.GetActualSize() << ")" << std::endl;

    if (RelayConnection* playerConnection = ClientRelayConnectionHandler::GetConnectionByAccountGuid(accountGuid))
    {
        Common::ByteBuffer packetBuffer;
        packetBuffer.Append(byteBuffer.GetReadPointer(), byteBuffer.GetActualSize());
        playerConnection->SendPacket(packetBuffer, Common::Opcode(clientOpcode));
    }
    std::cout << std::endl;

    return true;
}