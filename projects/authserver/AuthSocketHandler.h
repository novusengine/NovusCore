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

#include <Networking/TcpServer.h>
#include "Session\AuthSession.h"

class AuthSocketHandler : Common::TcpServer<AuthSession>
{
public:
    AuthSocketHandler(asio::io_service& io_service, int port) : Common::TcpServer<AuthSession>(io_service, port), _ioService(io_service) { }

    void Start()
    {
        _workerThread = new Common::WorkerThread<AuthSession>();
        _workerThread->_thread = std::thread(Common::WorkerThreadMain<Common::WorkerThread<AuthSession>, AuthSession>, _workerThread);

        StartListening();
    }

    uint16_t GetPort()
    {
        return _port;
    }
private:
    void StartListening() override
    {
        asio::ip::tcp::socket* socket = new asio::ip::tcp::socket(_ioService);
        _acceptor.async_accept(*socket, std::bind(&AuthSocketHandler::HandleNewConnection, this, socket, std::placeholders::_1));
    }

    void HandleNewConnection(asio::ip::tcp::socket* socket, const asio::error_code& error_code) override
    {
        if (!error_code)
        {
            printf("Client Connected\n");
            _workerThread->_mutex.lock();

            socket->non_blocking(true);
            AuthSession* authSession = new AuthSession(socket);
            authSession->Start();

            _workerThread->_sessions.push_back(authSession);
            _workerThread->_mutex.unlock();
        }

        StartListening();
    }

    asio::io_service& _ioService;
};