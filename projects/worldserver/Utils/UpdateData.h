#pragma once
#include <NovusTypes.h>
#include <Networking/ByteBuffer.h>
#include <Networking/Opcode/Opcode.h>
#include <zlib.h>

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

        if (false)//(pSize > 10000)
        {
            u32 destsize = compressBound(uLong(pSize));
            packet.Resize(destsize + sizeof(u32));
            packet._writePos = packet.size();

            packet.Replace<u32>(0, u32(pSize));
            Compress(const_cast<u8*>(packet.data()) + sizeof(u32), &destsize, (void*)buffer.data(), i32(pSize));
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

    void Compress(void* dst, u32 *dst_size, void* src, i32 src_size)
    {
        z_stream c_stream;

        c_stream.zalloc = (alloc_func)nullptr;
        c_stream.zfree = (free_func)nullptr;
        c_stream.opaque = (voidpf)nullptr;

        // default Z_BEST_SPEED (1)
        i32 z_res = deflateInit(&c_stream, 1); // 1 - 9 (9 Being best)
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
