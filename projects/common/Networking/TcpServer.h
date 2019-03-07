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

#include <asio.hpp>
#include "BaseSocket.h"

namespace Common
{
    struct WorkerThread
    {
        std::thread _thread;
        std::mutex _mutex;
        std::vector<Common::BaseSocket*>* _connections;
    };

    static void WorkerThreadMain(WorkerThread* thread)
    {
        while (true)
        {
            thread->_mutex.lock();
            // Remove closed sessions
            if (thread->_connections->size() > 0)
            {
                thread->_connections->erase(std::remove_if(thread->_connections->begin(), thread->_connections->end(), [](Common::BaseSocket* connection)
                {
                    return connection->IsClosed();
                }), thread->_connections->end());
            }

            thread->_mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        }
    }

    class TcpServer
    {
    public:
        TcpServer(asio::io_service& io_service, int port) : _acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), _ioService(io_service), _workerThread(new WorkerThread())
        {
            _workerThread->_connections = &_connections;
            _workerThread->_thread = std::thread(WorkerThreadMain, _workerThread);
        }

        void Start()
        {
            StartListening();
        }
        uint16_t GetPort()
        {
            return _acceptor.local_endpoint().port();
        }
    protected:
        virtual void StartListening() = 0;
        virtual void HandleNewConnection(asio::ip::tcp::socket* socket, const asio::error_code& error) = 0;

        std::vector<BaseSocket*> _connections;
        WorkerThread* _workerThread;
        asio::ip::tcp::acceptor _acceptor;
        asio::io_service& _ioService;
    };
}