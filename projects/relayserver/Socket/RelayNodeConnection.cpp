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

#include "RelayNodeConnection.h"
#include "Networking\ByteBuffer.h"

std::unordered_map<uint8_t, RelayMessageHandler> RelayNodeConnection::InitMessageHandlers()
{
    std::unordered_map<uint8_t, RelayMessageHandler> messageHandlers;

    messageHandlers[RELAY_CHALLENGE] =  { RELAYSTATUS_CHALLENGE,    sizeof(sRelayChallenge),    &RelayNodeConnection::HandleCommandChallenge };
    messageHandlers[RELAY_PROOF]     =  { RELAYSTATUS_PROOF,        sizeof(sRelayProof),        &RelayNodeConnection::HandleCommandProof };

    return messageHandlers;
}
std::unordered_map<uint8_t, RelayMessageHandler> const MessageHandlers = RelayNodeConnection::InitMessageHandlers();

bool RelayNodeConnection::Start()
{
    try
    {
        _socket->connect(asio::ip::tcp::endpoint(asio::ip::address::from_string(_address), _port));

        AsyncRead();

        /* NODE_CHALLENGE */
        Common::ByteBuffer packet(6);

        packet.Write(0);
        packet.Write(3);
        packet.Write(3);
        packet.Write(5);
        uint16_t build = 12340;
        packet.Append((uint8_t*)&build, sizeof(build));

        Send(packet);
        return true;
    }
    catch (asio::system_error error)
    {
        std::cout << "ERROR: " << error.what() << std::endl;
        return false;
    }
}

void RelayNodeConnection::HandleRead()
{
    Common::ByteBuffer& byteBuffer = GetByteBuffer();
    while (byteBuffer.GetActualSize())
    {
        // Decrypt data post CHALLENGE Status
        if (_status == RELAYSTATUS_PROOF || _status == RELAYSTATUS_AUTHED)
        {
            _crypto->Decrypt(byteBuffer.GetReadPointer(), byteBuffer.GetActualSize());
        }

        uint8_t command = byteBuffer.GetDataPointer()[0];

        auto itr = MessageHandlers.find(command);
        if (itr == MessageHandlers.end())
        {
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

        byteBuffer.ReadBytes(size);
    }

    AsyncRead();
}


bool RelayNodeConnection::HandleCommandChallenge()
{
    std::cout << "Received RelayChallenge" << std::endl;
    _status = RELAYSTATUS_CLOSED;
    sRelayChallenge* relayChallenge = reinterpret_cast<sRelayChallenge*>(GetByteBuffer().GetReadPointer());

    _status = RELAYSTATUS_PROOF;
    _key->Bin2BN(relayChallenge->K, 32);
    _crypto->SetupClient(_key);

    /* Send fancy encrypted packet here */
    Common::ByteBuffer packet;
    packet.Write(RELAY_PROOF); // RELAY_PROOF
    packet.Write(99);
    _crypto->Encrypt(packet.GetReadPointer(), 2);

    Send(packet);
    return true;
}

bool RelayNodeConnection::HandleCommandProof()
{
    std::cout << "Received RelayProof" << std::endl;
    _status = RELAYSTATUS_CLOSED;


    return true;
}