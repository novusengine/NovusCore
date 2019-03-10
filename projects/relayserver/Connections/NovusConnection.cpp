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
    messageHandlers[NOVUS_FOWARDPACKET]          =   { NOVUSSTATUS_AUTHED,            9,                        &NovusConnection::HandleCommandForwardPacket    };

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
    Common::ByteBuffer& buffer = GetByteBuffer();

    bool isDecrypted = false;
    while (buffer.GetActualSize())
    {
        // Decrypt data post CHALLENGE Status
        if (!isDecrypted && (_status == NOVUSSTATUS_PROOF || _status == NOVUSSTATUS_AUTHED))
        {
            _crypto->Decrypt(buffer.GetReadPointer(), buffer.GetActualSize());
            isDecrypted = true;
        }
        uint8_t command = buffer.GetDataPointer()[0];

        auto itr = MessageHandlers.find(command);
        if (itr == MessageHandlers.end())
        {
            std::cout << "Received HandleRead with no MessageHandler to respond." << std::endl;
            buffer.Clean();
            break;
        }

        if (_status != itr->second.status)
        {
            Close(asio::error::shut_down);
            return;
        }

        if (command == NOVUS_FOWARDPACKET)
        {
            // Check if we should read header
            if (_headerBuffer.GetSpaceLeft() > 0)
            {
                size_t headerSize = std::min(buffer.GetActualSize(), _headerBuffer.GetSpaceLeft());
                _headerBuffer.Write(buffer.GetReadPointer(), headerSize);
                buffer.ReadBytes(headerSize);

                if (_headerBuffer.GetSpaceLeft() > 0)
                {
                    // Wait until we have the entire header
                    assert(buffer.GetActualSize() == 0);
                    break;
                }

                /* Read Header */
                NovusHeader* header = reinterpret_cast<NovusHeader*>(_headerBuffer.GetReadPointer());
                _packetBuffer.Resize(header->size);
                _packetBuffer.ResetPos();
            }

            // We have a header, now check the packet data
            if (_packetBuffer.GetSpaceLeft() > 0)
            {
                std::size_t packetSize = std::min(buffer.GetActualSize(), _packetBuffer.GetSpaceLeft());
                _packetBuffer.Write(buffer.GetReadPointer(), packetSize);
                buffer.ReadBytes(packetSize);

                if (_packetBuffer.GetSpaceLeft() > 0)
                {
                    // Wait until we have all of the packet data
                    assert(buffer.GetActualSize() == 0);
                    break;
                }
            }

            if (!HandleCommandForwardPacket())
            {
                Close(asio::error::shut_down);
                return;
            }
            _headerBuffer.ResetPos();
        }
        else
        {
            uint16_t size = uint16_t(itr->second.packetSize);
            if (buffer.GetActualSize() < size)
                break;

            if (!(*this.*itr->second.handler)())
            {
                Close(asio::error::shut_down);
                return;
            }

            buffer.ReadBytes(size);
        }
    }

    AsyncRead();
}

bool NovusConnection::HandleCommandChallenge()
{
    _status = NOVUSSTATUS_CLOSED;
    cNovusChallenge* novusChallenge = reinterpret_cast<cNovusChallenge*>(GetByteBuffer().GetReadPointer());

    if (novusChallenge->version == 335 && novusChallenge->build == 12340)
    {
        Common::ByteBuffer packet;
        packet.Write<uint8_t>(NOVUS_CHALLENGE);
        packet.Append(_key->BN2BinArray(32).get(), 32);
        _crypto->SetupServer(_key);

        _status = NOVUSSTATUS_PROOF;
        _type = novusChallenge->type;

        if (_type == 1)
            _id = 1;

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
    NovusHeader* header = reinterpret_cast<NovusHeader*>(_headerBuffer.GetReadPointer());
    std::cout << "Received opcode: 0x" << std::hex << std::uppercase << header->opcode << std::endl;

    if (RelayConnection* playerConnection = ClientRelayConnectionHandler::GetConnectionByAccountGuid(header->account))
    {
        Common::ByteBuffer packetBuffer;
        packetBuffer.Append(_packetBuffer.GetReadPointer(), _packetBuffer.GetActualSize());
        playerConnection->SendPacket(packetBuffer, Common::Opcode(header->opcode));
    }
    //std::cout << std::endl;

    return true;
}