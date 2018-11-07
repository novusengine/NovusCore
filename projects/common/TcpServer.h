#include "BaseConnection.h"

namespace Common
{
    class TcpServer
    {
    public:
        TcpServer(asio::io_service& io_service)
            : acceptor_(io_service, tcp::endpoint(tcp::v4(), 3724))
        {
            start_accept();
        }

    private:
        void start_accept()
        {
            BaseConnection::pointer new_connection =
                BaseConnection::create(acceptor_.get_io_service());

            acceptor_.async_accept(new_connection->socket(),
                std::bind(&TcpServer::handle_accept, this, new_connection,
                    std::placeholders::_1));
        }

        void handle_accept(BaseConnection::pointer new_connection,
            const asio::error_code& error)
        {
            if (!error)
            {
                new_connection->start();
            }

            start_accept();
        }

        tcp::acceptor acceptor_;
    };
}
