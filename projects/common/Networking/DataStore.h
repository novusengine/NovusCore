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
#include "../Utils/SharedPool.h"
#include <cassert>

class DataStore
{
public:
    DataStore(u8* inData = nullptr, size_t inSize = 128)
    {
        if (inData == nullptr)
        {
            _data = new u8[inSize];
            IsOwner = true;
        }
        else
        {
            _data = inData;
        }

        Size = inSize;
    }
    ~DataStore()
    {
        if (IsOwner)
        {
            delete[] _data;
            _data = nullptr;
        }
    }

    bool CanPerformRead(size_t inSize)
    {
        return ReadData + inSize <= Size;
    }
    bool CanPerformRead(size_t inSize, size_t offset)
    {
        return offset + inSize <= Size;
    }
    bool CanPerformWrite(size_t inSize)
    {
        return WrittenData + inSize <= Size;
    }
    bool CanPerformWrite(size_t inSize, size_t offset)
    {
        return offset + inSize <= Size;
    }

    template <typename T>
    bool Get(T& val)
    {
        assert(_data != nullptr);

        size_t readSize = sizeof(T);
        if (!CanPerformRead(readSize))
            return false;

        val = *reinterpret_cast<T const*>(&_data[ReadData]);
        ReadData += readSize;
        return true;
    }
    template <typename T>
    bool Get(T& val, size_t offset)
    {
        assert(_data != nullptr);

        size_t readSize = sizeof(T);
        if (!CanPerformRead(readSize, offset))
            return false;

        val = *reinterpret_cast<T const*>(&_data[offset]);
        return true;
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
    bool GetBytes(u8* dest, size_t Size)
    {
        assert(_data != nullptr);

        if (!CanPerformRead(Size))
            return false;

        std::memcpy(dest, &_data[ReadData], Size);
        ReadData += Size;
        return true;
    }
    bool GetI16(i16& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i16);
        if (!CanPerformRead(readSize))
            return false;

        val = *reinterpret_cast<i16*>(&_data[ReadData]);
        ReadData += readSize;
        return true;
    }
    bool GetU16(u16& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u16);
        if (!CanPerformRead(readSize))
            return false;

