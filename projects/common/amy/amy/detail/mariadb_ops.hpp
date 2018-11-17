#ifndef __AMY_MARIADB_OPS_HPP__
#define __AMY_MARIADB_OPS_HPP__

#include <amy/detail/mysql_ops.hpp>

namespace amy {
namespace detail {
namespace mysql_ops {
/*
 * https://mariadb.com/kb/en/library/using-the-non-blocking-library/
 *
  The MariaDB non-blocking client API is modelled after the normal blocking
library calls. This makes it easy to learn and remember. It makes it easier to
translate code from using the blocking API to using the non-blocking API (or
vice versa). And it also makes it simple to mix blocking and non-blocking calls
in the same code path.

For every library call that may block on socket I/O, such as 'int
mysql_real_query(MYSQL, query, query_length)', two additional non-blocking calls
are introduced:

int mysql_real_query_start(&status, MYSQL, query, query_length)
int mysql_real_query_cont(&status, MYSQL, wait_status)

To do non-blocking operation, an application first calls
mysql_real_query_start() instead of mysql_real_query(), passing the same
parameters.

If mysql_real_query_start() returns zero, then the operation completed without
blocking, and 'status' is set to the value that would normally be returned from
mysql_real_query().

Else, the return value from mysql_real_query_start() is a bitmask of events that
the library is waiting on. This can be MYSQL_WAIT_READ, MYSQL_WAIT_WRITE, or
MYSQL_WAIT_EXCEPT, corresponding to the similar flags for select() or poll();
and it can include MYSQL_WAIT_TIMEOUT when waiting for a timeout to occur (e.g.
a connection timeout).

In this case, the application continues other processing and eventually checks
for the appropriate condition(s) to occur on the socket (or for timeout). When
this occurs, the application can resume the operation by calling
mysql_real_query_cont(), passing in 'wait_status' a bitmask of the events which
actually occurred.

Just like mysql_real_query_start(), mysql_real_query_cont() returns zero when
done, or a bitmask of events it needs to wait on. Thus the application continues
to repeatedly call mysql_real_query_cont(), intermixed with other processing of
its choice; until zero is returned, after which the result of the operation is
stored in 'status'.

Some calls, like mysql_option(), do not do any socket I/O, and so can never
block. For these, there are no separate _start() or _cont() calls. See the
"Non-blocking API reference" page for a full list of what functions can and can
not block.

The checking for events on the socket / timeout can be done with select() or
poll() or a similar mechanism. Though often it will be done using a higher-level
framework (such as libevent), which supplies facilities for registering and
acting on such conditions.

The descriptor of the socket on which to check for events can be obtained by
calling mysql_get_socket(). The duration of any timeout can be obtained from
mysql_get_timeout_value().
 */

using ::mysql_close_cont;
using ::mysql_close_slow_part;
using ::mysql_close_start;
using ::mysql_free_result_cont;
using ::mysql_free_result_start;

using ::mysql_get_socket;

enum wait_type {
  finish        = 0,
  read          = MYSQL_WAIT_READ,
  write         = MYSQL_WAIT_WRITE,
  except        = MYSQL_WAIT_EXCEPT,
  timeout       = MYSQL_WAIT_TIMEOUT,
  read_or_write = read | write,
};

inline auto mysql_get_timeout_value(mysql_handle m) {
  return std::chrono::milliseconds(::mysql_get_timeout_value_ms(m));
}

template <typename F, typename R, typename... Args>
int mysql_start(
    AMY_SYSTEM_NS::error_code& ec, F f, R* r, mysql_handle m, Args... args) {
  clear_error(ec);
  int status = f(r, m, args...);
  if (status == wait_type::finish) error_wrapper(*r, m, ec);
  else if (status | wait_type::except){
    ec = amy::error::unknown;
    error_wrapper(*r, m, ec);
  }
  return status;
}

template <typename F, typename R, typename... Args>
int mysql_continue(
    AMY_SYSTEM_NS::error_code& ec, F f, R* r, mysql_handle m, int status) {
  clear_error(ec);
  int status2 = f(r, m, status);
  if (status2 == wait_type::finish) error_wrapper(*r, m, ec);
  else if (status2 | wait_type::except) {
    ec = amy::error::unknown;
    error_wrapper(*r, m, ec);
  }
  return status2;
}

inline int mysql_autocommit_start(
    my_bool* ret, mysql_handle m, bool mode, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_start(ec, ::mysql_autocommit_start, ret, m, mode ? 1 : 0);
}

inline int mysql_autocommit_cont(
    my_bool* ret, mysql_handle m, int status, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_continue(ec, ::mysql_autocommit_cont, ret, m, status);
}

inline int mysql_real_connect_start(mysql_handle* ret, mysql_handle m,
    char const* host, char const* user, char const* password,
    char const* database, unsigned int port, char const* unix_socket,
    unsigned long client_flag, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_start(ec, ::mysql_real_connect_start, ret, m, host, user,
      password, database, port, unix_socket, client_flag);
}

inline int mysql_real_connect_cont(mysql_handle* ret, mysql_handle m,
    int status, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_continue(ec, ::mysql_real_connect_cont, ret, m, status);
}

inline int mysql_real_query_start(int* ret, mysql_handle m,
    char const* stmt_str, unsigned long length, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_start(ec, ::mysql_real_query_start, ret, m, stmt_str, length);
}

inline int mysql_real_query_cont(
    int* ret, mysql_handle m, int status, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_continue(ec, ::mysql_real_query_cont, ret, m, status);
}

inline int mysql_store_result_start(
    result_set_handle* ret, mysql_handle m, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_start(ec, ::mysql_store_result_start, ret, m);
}

inline int mysql_store_result_cont(result_set_handle* ret, mysql_handle m,
    int status, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_continue(ec, ::mysql_store_result_cont, ret, m, status);
}

inline int mysql_next_result_start(
    int* ret, mysql_handle m, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_start(ec, ::mysql_next_result_start, ret, m);
}

inline int mysql_next_result_cont(
    int* ret, mysql_handle m, int status, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_continue(ec, ::mysql_next_result_cont, ret, m, status);
}

inline int mysql_fetch_row_start(row_type* ret, detail::mysql_handle m,
    result_set_handle rs, AMY_SYSTEM_NS::error_code& ec) {
  clear_error(ec);
  int status = ::mysql_fetch_row_start(ret, rs);
  if (status == 0) error_wrapper(*ret, m, ec);
  else if (status | wait_type::except) {
    ec = amy::error::unknown;
    error_wrapper(*ret, m, ec);
  }
  return status;
}

inline int mysql_fetch_row_cont(row_type* ret, mysql_handle m,
    result_set_handle rs, int status, AMY_SYSTEM_NS::error_code& ec) {
  clear_error(ec);
  int status2 = mysql_fetch_row_cont(ret, rs, status);
  if (status2 == 0) error_wrapper(*ret, m, ec);
  else if (status2 | wait_type::except) {
    ec = amy::error::unknown;
    error_wrapper(*ret, m, ec);
  }
  return status2;
}

inline int mysql_commit_start(
    my_bool* ret, detail::mysql_handle m, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_start(ec, ::mysql_commit_start, ret, m);
}

inline int mysql_commit_cont(my_bool* ret, detail::mysql_handle m, int status,
    AMY_SYSTEM_NS::error_code& ec) {
  return mysql_continue(ec, ::mysql_commit_cont, ret, m, status);
}

inline int mysql_rollback_start(
    my_bool* ret, detail::mysql_handle m, AMY_SYSTEM_NS::error_code& ec) {
  return mysql_start(ec, ::mysql_rollback_start, ret, m);
}

inline int mysql_rollback_cont(my_bool* ret, detail::mysql_handle m, int status,
    AMY_SYSTEM_NS::error_code& ec) {
  return mysql_continue(ec, ::mysql_rollback_cont, ret, m, status);
}
} // namespace mysql_ops
} // namespace detail
} // namespace amy

#endif // __AMY_MARIADB_OPS_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
