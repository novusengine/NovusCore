#ifndef __AMY_DETAIL_MYSQL_TYPES_HPP__
#define __AMY_DETAIL_MYSQL_TYPES_HPP__

#include <amy/detail/mysql.hpp>

#include <cstdint>

namespace amy {
namespace detail {

typedef MYSQL  mysql_type;
typedef MYSQL* mysql_handle;

using ::my_bool;

const mysql_handle invalid_mysql_handle = static_cast<mysql_handle>(nullptr);

typedef MYSQL_RES  result_set_type;
typedef MYSQL_RES* result_set_handle;

const result_set_handle
    invalid_result_set_handle = static_cast<result_set_handle>(nullptr);

typedef MYSQL_ROW row_type;
typedef MYSQL_ROW_OFFSET row_offset;

typedef MYSQL_FIELD  field_type;
typedef MYSQL_FIELD* field_handle;

typedef unsigned long client_flags;

// Client connect options
const int client_compress         = CLIENT_COMPRESS;
const int client_found_rows       = CLIENT_FOUND_ROWS;
const int client_ignore_sigpipe   = CLIENT_IGNORE_SIGPIPE;
const int client_ignore_space     = CLIENT_IGNORE_SPACE;
const int client_interactive      = CLIENT_INTERACTIVE;
const int client_local_files      = CLIENT_LOCAL_FILES;
const int client_multi_results    = CLIENT_MULTI_RESULTS;
const int client_multi_statements = CLIENT_MULTI_STATEMENTS;
const int client_no_schema        = CLIENT_NO_SCHEMA;
const int client_odbc             = CLIENT_ODBC;
const int client_ssl              = CLIENT_SSL;

// MySQL options
const int init_command            = MYSQL_INIT_COMMAND;
const int compress                = MYSQL_OPT_COMPRESS;
const int connect_timeout         = MYSQL_OPT_CONNECT_TIMEOUT;
const int guess_connection        = MYSQL_OPT_GUESS_CONNECTION;
const int local_infile            = MYSQL_OPT_LOCAL_INFILE;
const int named_pipe              = MYSQL_OPT_NAMED_PIPE;
const int protocol                = MYSQL_OPT_PROTOCOL;
const int read_timeout            = MYSQL_OPT_READ_TIMEOUT;
const int reconnect               = MYSQL_OPT_RECONNECT;
const int set_client_ip           = MYSQL_SET_CLIENT_IP;
const int ssl_verify_server_cert  = MYSQL_OPT_SSL_VERIFY_SERVER_CERT;
const int use_embedded_connection = MYSQL_OPT_USE_EMBEDDED_CONNECTION;
const int use_remote_connection   = MYSQL_OPT_USE_REMOTE_CONNECTION;
const int use_result              = MYSQL_OPT_USE_RESULT;
const int write_timeout           = MYSQL_OPT_WRITE_TIMEOUT;
const int read_default_file       = MYSQL_READ_DEFAULT_FILE;
const int read_default_group      = MYSQL_READ_DEFAULT_GROUP;
const int report_data_truncation  = MYSQL_REPORT_DATA_TRUNCATION;
const int secure_auth             = MYSQL_SECURE_AUTH;
const int set_charset_dir         = MYSQL_SET_CHARSET_DIR;
const int set_charset_name        = MYSQL_SET_CHARSET_NAME;
const int shared_memory_base_name = MYSQL_SHARED_MEMORY_BASE_NAME;

} // namespace detail
} // namespace amy

#ifdef WIN32

using std::int8_t;
using std::int_least8_t;
using std::int_fast8_t;
using std::uint8_t;
using std::uint_least8_t;
using std::uint_fast8_t;

using std::int16_t;
using std::int_least16_t;
using std::int_fast16_t;
using std::uint16_t;
using std::uint_least16_t;
using std::uint_fast16_t;

using std::int32_t;
using std::int_least32_t;
using std::int_fast32_t;
using std::uint32_t;
using std::uint_least32_t;
using std::uint_fast32_t;

using std::int64_t;
using std::int_least64_t;
using std::int_fast64_t;
using std::uint64_t;
using std::uint_least64_t;
using std::uint_fast64_t;

using std::intmax_t;
using std::uintmax_t;

#endif // ifdef WIN32

#endif // __AMY_DETAIL_MYSQL_TYPES_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
