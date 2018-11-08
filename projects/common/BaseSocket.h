#include <ctime>
#include <iostream>
#include <string>
#include <functional>
#include <asio.hpp>
#include <asio\placeholders.hpp>

namespace Common
{
    class PacketBuffer
    {
    public:
        PacketBuffer() : _readPos(0), _writePos(0), _bufferData()
        {
            _bufferData.resize(4096);
        }

        PacketBuffer(size_t initialSize)
        {
            _bufferData.resize(initialSize);
        }

        void Clean()
        {
            _readPos = 0;
            _writePos = 0;
        }
        void Resize(size_t newSize)
        {
            _bufferData.resize(newSize);
        }

        uint8_t* GetDataPointer() { return _bufferData.data(); }
        uint8_t* GetReadPointer() { return _bufferData.data() + _readPos; }
        uint8_t* GetWritePointer() { return _bufferData.data() + _writePos; }

        size_t GetDataSize() { return _bufferData.size(); }
        size_t GetBufferSpace() { return _bufferData.size() - _writePos; }
        size_t GetPacketSize() { return _writePos - _readPos; }

        void Read(size_t readBytes)
        {
            _readPos += readBytes;
        }
        void Write(size_t writtenBytes)
        {
            _writePos += writtenBytes;
        }

    private:
        size_t _readPos, _writePos;
        std::vector<uint8_t> _bufferData;
    };

    class BaseSocket : public std::enable_shared_from_this<BaseSocket>
    {
    public:
        virtual void Start() = 0;
        virtual void HandleRead() = 0;
        virtual void HandleWrite() = 0;

        asio::ip::tcp::socket* socket()
        {
            return _socket;
        }

    protected:
        BaseSocket(asio::ip::tcp::socket* socket) : _socket(socket), _packetBuffer() { }
       
        void AsyncRead()
        {
            // Ensure validity of connectiion bound to the socket
            if (!_socket->is_open())
                return;

            // Clean Buffer after each read?

            // Call Async Read (some) for HandleInternalRead
            _socket->async_read_some(asio::buffer(_packetBuffer.GetWritePointer(), _packetBuffer.GetBufferSpace()),
                std::bind(&Common::BaseSocket::HandleInternalRead, this, std::placeholders::_1, std::placeholders::_2));
        }
        void HandleInternalRead(asio::error_code errorCode, size_t bytes)
        {
            if (errorCode)
            {
                printf("HandleInternalRead: Error %s\n", errorCode.message().c_str());
                _socket->close();
                return;
            }

            printf("HandleInternalRead: %u Bytes\n", bytes);
            _packetBuffer.Write(bytes);
            HandleRead();
        }

        PacketBuffer& GetPacketBuffer() { return _packetBuffer; }
        PacketBuffer _packetBuffer;
        asio::ip::tcp::socket* _socket;
    };
}