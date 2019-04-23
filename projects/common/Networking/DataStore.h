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
#include "../NovusTypes.h"
#include <cassert>

class DataStore
{
public:
    DataStore(u8* inData = nullptr, size_t inSize = 128)
    {
        assert(inSize > 0);

        if (inData == nullptr)
        {
            _data = new u8[inSize];
            _isOwner = true;
        }
        else
        {
            _data = inData;
        }

        Size = inSize;
    }
    ~DataStore() { if (_isOwner) { delete[] _data; _data = nullptr; } }

    bool CanPerformRead(size_t inSize)
    {
        return ReadData + inSize <= Size;
    }
    bool CanPerformWrite(size_t inSize)
    {
        return WrittenData + inSize <= Size;
    }

    bool GetI8(i8& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i8);
        if (!CanPerformRead(readSize))
            return false;

        val = _data[ReadData];
        ReadData += readSize;
        return true;
    }
    bool GetU8(u8& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u8);
        if (!CanPerformRead(readSize))
            return false;

        val = _data[ReadData];
        ReadData += readSize;
        return true;
    }
    bool GetI16(i16& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i16);
        if (!CanPerformRead(readSize))
            return false;

        val = static_cast<u16>(_data[ReadData]) | (static_cast<u16>(_data[ReadData + 1]) << 8);
        ReadData += readSize;
        return true;
    }
    bool GetU16(u16& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u16);
        if (!CanPerformRead(readSize))
            return false;

        val = static_cast<u16>(_data[ReadData]) | (static_cast<u16>(_data[ReadData + 1]) << 8);
        ReadData += readSize;
        return true;
    }
    bool GetI32(i32& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i32);
        if (!CanPerformRead(readSize))
            return false;

        val = static_cast<u32>(_data[ReadData]) | (static_cast<u32>(_data[ReadData + 1]) << 8) | (static_cast<u32>(_data[ReadData + 2]) << 16) | (static_cast<u32>(_data[ReadData + 3]) << 24);
        ReadData += readSize;
        return true;
    }
    bool GetU32(u32& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u32);
        if (!CanPerformRead(readSize))
            return false;

        val = static_cast<u32>(_data[ReadData]) | (static_cast<u32>(_data[ReadData + 1]) << 8) | (static_cast<u32>(_data[ReadData + 2]) << 16) | (static_cast<u32>(_data[ReadData + 3]) << 24);
        ReadData += readSize;
        return true;
    }
    bool GetF32(f32& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(f32);
        if (!CanPerformRead(readSize))
            return false;

        val = *reinterpret_cast<f32*>(_data[ReadData]);
        ReadData += readSize;
        return true;
    }
    bool GetI64(i64& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i64);
        if (!CanPerformRead(readSize))
            return false;

        val = static_cast<u64>(_data[ReadData]) | (static_cast<u64>(_data[ReadData + 1]) << 8) | (static_cast<u64>(_data[ReadData + 2]) << 16) | (static_cast<u64>(_data[ReadData + 3]) << 24) | (static_cast<u64>(_data[ReadData + 4]) << 32) | (static_cast<u64>(_data[ReadData + 5]) << 40) | (static_cast<u64>(_data[ReadData + 6]) << 48) | (static_cast<u64>(_data[ReadData] + 7) << 56);
        ReadData += readSize;
        return true;
    }
    bool GetU64(u64& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u64);
        if (!CanPerformRead(readSize))
            return false;

        val = static_cast<u64>(_data[ReadData]) | (static_cast<u64>(_data[ReadData + 1]) << 8) | (static_cast<u64>(_data[ReadData + 2]) << 16) | (static_cast<u64>(_data[ReadData + 3]) << 24) | (static_cast<u64>(_data[ReadData + 4]) << 32) | (static_cast<u64>(_data[ReadData + 5]) << 40) | (static_cast<u64>(_data[ReadData + 6]) << 48) | (static_cast<u64>(_data[ReadData] + 7) << 56);
        ReadData += readSize;
        return true;
    }
    bool GetF64(f64& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(f64);
        if (!CanPerformRead(readSize))
            return false;

        val = *reinterpret_cast<f64*>(_data[ReadData]);
        ReadData += readSize;
        return true;
    }

    bool PutI8(i8 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(i8);
        if (!CanPerformWrite(writeSize))
            return false;

        _data[WrittenData] = val;
        WrittenData += writeSize;
        return true;
    }
    bool PutU8(u8 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u8);
        if (!CanPerformWrite(writeSize))
            return false;

        _data[WrittenData] = val;
        WrittenData += writeSize;
        return true;
    }
    bool PutBytes(u8* val, size_t Size)
    {
        assert(_data != nullptr);

        if (!CanPerformWrite(Size))
            return false;

        std::memcpy(&_data[WrittenData], val, Size);
        WrittenData += Size;
        return true;
    }
    bool PutI16(i16 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(i16);
        if (!CanPerformWrite(writeSize))
            return false;

        _data[WrittenData] = static_cast<u8>(val);
        _data[WrittenData + 1] = (static_cast<u8>(val >> 8));
        WrittenData += writeSize;
        return true;
    }
    bool PutU16(u16 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u16);
        if (!CanPerformWrite(writeSize))
            return false;

        _data[WrittenData] = static_cast<u8>(val);
        _data[WrittenData + 1] = (static_cast<u8>(val >> 8));
        WrittenData += writeSize;
        return true;
    }
    bool PutI32(i32 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(i32);
        if (!CanPerformWrite(writeSize))
            return false;

        _data[WrittenData] = static_cast<u8>(val);
        _data[WrittenData + 1] = (static_cast<u8>(val >> 8));
        _data[WrittenData + 2] = (static_cast<u8>(val >> 16));
        _data[WrittenData + 3] = (static_cast<u8>(val >> 24));
        WrittenData += writeSize;
        return true;
    }
    bool PutU32(u32 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u32);
        if (!CanPerformWrite(writeSize))
            return false;

        _data[WrittenData] = static_cast<u8>(val);
        _data[WrittenData + 1] = (static_cast<u8>(val >> 8));
        _data[WrittenData + 2] = (static_cast<u8>(val >> 16));
        _data[WrittenData + 3] = (static_cast<u8>(val >> 24));
        WrittenData += writeSize;
        return true;
    }
    bool PutF32(f32 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(f32);
        if (!CanPerformWrite(writeSize))
            return false;

        std::memcpy(&_data[WrittenData], reinterpret_cast<const u8*>(&val), writeSize);
        WrittenData += writeSize;
        return true;
    }
    bool PutI64(i64 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(i64);
        if (!CanPerformWrite(writeSize))
            return false;

        _data[WrittenData] = static_cast<u8>(val);
        _data[WrittenData + 1] = (static_cast<u8>(val >> 8));
        _data[WrittenData + 2] = (static_cast<u8>(val >> 16));
        _data[WrittenData + 3] = (static_cast<u8>(val >> 24));
        _data[WrittenData + 4] = (static_cast<u8>(val >> 32));
        _data[WrittenData + 5] = (static_cast<u8>(val >> 40));
        _data[WrittenData + 6] = (static_cast<u8>(val >> 48));
        _data[WrittenData + 7] = (static_cast<u8>(val >> 56));
        WrittenData += writeSize;
        return true;
    }
    bool PutU64(u64 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u64);
        if (!CanPerformWrite(writeSize))
            return false;

        _data[WrittenData] = static_cast<u8>(val);
        _data[WrittenData + 1] = (static_cast<u8>(val >> 8));
        _data[WrittenData + 2] = (static_cast<u8>(val >> 16));
        _data[WrittenData + 3] = (static_cast<u8>(val >> 24));
        _data[WrittenData + 4] = (static_cast<u8>(val >> 32));
        _data[WrittenData + 5] = (static_cast<u8>(val >> 40));
        _data[WrittenData + 6] = (static_cast<u8>(val >> 48));
        _data[WrittenData + 7] = (static_cast<u8>(val >> 56));
        WrittenData += writeSize;
        return true;
    }
    bool PutF64(f64 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(f64);
        if (!CanPerformWrite(writeSize))
            return false;

        std::memcpy(&_data[WrittenData], reinterpret_cast<const u8*>(&val), writeSize);
        WrittenData += writeSize;
        return true;
    }
    size_t PutString(const std::string_view val)
    {
        assert(_data != nullptr);

        size_t writeSize = val.length();
        size_t writeSizeTotal = writeSize + 1;
        if (!CanPerformWrite(writeSizeTotal))
            return 0;

        std::memcpy(&_data[WrittenData], val.data(), writeSize);
        WrittenData += writeSize;
        _data[WrittenData++] = 0;
        return writeSizeTotal;
    }

    bool IsEmpty() { return WrittenData == 0; }

    size_t WrittenData = 0;
    size_t ReadData = 0;
    size_t Size = 0;

    u8* GetInternalData() { return _data; }
private:
    u8* _data;
    bool _isOwner = false;
};
