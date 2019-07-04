#pragma once
#include <NovusTypes.h>
#include <Networking/ByteBuffer.h>
#include <Networking/Opcode/Opcode.h>
#include <zlib.h>

class UpdateData
{
public:
    UpdateData() : _blockCount(0)
    {
        _data = ByteBuffer::Borrow<8192>();
    }

    void ResetBlocks()
    {
        _data->Reset();
        _blockCount = 0;
    }
    void ResetInvalidGuids() { _invalidGuids.clear(); }
    bool IsEmpty() { return _data->IsEmpty() && _invalidGuids.empty(); }

    void AddBlock(ByteBuffer* block)
    {
        _data->PutBytes(block->GetInternalData(), block->WrittenData);
        _blockCount++;
    }
    void AddInvalidGuid(u64 guid)
    {
        _invalidGuids.push_back(guid);
    }

    bool Build(ByteBuffer* packet, u16& opcode)
    {
        if (_blockCount == 0 && _invalidGuids.size() == 0)
            return false;

        std::shared_ptr<ByteBuffer> buffer = ByteBuffer::Borrow<8192>();
        buffer->PutU32(_invalidGuids.empty() ? _blockCount : _blockCount + 1);

        if (!_invalidGuids.empty())
        {
            buffer->PutU8(UPDATETYPE_OUT_OF_RANGE_OBJECTS);
            buffer->PutU32(static_cast<u32>(_invalidGuids.size()));

            for (u64 guid : _invalidGuids)
            {
                buffer->PutGuid(guid);
            }
        }

        buffer->PutBytes(_data->GetInternalData(), _data->WrittenData);
        size_t pSize = buffer->WrittenData;

        if (pSize > 1024) // Update packets over 1kb gets compressed
        {
            u32 destsize = compressBound(static_cast<uLong>(pSize));

            packet->Put<u32>(static_cast<u32>(pSize), 0);
            Compress(const_cast<u8*>(packet->GetInternalData()) + sizeof(u32), &destsize, (void*)buffer->GetInternalData(), static_cast<i32>(pSize));
            if (destsize == 0)
                return false;

            packet->WrittenData = destsize + sizeof(u32);
            opcode = Opcode::SMSG_COMPRESSED_UPDATE_OBJECT;
        }
        else
        {
            packet->PutBytes(buffer->GetInternalData(), buffer->WrittenData);
            opcode = Opcode::SMSG_UPDATE_OBJECT;
        }

        return true;
    }

private:
    u32 _blockCount;
    std::vector<u64> _invalidGuids;
    std::shared_ptr<ByteBuffer> _data;

    void Compress(void* dst, u32* dst_size, void* src, i32 src_size)
    {
        z_stream c_stream;

        c_stream.zalloc = static_cast<alloc_func>(nullptr);
        c_stream.zfree = static_cast<free_func>(nullptr);
        c_stream.opaque = static_cast<voidpf>(nullptr);

        // default Z_BEST_SPEED (1)
        i32 z_res = deflateInit(&c_stream, 1); // 1 - 9 (9 Being best)
        if (z_res != Z_OK)
        {
            std::cout << "Can't compress update packet (zlib: deflateInit) Error code: %i (%s)" << std::endl;
            //TC_LOG_ERROR("misc", "Can't compress update packet (zlib: deflateInit) Error code: %i (%s)", z_res, zError(z_res));
            *dst_size = 0;
            return;
        }

        c_stream.next_out = static_cast<Bytef*>(dst);
        c_stream.avail_out = *dst_size;
        c_stream.next_in = static_cast<Bytef*>(src);
        c_stream.avail_in = static_cast<uInt>(src_size);

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
