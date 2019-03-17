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
#include <Networking\TcpServer.h>
#include <Networking/Opcode/Opcode.h>
#include <Cryptography\BigNumber.h>
#include <Cryptography\StreamCrypto.h>
#include <robin_hood.h>
#include <bitset>
#include <zlib.h>

enum NovusCommand
{
    NOVUS_CHALLENGE         = 0x00,
    NOVUS_PROOF             = 0x01,
    NOVUS_FORWARDPACKET      = 0x02
};
enum NovusStatus
{
    NOVUSSTATUS_CHALLENGE   = 0x0,
    NOVUSSTATUS_PROOF       = 0x1,
    NOVUSSTATUS_AUTHED      = 0x2,
    NOVUSSTATUS_CLOSED      = 0x3
};


#define CLIENT_UPDATE_MASK_BITS 32
template <size_t size>
class UpdateMask
{
public:
    UpdateMask(u32 valuesCount)
    {
        Reset();

        _fieldCount = valuesCount;
        _blockCount = (valuesCount + CLIENT_UPDATE_MASK_BITS - 1) / CLIENT_UPDATE_MASK_BITS;
    }

    void SetBit(u32 index)
    {
        _bits.set(index);
    }
    void UnsetBit(u32 index)
    {
        _bits.reset(index);
    }
    bool IsSet(u32 index)
    {
        return _bits.test(index);
    }
    void Reset()
    {
        _bits.reset();
    }
    bool Any()
    {
        return _bits.any();
    }

    void AddTo(Common::ByteBuffer& buffer)
    {
        u32 maskPart = 0;
        for (u32 i = 0; i < GetBlocks() * 32; i++)
        {
            if (IsSet(i))
                maskPart |= 1 << i % 32;

            if ((i + 1) % 32 == 0)
            {
                buffer.Write<u32>(maskPart);
                maskPart = 0;
            }
        }
    }

    u32 GetFields() { return _fieldCount; }
    u32 GetBlocks() { return _blockCount; }

private:
    u32 _fieldCount;
    u32 _blockCount;
    std::bitset<size> _bits;
};

class UpdateData
{
public:
    UpdateData() : _blockCount(0) { }

    bool IsEmpty() { return _data.empty() && _nonVisibleGuids.empty(); }

    void AddBlock(Common::ByteBuffer const& block)
    {
        _data.Append(block);
        ++_blockCount;
    }
    void AddGuid(u64 guid)
    {
        _nonVisibleGuids.push_back(guid);
    }

    bool Build(Common::ByteBuffer& packet, u16& opcode)
    {
        Common::ByteBuffer buffer((4 + _nonVisibleGuids.empty() ? 0 : 1 + 4 + 9 * _nonVisibleGuids.size()) + _data._writePos);
        buffer.Write<u32>(_nonVisibleGuids.empty() ? _blockCount : _blockCount + 1);

        if (!_nonVisibleGuids.empty())
        {
            buffer.Write<u8>(4); // UPDATETYPE_OUT_OF_RANGE_OBJECTS
            buffer.Write<u32>(u32(_nonVisibleGuids.size()));

            for (u64 guid : _nonVisibleGuids)
            {
                buffer.AppendGuid(guid);
            }
        }

        buffer.Append(_data);
        size_t pSize = buffer._writePos;

        if (pSize > 100)
        {
            u32 destsize = compressBound((uLong)pSize);
            packet.Resize(destsize + sizeof(u32));
            packet._writePos = packet.size();

            packet.Replace<u32>(0, (u32)pSize);
            Compress(const_cast<u8*>(packet.data()) + sizeof(u32), &destsize, (void*)buffer.data(), (int)pSize);
            if (destsize == 0)
                return false;

            packet.Resize(destsize + sizeof(u32));
            packet._writePos = packet.size();
            opcode = Common::Opcode::SMSG_COMPRESSED_UPDATE_OBJECT;
        }
        else
        {
            packet.Append(buffer);
            packet._writePos = packet.size();
            opcode = Common::Opcode::SMSG_UPDATE_OBJECT;
        }

        return true;
    }

private:
    u32 _blockCount;
    std::vector<u64> _nonVisibleGuids;
    Common::ByteBuffer _data;

