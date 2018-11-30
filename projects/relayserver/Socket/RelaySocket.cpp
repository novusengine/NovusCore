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

#include "RelaySocket.h"
#include "Networking\ByteBuffer.h"
#include <Cryptography\BigNumber.h>

bool RelaySocket::Start()
{
    AsyncRead();

    /* SMSG_AUTH_CHALLENGE */
    Common::ByteBuffer packet(37);
    uint32_t unk1 = 1;
    packet.Append((uint8_t*)&unk1, sizeof(unk1));
    packet.Append((uint8_t*)&_seed, sizeof(_seed));

    BigNumber s1;
    s1.Rand(16 * 8);
    packet.Append(s1.BN2BinArray(16).get(), 16);

    BigNumber s2;
    s2.Rand(16 * 8);
    packet.Append(s2.BN2BinArray(16).get(), 16);

    Common::ByteBuffer sendPacket(4096);
    sendPacket.Resize(4096);
    Common::ServerMessageHeader header(packet.size() + 2, 0x1EC);

    sendPacket.Write(header.header, header.getHeaderLength());
    sendPacket.Write(packet.data(), packet.size());

    Send(sendPacket);
    return true;
}

void RelaySocket::HandleRead()
{
    std::cout << "HandleRead()" << std::endl;
    while (_byteBuffer.GetActualSize() > 0)
    {
        if (_headerBuffer.GetSpaceLeft() > 0)
        {
            // need to receive the header
            std::size_t readHeaderSize = std::min(_byteBuffer.GetActualSize(), _headerBuffer.GetSpaceLeft());
            _headerBuffer.Write(_byteBuffer.GetReadPointer(), readHeaderSize);
            _byteBuffer.ReadBytes(readHeaderSize);

            if (_headerBuffer.GetSpaceLeft() > 0)
            {
                // Couldn't receive the whole header this time.
                assert(_byteBuffer.GetActualSize() == 0);
                break;
            }

            // We just received nice new header
            std::cout << "Just received new header" << std::endl;

            assert(_headerBuffer.GetActualSize() == sizeof(Common::ClientMessageHeader));
            _streamCrypto.Decrypt(_headerBuffer.GetReadPointer(), sizeof(Common::ClientMessageHeader));

            Common::ClientMessageHeader* header = reinterpret_cast<Common::ClientMessageHeader*>(_headerBuffer.GetReadPointer());

            std::cout << "Header: Size(" << header->size << "), command(" << header->command << ")" << std::endl;
        }

        // We have full read header, now check the data payload
        if (_packetBuffer.GetSpaceLeft() > 0)
        {
            // need more data in the payload
            std::size_t readDataSize = std::min(_byteBuffer.GetActualSize(), _packetBuffer.GetSpaceLeft());
            _packetBuffer.Write(_byteBuffer.GetReadPointer(), readDataSize);
            _byteBuffer.ReadBytes(readDataSize);

            if (_packetBuffer.GetSpaceLeft() > 0)
            {
                // Couldn't receive the whole data this time.
                assert(_byteBuffer.GetActualSize() == 0);
                break;
            }
        }

        Common::ClientMessageHeader* header = reinterpret_cast<Common::ClientMessageHeader*>(_headerBuffer.GetReadPointer());
        std::cout << "Just received full packet > Command: " << header->command << ", Size: " << header->size << std::endl;

        _headerBuffer.ResetPos();
    }

    AsyncRead();
}