#ifndef __AMY_MYSQL_OPS_HPP__
#define __AMY_MYSQL_OPS_HPP__

#include <amy/detail/mysql.hpp>
#include <amy/detail/mysql_types.hpp>

#include <amy/error.hpp>

#include <cerrno>
#include <stdexcept>

namespace amy {
namespace detail {
namespace mysql_ops {

using ::mysql_affected_rows;
using ::mysql_close;
using ::mysql_data_seek;
using ::mysql_fetch_field;
using ::mysql_fetch_lengths;
using ::mysql_free_result;
using ::mysql_hex_string;
using ::mysql_num_fields;
using ::mysql_num_rows;
using ::mysql_real_escape_string;
using ::mysql_row_seek;
using ::mysql_row_tell;

inline void clear_error(AMY_SYSTEM_NS::error_code& ec) {
    errno = 0; // this won't clear the ::mysql_errno()
    ec = AMY_SYSTEM_NS::error_code();
}

template<typename ReturnType>
ReturnType error_wrapper(ReturnType return_value,
                         mysql_handle m,
                         AMY_SYSTEM_NS::error_code& ec);

template<typename T>
T* error_wrapper(T* return_value,
                 mysql_handle m,
                 AMY_SYSTEM_NS::error_code& ec)
{
    if (return_value == nullptr)
        ec = AMY_SYSTEM_NS::error_code(::mysql_errno(m),
                                       amy::error::get_client_category());
    return return_value;
}

template<>
inline int error_wrapper(int return_value,
                         mysql_handle m,
                         AMY_SYSTEM_NS::error_code& ec)
{
    if (return_value != 0)
        ec = AMY_SYSTEM_NS::error_code(::mysql_errno(m),
                                       amy::error::get_client_category());
    return return_value;
}

inline mysql_handle mysql_init(mysql_handle m, AMY_SYSTEM_NS::error_code& ec) {
    clear_error(ec);
    mysql_handle ret = ::mysql_init(m);

    // ::mysql_init() only fails on out of memory.
    if (invalid_mysql_handle == ret) {
        ec = amy::error::make_error_code(
                amy::error::initialization_error);
    }

    return ret;
}

inline void mysql_autocommit(mysql_handle m,
                             bool mode,
                             AMY_SYSTEM_NS::error_code& ec)
{
    clear_error(ec);
    if (::mysql_autocommit(m, mode ? 1 : 0)) {
        ec = amy::error::autocommit_setting_error;
    }
}

inline mysql_handle mysql_real_connect(mysql_handle m,
                                       char const* host,
                                       char const* user,
                                       char const* password,
                                       char const* database,
                                       unsigned int port,
                                       char const* unix_socket,
                                       unsigned long client_flag,
                                       AMY_SYSTEM_NS::error_code& ec)
{
    clear_error(ec);
    mysql_handle h = ::mysql_real_connect(m, host, user, password, database,
                                          port, unix_socket, client_flag);
    return error_wrapper(h, m, ec);
}

inline int32_t mysql_real_query(mysql_handle m,
                                char const* stmt_str,
                                unsigned long length,
                                AMY_SYSTEM_NS::error_code& ec)
{
    clear_error(ec);
    return error_wrapper(::mysql_real_query(m, stmt_str, length), m, ec);
}

inline uint32_t mysql_field_count(const mysql_handle m) {
    // ::mysql_field_count() never fails.
    return ::mysql_field_count(m);
}

inline result_set_handle mysql_store_result(mysql_handle m,
                                            AMY_SYSTEM_NS::error_code& ec)
{
    clear_error(ec);
    return error_wrapper(::mysql_store_result(m), m, ec);
}

inline bool mysql_more_results(mysql_handle m) {
    // ::mysql_more_results() never fails.
    return !!::mysql_more_results(m);
}

inline int mysql_next_result(mysql_handle m, AMY_SYSTEM_NS::error_code& ec) {
    clear_error(ec);
    return error_wrapper(::mysql_next_result(m), m, ec);
}

inline row_type mysql_fetch_row(detail::mysql_handle m,
                                result_set_handle r,
                                AMY_SYSTEM_NS::error_code& ec)
{
    clear_error(ec);
    return error_wrapper(::mysql_fetch_row(r), m, ec);
}

inline void mysql_commit(detail::mysql_handle m,
                         AMY_SYSTEM_NS::error_code& ec)
{
    clear_error(ec);
    if (::mysql_commit(m)) {
        ec = amy::error::commit_error;
    }
}

inline void mysql_rollback(detail::mysql_handle m,
                           AMY_SYSTEM_NS::error_code& ec)
{
    clear_error(ec);
    if (::mysql_rollback(m)) {
        ec = amy::error::rollback_error;
    }
}

inline void mysql_options(detail::mysql_handle m,
                          int option,
                          char const* arg,
                          AMY_SYSTEM_NS::error_code& ec)
{
    clear_error(ec);
    error_wrapper(::mysql_options(m, (enum mysql_option)option, arg), m, ec);
}

} // namespace mysql_ops
} // namespace detail
} // namespace amy

#endif // __AMY_MYSQL_OPS_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
