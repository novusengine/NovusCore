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
#pragma once

#include <vector>

namespace Common
{
    class ByteBuffer
    {
    public:
        ByteBuffer() : _readPos(0), _writePos(0), _bufferData()
        {
            _bufferData.reserve(4096);
        }
        ByteBuffer(size_t reserveSize)
        {
            _bufferData.reserve(reserveSize);
        }
        virtual ~ByteBuffer() { }

        void Write(const std::string& value)
        {
            if (size_t len = value.length())
                Append((uint8_t const*)value.c_str(), len);
            Write(0);
        }

        void Write(uint8_t value)
        {
            const uint8_t* src = (uint8_t*)&value;
            size_t size = sizeof(value);

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

            std::memcpy(&_bufferData[_writePos], src, size);
            _writePos = newSize;
        }
        void Append(ByteBuffer const& buffer)
        {
            if (buffer._writePos)
                Append(buffer.data(), buffer._writePos);
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
            _readPos = size;
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
                _bufferData.resize(_bufferData.size() * 1.5f);
            }
        }

        uint8_t* GetDataPointer() { return _bufferData.data(); }
        uint8_t* GetReadPointer() { return _bufferData.data() + _readPos; }
        uint8_t* GetWritePointer() { return _bufferData.data() + _writePos; }
        size_t GetActualSize() { return _writePos - _readPos; }
        size_t GetSpaceLeft() { return _bufferData.size() - _writePos; }
        size_t size() const { return _bufferData.size(); }
        bool empty() const { return _bufferData.empty(); }

        size_t _readPos, _writePos;
    private:
        std::vector<uint8_t> _bufferData;
    };
}