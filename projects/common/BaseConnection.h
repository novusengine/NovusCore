#include <ctime>
#include <iostream>
#include <string>
#include <functional>
#include <asio.hpp>
#include <asio\placeholders.hpp>

using asio::ip::tcp;

namespace Common
{
    class BaseConnection
        : public std::enable_shared_from_this<BaseConnection>
    {
    public:
        typedef std::shared_ptr<BaseConnection> pointer;

        static pointer create(asio::io_service& io_service)
        {
            return pointer(new BaseConnection(io_service));
        }

        tcp::socket& socket()
        {
            return socket_;
        }

        void start()
        {
            printf("New Client Connected\n");
        }

    private:
        BaseConnection(asio::io_service& io_service)
            : socket_(io_service)
        {
        }

        void handle_write(const asio::error_code& /*error*/,
            size_t /*bytes_transferred*/)
        {
        }

        tcp::socket socket_;
    };
}