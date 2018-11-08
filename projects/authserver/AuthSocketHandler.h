#include "../common/TcpServer.h"
#include "AuthSession.h"

struct WorkerThread
{
    std::thread _thread;
    std::vector<AuthSession*> _sessions;
    std::mutex _mutex;
};

void WorkerThreadMain(WorkerThread* workerThread)
{
    while (true)
    {
        workerThread->_mutex.lock();

        // Remove closed sessions
        if (workerThread->_sessions.size() > 0)
        {
            printf("Sessions Before: %u\n", workerThread->_sessions.size());
            workerThread->_sessions.erase((std::remove_if(workerThread->_sessions.begin(), workerThread->_sessions.end() - 1, [](AuthSession* session) {
                return !session->socket()->is_open();
            }), workerThread->_sessions.end() - 1));

            printf("Sessions After: %u\n", workerThread->_sessions.size());
        }

        /*for (auto& session : workerThread->_sessions)
        {
        }*/

        workerThread->_mutex.unlock();
    }
}

class AuthSocketHandler : Common::TcpServer
{
public:
    AuthSocketHandler(asio::io_service& io_service, int port) : Common::TcpServer(io_service, port), _ioService(io_service) { }

    void Init()
    {
        // Network Thread (TEMPORARY)
        for (int i = 0; i < 2; i++)
        {
            WorkerThread* workerThread = new WorkerThread();
            workerThread->_thread = std::thread(WorkerThreadMain, workerThread);
            _workerThreads.push_back(workerThread);
        }
    }
    void Start()
    {
        StartListening();
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

            WorkerThread* workerThread = nullptr;
            size_t leastActiveSessions = INT32_MAX;
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


            socket->non_blocking(true);
            AuthSession* authSession = new AuthSession(socket);
            authSession->Start();
            workerThread->_sessions.push_back(authSession);
            workerThread->_mutex.unlock();
        }

        StartListening();
    }

    asio::io_service& _ioService;
    std::vector<WorkerThread*> _workerThreads;
};