        val = *reinterpret_cast<u16*>(&_data[ReadData]);
        ReadData += readSize;
        return true;
    }
    bool GetI32(i32& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i32);
        if (!CanPerformRead(readSize))
            return false;

        val = *reinterpret_cast<i32*>(&_data[ReadData]);
        ReadData += readSize;
        return true;
    }
    bool GetU32(u32& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u32);
        if (!CanPerformRead(readSize))
            return false;

        val = *reinterpret_cast<u32*>(&_data[ReadData]);
        ReadData += readSize;
        return true;
    }
    bool GetF32(f32& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(f32);
        if (!CanPerformRead(readSize))
            return false;

        val = *reinterpret_cast<f32*>(&_data[ReadData]);
        ReadData += readSize;
        return true;
    }
    bool GetI64(i64& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(i64);
        if (!CanPerformRead(readSize))
            return false;

        val = *reinterpret_cast<i64*>(&_data[ReadData]);
        ReadData += readSize;
        return true;
    }
    bool GetU64(u64& val)
    {
        assert(_data != nullptr);

        const size_t readSize = sizeof(u64);
        if (!CanPerformRead(readSize))
            return false;

        val = *reinterpret_cast<u64*>(&_data[ReadData]);
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
    void GetString(std::string& val)
    {
        assert(_data != nullptr);
        val.clear();
        while (ReadData < Size)
        {
            char c = _data[ReadData++];
            if (c == 0)
                break;

            val += c;
        }
    }
    bool GetGuid(u64& val)
    {
        val = 0;

        u8 guidmark = 0;
        if (!GetU8(guidmark))
            return false;

        for (i32 i = 0; i < 8; ++i)
        {
            if (guidmark & (static_cast<u8>(1) << i))
            {
                u8 bit;
                if (!GetU8(bit))
                    return false;

                val |= (static_cast<u64>(bit) << (i * 8));
            }
        }

        return val != 0;
    }

    template <typename T>
    bool Put(T val)
    {
        assert(_data != nullptr);

        size_t writeSize = sizeof(T);
        if (!CanPerformWrite(writeSize))
            return false;

        std::memcpy(&_data[WrittenData], &val, writeSize);
        WrittenData += writeSize;
        return true;
    }
    template <typename T>
    bool Put(T val, size_t offset)
    {
        assert(_data != nullptr);

        size_t writeSize = sizeof(T);
        if (!CanPerformWrite(writeSize, offset))
            return false;

        std::memcpy(&_data[offset], &val, writeSize);
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

        std::memcpy(&_data[WrittenData], reinterpret_cast<const u8*>(&val), writeSize);
        WrittenData += writeSize;
        return true;
    }
    bool PutU16(u16 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u16);
        if (!CanPerformWrite(writeSize))
            return false;

        std::memcpy(&_data[WrittenData], reinterpret_cast<const u8*>(&val), writeSize);
        WrittenData += writeSize;
        return true;
    }
    bool PutI32(i32 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(i32);
        if (!CanPerformWrite(writeSize))
            return false;

        std::memcpy(&_data[WrittenData], reinterpret_cast<const u8*>(&val), writeSize);
        WrittenData += writeSize;
        return true;
    }
    bool PutU32(u32 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u32);
        if (!CanPerformWrite(writeSize))
            return false;

        std::memcpy(&_data[WrittenData], reinterpret_cast<const u8*>(&val), writeSize);
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

        std::memcpy(&_data[WrittenData], reinterpret_cast<const u8*>(&val), writeSize);
        WrittenData += writeSize;
        return true;
    }
    bool PutU64(u64 val)
    {
        assert(_data != nullptr);

        const size_t writeSize = sizeof(u64);
        if (!CanPerformWrite(writeSize))
            return false;

        std::memcpy(&_data[WrittenData], reinterpret_cast<const u8*>(&val), writeSize);
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
    bool PutGuid(u64 val)
    {
        u8 packedGuid[8 + 1];
        packedGuid[0] = 0;
        size_t size = 1;

        for (u8 i = 0; val != 0; ++i)
        {
            if (val & 0xFF)
            {
                packedGuid[0] |= static_cast<u8>(1 << i);
                packedGuid[size] = static_cast<u8>(val & 0xFF);
                ++size;
            }

            val >>= 8;
        }

        return PutBytes(packedGuid, size);
    }

    void Reset() { WrittenData = 0; ReadData = 0; }
    bool IsEmpty() { return WrittenData == 0; }
    bool IsFull() { return WrittenData == Size; }
    u32 GetRemainingSpace() { return static_cast<u32>(Size - WrittenData); }
    u32 GetActiveSize() { return static_cast<u32>(WrittenData - ReadData); }

    size_t WrittenData = 0;
    size_t ReadData = 0;
    size_t Size = 0;
    bool IsOwner = false;

    u8* GetInternalData() { return _data; }
    u8* GetReadPointer() { return _data + ReadData; }
    u8* GetWritePointer() { return _data + WrittenData; }

    template <size_t size>
    static std::shared_ptr<DataStore> Borrow()
    {
        static_assert(size <= 8192);

        if constexpr (size <= 128)
        {
            if (_dataStores128.empty())
            {
                DataStore* newDataStore = new DataStore(nullptr, 128);
                _dataStores128.add(std::unique_ptr<DataStore>(newDataStore));
            }

            std::shared_ptr<DataStore> dataStore = _dataStores128.acquire();
            dataStore->Size = size;
            dataStore->Reset();

            return dataStore;
        }
        else if constexpr (size <= 512)
        {
            if (_dataStores512.empty())
            {
                DataStore* newDataStore = new DataStore(nullptr, 512);
                _dataStores512.add(std::unique_ptr<DataStore>(newDataStore));
            }

            std::shared_ptr<DataStore> dataStore = _dataStores512.acquire();
            dataStore->Size = size;
            dataStore->Reset();

            return dataStore;
        }
        else if constexpr (size <= 1024)
        {
            if (_dataStores1024.empty())
            {
                DataStore* newDataStore = new DataStore(nullptr, 1024);
                _dataStores1024.add(std::unique_ptr<DataStore>(newDataStore));
            }

            std::shared_ptr<DataStore> dataStore = _dataStores1024.acquire();
            dataStore->Size = size;
            dataStore->Reset();

            return dataStore;
        }
        else if constexpr (size <= 4096)
        {
            if (_dataStores4096.empty())
            {
                DataStore* newDataStore = new DataStore(nullptr, 4096);
                _dataStores4096.add(std::unique_ptr<DataStore>(newDataStore));
            }

            std::shared_ptr<DataStore> dataStore = _dataStores4096.acquire();
            dataStore->Size = size;
            dataStore->Reset();

            return dataStore;
        }
        else if constexpr (size <= 8192)
        {
            if (_dataStores8192.empty())
            {
                DataStore* newDataStore = new DataStore(nullptr, 8192);
                _dataStores8192.add(std::unique_ptr<DataStore>(newDataStore));
            }

            std::shared_ptr<DataStore> dataStore = _dataStores8192.acquire();
            dataStore->Size = size;
            dataStore->Reset();

            return dataStore;
        }
    }
private:
    u8* _data;

    static SharedPool<DataStore> _dataStores128;
    static SharedPool<DataStore> _dataStores512;
    static SharedPool<DataStore> _dataStores1024;
    static SharedPool<DataStore> _dataStores4096;
    static SharedPool<DataStore> _dataStores8192;
};
