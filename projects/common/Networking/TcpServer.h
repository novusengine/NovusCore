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

#include <asio.hpp>

namespace Common
{
    template <class T>
    struct WorkerThread
    {
        std::thread _thread;
        std::mutex _mutex;
        std::vector<T*> _sessions;
    };

    template <class T, class S>
    void WorkerThreadMain(T* thread)
    {
        while (true)
        {
            thread->_mutex.lock();
            // Remove closed sessions
            if (thread->_sessions.size() > 0)
            {
                thread->_sessions.erase(std::remove_if(thread->_sessions.begin(), thread->_sessions.end(), [](S* session)
                {
                    return session->IsClosed();
                }), thread->_sessions.end());
            }

            thread->_mutex.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(5000));
        }
    }

    template <class T>
    class TcpServer
    {
    public:
        TcpServer(asio::io_service& io_service, int port) : _acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), _port(port) { }

    protected:
        virtual void StartListening() = 0;
        virtual void HandleNewConnection(asio::ip::tcp::socket* socket, const asio::error_code& error) = 0;

        Common::WorkerThread<T>* _workerThread;
        asio::ip::tcp::acceptor _acceptor;
        uint16_t _port;
    };
}