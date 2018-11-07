#include "BaseConnection.h"

namespace Common
{
    class TcpServer
    {
    public:
        TcpServer(asio::io_service& io_service) : acceptor_(io_service, tcp::endpoint(tcp::v4(), 3724)) { }

    protected:
        virtual void StartAccept() = 0;
        virtual void HandleAccept(BaseConnection::pointer new_connection, const asio::error_code& error) = 0;

        tcp::acceptor acceptor_;
    };
}