    void Compress(void* dst, u32 *dst_size, void* src, int src_size)
    {
        z_stream c_stream;

        c_stream.zalloc = (alloc_func)nullptr;
        c_stream.zfree = (free_func)nullptr;
        c_stream.opaque = (voidpf)nullptr;

        // default Z_BEST_SPEED (1)
        int z_res = deflateInit(&c_stream, 1); // 1 - 9 (9 Being best)
        if (z_res != Z_OK)
        {
            std::cout << "Can't compress update packet (zlib: deflateInit) Error code: %i (%s)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflateInit) Error code: %i (%s)", z_res, zError(z_res));
            *dst_size = 0;
            return;
        }

        c_stream.next_out = (Bytef*)dst;
        c_stream.avail_out = *dst_size;
        c_stream.next_in = (Bytef*)src;
        c_stream.avail_in = (uInt)src_size;

        z_res = deflate(&c_stream, Z_NO_FLUSH);
        if (z_res != Z_OK)
        {
            std::cout << "Can't compress update packet (zlib: deflate) Error code: %i (%s)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflate) Error code: %i (%s)", z_res, zError(z_res));
            *dst_size = 0;
            return;
        }

        if (c_stream.avail_in != 0)
        {
            std::cout << "Can't compress update packet (zlib: deflate not greedy)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflate not greedy)");
            *dst_size = 0;
            return;
        }

        z_res = deflate(&c_stream, Z_FINISH);
        if (z_res != Z_STREAM_END)
        {
            std::cout << "Can't compress update packet (zlib: deflate should report Z_STREAM_END instead %i (%s)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflate should report Z_STREAM_END instead %i (%s)", z_res, zError(z_res));
            *dst_size = 0;
            return;
        }

        z_res = deflateEnd(&c_stream);
        if (z_res != Z_OK)
        {
            std::cout << "Can't compress update packet (zlib: deflateEnd) Error code: %i (%s)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflateEnd) Error code: %i (%s)", z_res, zError(z_res));
            *dst_size = 0;
            return;
        }

        *dst_size = c_stream.total_out;
    }
};


#pragma pack(push, 1)
struct sNovusChallenge
{
    u8 command;
    u8 K[32];
};

struct NovusHeader
{
    u8     command;
    u32    account;
    u16    opcode;
    u16    size;

    void CreateForwardHeader(u32 _account, u16 _opcode, u16 _size)
    {
        command = NOVUS_FORWARDPACKET;
        account = _account;
        opcode = _opcode;
        size = _size;
    }

    void Read(Common::ByteBuffer& buffer)
    {
        buffer.Read<u8>(command);
        buffer.Read<u32>(account);
        buffer.Read<u16>(opcode);
        buffer.Read<u16>(size);
    }

    void AddTo(Common::ByteBuffer& buffer)
    {
        buffer.Append((u8*)this, sizeof(NovusHeader));
    }
};

class NovusConnection;
struct NovusMessageHandler
{
    NovusStatus status;
    size_t packetSize;
    bool (NovusConnection::*handler)();
};
#pragma pack(pop)

class WorldServerHandler;
class NovusConnection : Common::BaseSocket
{
public:
    static robin_hood::unordered_map<u8, NovusMessageHandler> InitMessageHandlers();

    NovusConnection(WorldServerHandler* worldServerHandler, asio::ip::tcp::socket* socket, std::string address, u16 port, u8 realmId) : Common::BaseSocket(socket), _status(NOVUSSTATUS_CHALLENGE), _crypto(), _address(address), _port(port), _realmId(realmId), _headerBuffer(), _packetBuffer()
    { 
        _crypto = new StreamCrypto();
        _key = new BigNumber();

        _headerBuffer.Resize(sizeof(NovusHeader));
        _worldServerHandler = worldServerHandler;
    }

    bool Start() override;
    void HandleRead() override;

    bool HandleCommandChallenge();
    bool HandleCommandProof();
    bool HandleCommandForwardPacket();

    void SendPacket(Common::ByteBuffer& packet);

    NovusStatus _status;
private:
    std::string _address;
    u16 _port;
    u8 _realmId;

    StreamCrypto* _crypto;
    BigNumber* _key;

    Common::ByteBuffer _headerBuffer;
    Common::ByteBuffer _packetBuffer;
    WorldServerHandler* _worldServerHandler;
};