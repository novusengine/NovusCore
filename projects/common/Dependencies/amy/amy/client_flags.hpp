#ifndef __AMY_CLIENT_FLAGS_HPP__
#define __AMY_CLIENT_FLAGS_HPP__

#include <amy/detail/mysql_types.hpp>

namespace amy {

/// Bitmask type of flags that can be passed to connect operations.
using detail::client_flags;

/// Use compression protocol.
const client_flags client_compress = detail::client_compress;

/// Return the number of found (matched) rows, not the number of changed rows.
const client_flags client_found_rows = detail::client_found_rows;

/// Prevents the client library from installing a \c SIGPIPE  signal handler.
/// This can be used to avoid conflicts with a handler that the application has
/// already installed.
const client_flags client_ignore_sigpipe = detail::client_ignore_sigpipe;

/// Allow spaces after function names. Makes all functions names reserved
/// words.
const client_flags client_ignore_space = detail::client_ignore_space;

/// Allow \c interactive_timeout seconds (instead of \c wait_timeout seconds)
/// of inactivity before closing the connection. The client's session \c
/// wait_timeout  variable is set to the value of the session \c
/// interactive_timeout variable.
const client_flags client_interactive = detail::client_interactive;

/// Enable <tt>LOAD DATA LOCAL</tt> handling.
const client_flags client_local_files = detail::client_local_files;

/// Tell the server that the client can handle multiple result sets from
/// multiple-statement executions or stored procedures. This is automatically
/// set if \c CLIENT_MULTI_STATEMENTS is set. See the note following this table
/// for more information about this flag.
const client_flags client_multi_results = detail::client_multi_results;

/// Tell the server that the client may send multiple statements in a single
/// string (separated by ";"). If this flag is not set, multiple-statement
/// execution is disabled. See the note following this table for more
/// information about this flag.
const client_flags client_multi_statements = detail::client_multi_statements;

/// Don't allow the <tt>db_name.tbl_name.col_name</tt> syntax. This is for
/// ODBC. It causes the parser to generate an error if you use that syntax,
/// which is useful for trapping bugs in some ODBC programs.
const client_flags client_no_schema = detail::client_no_schema;

/// Unused.
const client_flags client_odbc = detail::client_odbc;

/// Use SSL (encrypted protocol). This option should not be set by application
/// programs; it is set internally in the client library. Instead, use \c
/// mysql_ssl_set()  before calling \c mysql_real_connect().
const client_flags client_ssl = detail::client_ssl;

/// Default client flags.
const client_flags default_flags = 0;

} // namespace amy

#endif // __AMY_CLIENT_FLAGS_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
