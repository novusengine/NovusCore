#pragma once
#include "BaseSocket.h"

class Connection
{
public:
    Connection(asio::ip::tcp::socket* socket) 
    {
        _baseSocket = new BaseSocket(socket, std::bind(&Connection::HandleRead, this));
    }

    void Start();
    void HandleRead();

private:
    BaseSocket* _baseSocket;
};