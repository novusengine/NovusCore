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
#include <Database\DatabaseConnector.h>

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
    Common::ServerPacketHeader header(packet.size() + 2, 0x1EC);

    sendPacket.Write(header.headerArray, header.GetLength());
    sendPacket.Write(packet.data(), packet.size());

    Send(sendPacket);
    return true;
}

void RelaySocket::HandleRead()
{
    Common::ByteBuffer& buffer = GetByteBuffer();

    while (buffer.GetActualSize() > 0)
    {
        if (_headerBuffer.GetSpaceLeft() > 0)
        {
            // need to receive the header
            std::size_t readHeaderSize = std::min(buffer.GetActualSize(), _headerBuffer.GetSpaceLeft());
            _headerBuffer.Write(buffer.GetReadPointer(), readHeaderSize);
            buffer.ReadBytes(readHeaderSize);

            if (_headerBuffer.GetSpaceLeft() > 0)
            {
                // Couldn't receive the whole header this time.
                assert(buffer.GetActualSize() == 0);
                break;
            }

            // We just received nice new header
            std::cout << "Just received new header" << std::endl;
            if (!HandleHeaderRead())
            {
                Close(asio::error::shut_down);
                return;
            }
        }

        // We have full read header, now check the data payload
        if (_packetBuffer.GetSpaceLeft() > 0)
        {
            // need more data in the payload
            std::size_t readDataSize = std::min(buffer.GetActualSize(), _packetBuffer.GetSpaceLeft());
            _packetBuffer.Write(buffer.GetReadPointer(), readDataSize);
            buffer.ReadBytes(readDataSize);

            if (_packetBuffer.GetSpaceLeft() > 0)
            {
                // Couldn't receive the whole data this time.
                assert(buffer.GetActualSize() == 0);
                break;
            }
        }

        if (!HandlePacketRead())
        {
            Close(asio::error::shut_down);
            return;
        }

        _headerBuffer.ResetPos();
    }

    AsyncRead();
}

bool RelaySocket::HandleHeaderRead()
{
    // Make sure header buffer matches ClientPacketHeader size & Decrypt if required
    assert(_headerBuffer.GetActualSize() == sizeof(Common::ClientPacketHeader));
    _streamCrypto.Decrypt(_headerBuffer.GetReadPointer(), sizeof(Common::ClientPacketHeader));

    Common::ClientPacketHeader* header = reinterpret_cast<Common::ClientPacketHeader*>(_headerBuffer.GetReadPointer());
    
    // Reverse size bytes
    EndianConvertReverse(header->size);

    if (header->size < 4 && header->size > 10240 || header->command >= Common::Opcode::NUM_MSG_TYPES)
        return false;

    header->size -= sizeof(header->command);
    _packetBuffer.Resize(header->size);
    return true;
}

bool RelaySocket::HandlePacketRead()
{
    Common::ClientPacketHeader* header = reinterpret_cast<Common::ClientPacketHeader*>(_headerBuffer.GetReadPointer());
    Common::Opcode opcode = (Common::Opcode)header->command;

    switch (opcode)
    {
    case Common::Opcode::CMSG_AUTH_SESSION:
        std::cout << "Received CMSG_AUTH_SESSION Opcode: " << opcode << std::endl;
        HandleAuthSession();
        break;

    default:
        std::cout << "Received Unhandled Opcode: " << opcode << std::endl;
        break;
    }

    return true;
}

void RelaySocket::HandleAuthSession()
{
    uint32_t Build = 0;
    uint32_t LoginServerID = 0;
    std::string Account;
    uint32_t LoginServerType = 0;
    uint32_t LocalChallenge = 0;
    uint32_t RegionID = 0;
    uint32_t BattlegroupID = 0;
    uint32_t RealmID = 0;
    uint64_t DosResponse = 0;

    _packetBuffer.Read(&Build, 4);
    _packetBuffer.Read(&LoginServerID, 4);
    _packetBuffer.Read(Account);
    _packetBuffer.Read(&LoginServerType, 4);
    _packetBuffer.Read(&LocalChallenge, 4);
    _packetBuffer.Read(&RegionID, 4);
    _packetBuffer.Read(&BattlegroupID, 4);
    _packetBuffer.Read(&RealmID, 4);
    _packetBuffer.Read(&DosResponse, 8);

    std::cout << "Build: " << Build << std::endl;
    std::cout << "LoginServerID: " << LoginServerID << std::endl;
    std::cout << "Account: " << Account << std::endl;
    std::cout << "LoginServerType: " << LoginServerType << std::endl;
    std::cout << "LocalChallenge: " << LocalChallenge << std::endl;
    std::cout << "RegionID: " << RegionID << std::endl;
    std::cout << "BattlegroupID: " << BattlegroupID << std::endl;
    std::cout << "RealmID: " << RealmID << std::endl;
    std::cout << "DosResponse: " << DosResponse << std::endl;


    // Start
    Common::ByteBuffer packet(1);
    packet.Write(21); // UNK ACC
                      // End

    Common::ByteBuffer sendPacket(4096);
    sendPacket.Resize(4096);
    Common::ServerPacketHeader header(packet.size() + 2, 0x1EE);

    sendPacket.Write(header.headerArray, header.GetLength());
    sendPacket.Write(packet.data(), packet.size());

    Send(sendPacket);
    Close(asio::error::interrupted);
    return;

    // Check if account exist in DB if so, grab v and s
    std::shared_ptr<DatabaseConnector> connector;
    if (!DatabaseConnector::Borrow(DATABASE_TYPE::AUTHSERVER, connector)) { return; }

    PreparedStatement sql("SELECT k FROM accounts WHERE name={s};");
    sql.Bind(Account);

    amy::result_set results;
    if (connector->Query(sql, results) && results.affected_rows() != 2)
    {
 
    }

    /*
        -- Start
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
        -- End

        Common::ByteBuffer sendPacket(4096);
        sendPacket.Resize(4096);
        Common::ServerPacketHeader header(packet.size() + 2, 0x1EC);

        sendPacket.Write(header.headerArray, header.GetLength());
        sendPacket.Write(packet.data(), packet.size());

        Send(sendPacket);
    */

    return;
}