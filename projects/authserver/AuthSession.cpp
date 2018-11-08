#include "AuthSession.h"

void AuthSession::Start()
{
    AsyncRead();
}

void AuthSession::HandleRead()
{
    // Do shizzle nizzle
    Common::PacketBuffer& packetBuffer = GetPacketBuffer();
    printf("HandleRead(): %u Bytes\n", packetBuffer.GetPacketSize());
    _socket->close();

    // Call AsyncRead to loop
    AsyncRead();
}
void AuthSession::HandleWrite()
{

}

/*
void AsyncRead()
{
    if (!IsOpen())
    return;

    _readBuffer.Normalize();
    _readBuffer.EnsureFreeSpace();
    _socket.async_read_some(boost::asio::buffer(_readBuffer.GetWritePointer(), _readBuffer.GetRemainingSpace()),
    std::bind(&Socket<T>::ReadHandlerInternal, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}
*/