#ifndef __AMY_EXECUTE_HPP__
#define __AMY_EXECUTE_HPP__

#include <amy/detail/execute_handler.hpp>
#include <amy/detail/throw_error.hpp>

#include <amy/basic_connector.hpp>
#include <amy/placeholders.hpp>

namespace amy {

template<typename MySQLService>
uint64_t execute(basic_connector<MySQLService>& connector,
                 std::string const& stmt)
{
    AMY_SYSTEM_NS::error_code ec;
    uint64_t affected_rows = execute(connector, stmt, ec);
    detail::throw_error(ec, connector.native());
    return affected_rows;
}

template<typename MySQLService>
uint64_t execute(basic_connector<MySQLService>& connector,
                 std::string const& stmt,
                 AMY_SYSTEM_NS::error_code& ec)
{
    connector.query(stmt, ec);
    if (ec) {
        return 0u;
    } else {
        return connector.affected_rows();
    }
}

template<
    typename MySQLService,
    typename ExecuteHandler
>
void async_execute(basic_connector<MySQLService>& connector,
                   std::string const& stmt,
                   ExecuteHandler handler)
{
    typedef
        detail::execute_handler<MySQLService, ExecuteHandler>
        execute_handler_type;

    connector.async_query(stmt, execute_handler_type(connector, handler));
}

} // namespace amy

#endif // __AMY_EXECUTE_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
