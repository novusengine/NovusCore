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

#include <vector>

namespace Common
{
    class ByteBuffer
    {
    public:
        ByteBuffer() : _readPos(0), _writePos(0)
        {
            _bufferData.reserve(4096);
        }
        ByteBuffer(size_t reserveSize) : _readPos(0), _writePos(0)
        {
            _bufferData.reserve(reserveSize);
        }
        virtual ~ByteBuffer() { }

        ByteBuffer &operator<<(uint8_t value)
        {
            Write<uint8_t>(value);
            return *this;
        }

        ByteBuffer &operator<<(uint16_t value)
        {
            Write<uint16_t>(value);
            return *this;
        }

        ByteBuffer &operator<<(uint32_t value)
        {
            Write<uint32_t>(value);
            return *this;
        }

        ByteBuffer &operator<<(uint64_t value)
        {
            Write<uint64_t>(value);
            return *this;
        }

        // signed as in 2e complement
        ByteBuffer &operator<<(int8_t value)
        {
            Write<int8_t>(value);
            return *this;
        }

        ByteBuffer &operator<<(int16_t value)
        {
            Write<int16_t>(value);
            return *this;
        }

        ByteBuffer &operator<<(int32_t value)
        {
            Write<int32_t>(value);
            return *this;
        }

        ByteBuffer &operator<<(int64_t value)
        {
            Write<int64_t>(value);
            return *this;
        }

        // floating points
        ByteBuffer &operator<<(float value)
        {
            Write<float>(value);
            return *this;
        }

        ByteBuffer &operator<<(double value)
        {
            Write<double>(value);
            return *this;
        }

        void ReadPackedGUID(uint64_t& guid)
        {
            guid = 0;

            uint8_t guidmark = 0;
            Read(&guidmark, 1);

            for (int i = 0; i < 8; ++i)
            {
                if (guidmark & (uint8_t(1) << i))
                {
                    uint8_t bit;
                    Read(&bit, 1);
                    guid |= (uint64_t(bit) << (i * 8));
                }
            }
        }

        void Read(void* destination, size_t length)
        {
            memcpy(destination, &_bufferData[_readPos], length);
            _readPos += length;
        }
        void Read(uint8_t* destination, size_t length)
        {
            memcpy(destination, &_bufferData[_readPos], length);
            _readPos += length;
        }
        void Read(uint16_t* destination, size_t length)
        {
            memcpy(destination, &_bufferData[_readPos], length);
            _readPos += length;
        }
        void Read(uint32_t* destination, size_t length)
        {
            memcpy(destination, &_bufferData[_readPos], length);
            _readPos += length;
        }
        void Read(uint64_t* destination, size_t length)
        {
            memcpy(destination, &_bufferData[_readPos], length);
            _readPos += length;
        }

        char Read(size_t position)
        {
            char val = *((char const*)&_bufferData[position]);
            return val;
        }

        char Read()
        {
            char r = Read(_readPos);
            _readPos += sizeof(char);
            return r;
        }

        void Read(std::string& value)
        {
            value.clear();
            while (_readPos < size())                         // prevent crash at wrong string format in packet
            {
                char c = Read();
                if (c == 0)
                    break;
                value += c;
            }
        }

        void Write(void const* data, std::size_t size)
        {
            if (size)
            {
                memcpy(GetWritePointer(), data, size);
                WriteBytes(size);
            }
        }

        void WriteString(const std::string& value)
        {
            if (size_t len = value.length())
                Append((uint8_t const*)value.c_str(), len);
            Write<uint8_t>(0);
        }

        template <typename T>
        void Write(T const value)
        {
            Append((uint8_t*)&value, sizeof(value));
        }
        template <typename T>
        void Replace(size_t position, T Value)
        {
            _replace(position, (uint8_t*)&Value, sizeof(Value));
        }
        void _replace(size_t position, uint8_t const* src, size_t content)
        {
            std::memcpy(&_bufferData[position], src, content);
        }


        void Append(ByteBuffer const& buffer)
        {
            if (buffer._writePos)
                Append(buffer.data(), buffer._writePos);
        }

        void AppendGuid(uint64_t guid)
        {
            uint8_t packedGuid[8 + 1];
            packedGuid[0] = 0;
            size_t size = 1;

            for (uint8_t i = 0; guid != 0; ++i)
            {
                if (guid & 0xFF)
                {
                    packedGuid[0] |= uint8_t(1 << i);
                    packedGuid[size] = uint8_t(guid & 0xFF);
                    ++size;
                }

                guid >>= 8;
            }

            Append(packedGuid, size);
        }

        void Append(uint8_t const* value, size_t size)
        {
            size_t const newSize = _writePos + size;
            if (_bufferData.capacity() < newSize)
            {
                if (newSize < 100)
                    _bufferData.reserve(300);
                else if (newSize < 750)
                    _bufferData.reserve(2500);
                else if (newSize < 6000)
                    _bufferData.reserve(10000);
                else
                    _bufferData.reserve(400000);
            }

            if (_bufferData.size() < newSize)
                _bufferData.resize(newSize);

            std::memcpy(&_bufferData[_writePos], value, size);
            _writePos = newSize;
        }
        void ResetPos()
        {
            _readPos = 0;
            _writePos = 0;
        }

        void Clean()
        {
            _readPos = 0;
            _writePos = 0;
            _bufferData.clear();
        }
        void Resize(size_t newSize)
        {
            _bufferData.resize(newSize);
        }
        uint8_t* data()
        {
            return _bufferData.data();
        }
        uint8_t const* data() const
        {
            return _bufferData.data();
        }

        void WriteBytes(size_t size)
        {
            _writePos += size;
        }

        void ReadBytes(size_t size)
        {
            _readPos += size;
        }

        void CleanBuffer()
        {
            if (_readPos)
            {
                if (_readPos != _writePos)
                    memmove(GetDataPointer(), GetReadPointer(), GetActualSize());
                _writePos -= _readPos;
                _readPos = 0;
            }
        }

        void RecalculateSize()
        {
            if (GetSpaceLeft() == 0)
            {
                _bufferData.resize((uint64_t)(_bufferData.size() * 1.5f));
            }
        }

        uint8_t* GetDataPointer() { return _bufferData.data(); }
        uint8_t* GetReadPointer() { return _bufferData.data() + _readPos; }
        uint8_t* GetWritePointer() { return _bufferData.data() + _writePos; }
        uint32_t GetActualSize() { return (uint32_t)(_writePos - _readPos); }
        uint32_t GetSpaceLeft() { return (uint32_t)(_bufferData.size() - _writePos); }
        uint32_t size() const { return (uint32_t)(_bufferData.size()); }
        bool empty() const { return _bufferData.empty(); }

        size_t _readPos, _writePos;
    private:
        std::vector<uint8_t> _bufferData;
    };
}