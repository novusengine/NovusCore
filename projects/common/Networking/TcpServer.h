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
#include <Utils/Timer.h>
#include "BaseSocket.h"

namespace Common
{
struct WorkerThread
{
    std::thread _thread;
    std::mutex _mutex;
    std::vector<BaseSocket*>* _connections;
    bool _running;
};

static void WorkerThreadMain(WorkerThread* thread)
{
    Timer timer;
    f32 targetDelta = 1.0f / 0.2f;
    while (thread->_running)
    {
        f32 deltaTime = timer.GetDeltaTime();
        timer.Tick();

        // Remove closed sessions
        thread->_mutex.lock();
        if (thread->_connections->size())
        {
            thread->_connections->erase(
                std::remove_if(thread->_connections->begin(), thread->_connections->end(), [](BaseSocket* connection) {
                    if (!connection)
                        return false;

                    return connection->IsClosed();
                }),
                thread->_connections->end());
        }
        thread->_mutex.unlock();

        for (deltaTime = timer.GetDeltaTime(); deltaTime < targetDelta; deltaTime = timer.GetDeltaTime())
        {
            if (!thread->_running)
                break;

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

class TcpServer
{
public:
    TcpServer(asio::io_service& io_service, i32 port) : _acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), _ioService(io_service)
    {
        _connections.resize(4096);

        _workerThread = new WorkerThread();
        _workerThread->_connections = &_connections;
        _workerThread->_running = true;
        _workerThread->_thread = std::thread(WorkerThreadMain, _workerThread);
        _workerThread->_thread.detach();
    }

    void Start()
    {
        StartListening();
    }
    void Stop()
    {
        _workerThread->_running = false;
        _acceptor.close();
    }
    u16 GetPort()
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
} // namespace Common