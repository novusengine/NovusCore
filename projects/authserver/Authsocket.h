#include "../common/TcpServer.h"
#include <thread>

struct SocketSession
{
    Common::BaseConnection::pointer _connections;
};

struct WorkerThread
{
    std::thread _thread;
    std::vector<SocketSession*> _sessions;
    std::mutex _mutex;
};

void WorkerThreadMain(WorkerThread* workerThread)
{
    while (true)
    {
        workerThread->_mutex.lock();

        for (auto& session : workerThread->_sessions)
        {
            // Listen & Read
        }

        workerThread->_mutex.unlock();
    }
}

class Authsocket : Common::TcpServer
{
public:
    Authsocket(asio::io_service& io_service) : Common::TcpServer(io_service)
    {
        StartAccept();
    }

    void Init()
    {
        // Network Thread (TEMPORARY)
        for (int i = 0; i < 2; i++)
        {
            WorkerThread* thread = new WorkerThread();
            thread->_thread = std::thread(WorkerThreadMain, thread);
            _workerThreads.push_back(thread);
        }
    }

private:
    std::vector<WorkerThread*> _workerThreads;

    void StartAccept() override
    {
        Common::BaseConnection::pointer new_connection =
            Common::BaseConnection::create(acceptor_.get_io_service());

        acceptor_.async_accept(new_connection->socket(),
            std::bind(&Authsocket::HandleAccept, this, new_connection,
                std::placeholders::_1));
    }

    void HandleAccept(Common::BaseConnection::pointer new_connection, const asio::error_code& error) override
    {
        if (!error)
        {
            printf("Client Connected\n");

            WorkerThread* workerThread = nullptr;
            int leastActiveSessions = INT32_MAX;
            for (auto& i : _workerThreads)
            {
                i->_mutex.lock();

                if (i->_sessions.size() < leastActiveSessions)
                {
                    leastActiveSessions = i->_sessions.size();
                    workerThread = i;
                }

                i->_mutex.unlock();
            }

            workerThread->_mutex.lock();

            SocketSession* socketSession = new SocketSession();
            socketSession->_connections = new_connection;

            workerThread->_sessions.push_back(socketSession);
            workerThread->_mutex.unlock();
        }

        StartAccept();
    }
};