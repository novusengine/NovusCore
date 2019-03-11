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

#include <Networking\TcpServer.h>
#include "../Connections/NovusConnection.h"

class NovusConnectionHandler : public Common::TcpServer
{
public:
    NovusConnectionHandler(asio::io_service& io_service, int port) : Common::TcpServer(io_service, port) { _instance = this; }
    static NovusConnection* GetCharacterConnection()
    {
        static int loadDistributionId = 0;

        if (_instance->_connections.size() == 0)
            return nullptr;

        /*if (loadDistributionId >= _instance->_connections.size())
            loadDistributionId = int(_instance->_connections.size() - 1);
        
        NovusConnection* connection = reinterpret_cast<NovusConnection*>(_instance->_connections.at(loadDistributionId));
        loadDistributionId = (loadDistributionId + 1) % _instance->_connections.size();*/

        for (auto itr : _instance->_connections)
        {
            NovusConnection* conn = reinterpret_cast<NovusConnection*>(itr);
            if (conn->_type == 0 && !conn->IsClosed())
                return conn;
        }

        return nullptr;
    }

    static NovusConnection* GetWorldConnection(u8 id)
    {
        if (_instance->_connections.size() == 0)
            return nullptr;

        for (auto itr : _instance->_connections)
        {
            NovusConnection* conn = reinterpret_cast<NovusConnection*>(itr);
            if (conn->_type == 1 && conn->_id == id && !conn->IsClosed())
                return conn;
        }

        return nullptr;
    }

private:
    static NovusConnectionHandler* _instance;
    void StartListening() override
    {
        asio::ip::tcp::socket* socket = new asio::ip::tcp::socket(_ioService);
        _acceptor.async_accept(*socket, std::bind(&NovusConnectionHandler::HandleNewConnection, this, socket, std::placeholders::_1));
    }

    void HandleNewConnection(asio::ip::tcp::socket* socket, const asio::error_code& error_code) override
    {
        if (!error_code)
        {
            _workerThread->_mutex.lock();

            socket->non_blocking(true);
            NovusConnection* connection = new NovusConnection(socket);
            connection->Start();

            _connections.push_back(connection);
            _workerThread->_mutex.unlock();
        }

        StartListening();
    }
};