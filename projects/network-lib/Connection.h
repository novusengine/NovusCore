#pragma once
#include <NovusTypes.h>
#include "BaseSocket.h"

class Connection : public BaseSocket
{
public:
    Connection(asio::ip::tcp::socket* socket) : BaseSocket(socket) { }

    void Start() override;
    void HandleRead() override;
};