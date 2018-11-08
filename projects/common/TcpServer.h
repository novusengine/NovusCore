#include <asio.hpp>

namespace Common
{
    class TcpServer
    {
    public:
        TcpServer(asio::io_service& io_service, int port) : _acceptor(io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) { }

    protected:
        virtual void StartListening() = 0;
        virtual void HandleNewConnection(asio::ip::tcp::socket* socket, const asio::error_code& error) = 0;

        asio::ip::tcp::acceptor _acceptor;
    };
}
