#ifndef __AMY_DETAIL_ENDPOINT_TRAITS_HPP__
#define __AMY_DETAIL_ENDPOINT_TRAITS_HPP__

#include <amy/asio.hpp>

namespace amy {

using namespace AMY_ASIO_NS;

template<typename Endpoint>
class endpoint_traits;

struct null_endpoint {};

template<>
class endpoint_traits<null_endpoint> {
public:
    endpoint_traits<null_endpoint>(null_endpoint const&) {
    }

    char const* host() const {
        return 0;
    }

    unsigned int port() const {
        return 0;
    }

    char const* unix_socket() const {
        return 0;
    }

}; // class endpoint_traits<null_endpoint>

template<>
class endpoint_traits<ip::tcp::endpoint> {
public:
    endpoint_traits(ip::tcp::endpoint const& endpoint) :
        host_(endpoint.address().to_string()),
        port_(endpoint.port())
    {}

    char const* host() const {
        return host_.c_str();
    }

    unsigned int port() const {
        return port_;
    }

    char const* unix_socket() const {
        return 0;
    }

private:
    std::string host_;
    unsigned int port_;

}; // struct endpoint_traits<ip::tcp::endpoint>

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

template<>
class endpoint_traits<local::stream_protocol::endpoint> {
public:
    endpoint_traits(local::stream_protocol::endpoint const& endpoint) :
        unix_socket_(endpoint.path())
    {}

    char const* host() const {
        return 0;
    }

    unsigned int port() const {
        return 0u;
    }

    char const* unix_socket() const {
        return unix_socket_.c_str();
    }

private:
    std::string unix_socket_;

}; // endpoint_traits<local::stream_protocol::endpoint>

#endif // if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

} // namespace amy

#endif // __AMY_DETAIL_ENDPOINT_TRAITS_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
