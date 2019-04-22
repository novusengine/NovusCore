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

        size = inSize;
    }
    ~DataStore() { if (_isOwner) { delete[] _data; } }

    bool CanPerformReadOrWrite(size_t inSize)
    {
        return read + inSize <= size;
    }

    bool GetI8(i8& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i8);
        if (!CanPerformReadOrWrite(readSize))
            return false;

        val = _data[read];
        read += readSize;
        return true;
    }
    bool GetU8(u8& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u8);
        if (!CanPerformReadOrWrite(readSize))
            return false;

        val = _data[read];
        read += readSize;
        return true;
    }
    bool GetI16(i16& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i16);
        if (!CanPerformReadOrWrite(readSize))
            return false;

        val = (static_cast<u16>(_data[read]) << 8) | static_cast<u16>(_data[read + 1]);
        read += readSize;
        return true;
    }
    bool GetU16(u16& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u16);
        if (!CanPerformReadOrWrite(readSize))
            return false;

        val = (static_cast<u16>(_data[read]) << 8) | static_cast<u16>(_data[read + 1]);
        read += readSize;
        return true;
    }
    bool GetI32(i32& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i32);
        if (!CanPerformReadOrWrite(readSize))
            return false;

        val = (static_cast<u32>(_data[read]) << 24) | (static_cast<u32>(_data[read + 1]) << 16) | (static_cast<u32>(_data[read + 2]) << 8) | static_cast<u32>(_data[read + 3]);
        read += readSize;
        return true;
    }
    bool GetU32(u32& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u32);
        if (!CanPerformReadOrWrite(readSize))
            return false;

        val = (static_cast<u32>(_data[read]) << 24) | (static_cast<u32>(_data[read + 1]) << 16) | (static_cast<u32>(_data[read + 2]) << 8) | static_cast<u32>(_data[read + 3]);
        read += readSize;
        return true;
    }    
    bool GetF32(f32& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(f32);
        if (!CanPerformReadOrWrite(readSize))
            return false;

        val = *reinterpret_cast<f32*>(_data[read]);
        read += readSize;
        return true;
    }
    bool GetI64(i64& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i64);
        if (!CanPerformReadOrWrite(readSize))
            return false;

        val = (static_cast<u64>(_data[read]) << 56) | (static_cast<u64>(_data[read + 1]) << 48) | (static_cast<u64>(_data[read + 2]) << 40) | (static_cast<u64>(_data[read + 3]) << 32) | (static_cast<u64>(_data[read + 4]) << 24) | (static_cast<u64>(_data[read + 5]) << 16) | (static_cast<u64>(_data[read + 6]) << 8) | static_cast<u64>(_data[read + 7]);
        read += readSize;
        return true;
    }
    bool GetU64(u64& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u64);
        if (!CanPerformReadOrWrite(readSize))
            return false;

        val = (static_cast<u64>(_data[read]) << 56) | (static_cast<u64>(_data[read + 1]) << 48) | (static_cast<u64>(_data[read + 2]) << 40) | (static_cast<u64>(_data[read + 3]) << 32) | (static_cast<u64>(_data[read + 4]) << 24) | (static_cast<u64>(_data[read + 5]) << 16) | (static_cast<u64>(_data[read + 6]) << 8) | static_cast<u64>(_data[read + 7]);
        read += readSize;
        return true;
    }
    bool GetF64(f64& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(f64);
        if (!CanPerformReadOrWrite(readSize))
            return false;

        val = *reinterpret_cast<f64*>(_data[read]);
        read += readSize;
        return true;
    }

    bool PutI8(i8 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(i8);
        if (!CanPerformReadOrWrite(writeSize))
            return false;

        _data[write] = val;
        write += writeSize;
        return true;
    }
    bool PutU8(u8 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u8);
        if (!CanPerformReadOrWrite(writeSize))
            return false;

        _data[write] = val;
        write += writeSize;
        return true;
    }
    bool PutBytes(u8* val, size_t size)
    {
        assert(_data != nullptr);

        if (!CanPerformReadOrWrite(size))
            return false;

        std::memcpy(&_data[write], val, size);
        write += size;
        return true;
    }
    bool PutI16(i16 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(i16);
        if (!CanPerformReadOrWrite(writeSize))
            return false;

        _data[write] = (static_cast<u8>(val >> 8));
        _data[read + 1] = static_cast<u8>(val);
        write += writeSize;
        return true;
    }
    bool PutU16(u16 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u16);
        if (!CanPerformReadOrWrite(writeSize))
            return false;

        _data[write] = (static_cast<u8>(val >> 8));
        _data[read + 1] = static_cast<u8>(val);
        write += writeSize;
        return true;
    }
    bool PutI32(i32 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(i32);
        if (!CanPerformReadOrWrite(writeSize))
            return false;

        _data[write] = (static_cast<u8>(val >> 24));
        _data[read + 1] = (static_cast<u8>(val >> 16));
        _data[read + 2] = (static_cast<u8>(val >> 8));
        _data[read + 3] = static_cast<u8>(val);
        write += writeSize;
        return true;
    }
    bool PutU32(u32 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u32);
        if (!CanPerformReadOrWrite(writeSize))
            return false;

        _data[write] = (static_cast<u8>(val >> 24));
        _data[read + 1] = (static_cast<u8>(val >> 16));
        _data[read + 2] = (static_cast<u8>(val >> 8));
        _data[read + 3] = static_cast<u8>(val);
        write += writeSize;
        return true;
    }
    bool PutI64(i64 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(i64);
        if (!CanPerformReadOrWrite(writeSize))
            return false;

        _data[write] = static_cast<u8>(val);
        _data[read + 1] = (static_cast<u8>(val >> 8));
        _data[read + 2] = (static_cast<u8>(val >> 16));
        _data[read + 3] = (static_cast<u8>(val >> 24));
        _data[read + 4] = (static_cast<u8>(val >> 32));
        _data[read + 5] = (static_cast<u8>(val >> 40));
        _data[read + 6] = (static_cast<u8>(val >> 48));
        _data[read + 7] = (static_cast<u8>(val >> 56));
        write += writeSize;
        return true;
    }
    bool PutU64(u64 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u64);
        if (!CanPerformReadOrWrite(writeSize))
            return false;

        _data[write] = static_cast<u8>(val);
        _data[read + 1] = (static_cast<u8>(val >> 8));
        _data[read + 2] = (static_cast<u8>(val >> 16));
        _data[read + 3] = (static_cast<u8>(val >> 24));
        _data[read + 4] = (static_cast<u8>(val >> 32));
        _data[read + 5] = (static_cast<u8>(val >> 40));
        _data[read + 6] = (static_cast<u8>(val >> 48));
        _data[read + 7] = (static_cast<u8>(val >> 56));
        write += writeSize;
        return true;
    }

    bool IsEmpty() { return write == 0; }

    size_t write = 0;
    size_t read = 0;
    size_t size = 0;

    u8* GetInternalData() { return _data; }
private:
    u8* _data;
    bool _isOwner = false;
};
