#include "../common/TcpServer.h"

class Authsocket : Common::TcpServer
{
    Authsocket(asio::io_service& io_service) : Common::TcpServer(io_service)
    {
        StartAccept();
    }

private:
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
            // Add client to queue here
        }

        StartAccept();
    }
};