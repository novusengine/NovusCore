#ifndef __AMY_DETAIL_EXECUTE_HANDLER_HPP__
#define __AMY_DETAIL_EXECUTE_HANDLER_HPP__

#include <amy/asio.hpp>

#include <amy/basic_connector.hpp>

namespace amy {
namespace detail {

template<
    typename MySQLService,
    typename ExecuteHandler
>
class execute_handler {
public:
    typedef void result_type;

    execute_handler(basic_connector<MySQLService>& connector,
                    ExecuteHandler handler)
      : connector(connector),
        handler(handler)
    {}

    void operator()(AMY_SYSTEM_NS::error_code const& ec) {
        handler(ec, !!ec ? connector.affected_rows() : 0u);
    }

private:
    basic_connector<MySQLService>& connector;
    ExecuteHandler handler;

}; // class execute_handler

} // namespace detail
} // namespace amy

#endif  //  __AMY_DETAIL_EXECUTE_HANDLER_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
