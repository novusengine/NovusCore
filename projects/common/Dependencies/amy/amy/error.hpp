#ifndef __AMY_ERROR_HPP__
#define __AMY_ERROR_HPP__

#include <amy/detail/mysql.hpp>
#include <amy/detail/mysql_types.hpp>

#include <amy/asio.hpp>

namespace amy {
namespace error {

enum client_errors {
    /// Unknown MySQL error
    unknown_error = CR_UNKNOWN_ERROR,

    /// Can't create UNIX socket (%d)
    socket_create_error = CR_SOCKET_CREATE_ERROR,

    /// Can't connect to local MySQL server through socket '%-.100s' (%d)
    connection_error = CR_CONNECTION_ERROR,

    /// Can't connect to MySQL server on '%-.100s' (%d)
    conn_host_error = CR_CONN_HOST_ERROR,

    /// Can't create TCP/IP socket (%d)
    ipsock_error = CR_IPSOCK_ERROR,

    /// Unknown MySQL server host '%-.100s' (%d)
    unknown_host = CR_UNKNOWN_HOST,

    /// MySQL server has gone away
    server_gone_error = CR_SERVER_GONE_ERROR,

    /// Protocol mismatch; server version = %d client version = %d
    version_error = CR_VERSION_ERROR,

    /// MySQL client ran out of memory
    out_of_memory = CR_OUT_OF_MEMORY,

    /// Wrong host info
    wrong_host_info = CR_WRONG_HOST_INFO,

    /// Localhost via UNIX socket
    localhost_connection = CR_LOCALHOST_CONNECTION,

    /// %-.100s via TCP/IP
    tcp_connection = CR_TCP_CONNECTION,

    /// Error in server handshake
    server_handshake_err = CR_SERVER_HANDSHAKE_ERR,

    /// Lost connection to MySQL server during query
    server_lost = CR_SERVER_LOST,

    /// Commands out of sync; you can't run this command now
    commands_out_of_sync = CR_COMMANDS_OUT_OF_SYNC,

    /// Named pipe: %-.32s
    named_pipe_connection = CR_NAMEDPIPE_CONNECTION,

    /// Can't wait for named pipe to host: %-.64s  pipe: %-.32s (%lu)
    named_pipe_wait_error = CR_NAMEDPIPEWAIT_ERROR,

    /// Can't open named pipe to host: %-.64s  pipe: %-.32s (%lu)
    named_pipe_open_error = CR_NAMEDPIPEOPEN_ERROR,

    /// Can't set state of named pipe to host: %-.64s  pipe: %-.32s (%lu)
    named_pipe_setstate_error = CR_NAMEDPIPESETSTATE_ERROR,

    /// Can't initialize character set %-.32s (path: %-.100s)
    cant_read_charset = CR_CANT_READ_CHARSET,

    /// Got packet bigger than 'max_allowed_packet' bytes
    net_packet_too_large = CR_NET_PACKET_TOO_LARGE,

    /// Embedded server
    embedded_connection = CR_EMBEDDED_CONNECTION,

    /// Error on SHOW SLAVE STATUS
    probe_slave_status = CR_PROBE_SLAVE_STATUS,

    /// Error on SHOW SLAVE HOSTS
    probe_slave_hosts = CR_PROBE_SLAVE_HOSTS,

    /// Error connecting to slave
    probe_slave_connect = CR_PROBE_SLAVE_CONNECT,

    /// Error connecting to master
    probe_master_connect = CR_PROBE_MASTER_CONNECT,

    /// SSL connection error
    ssl_connection_error = CR_SSL_CONNECTION_ERROR,

    /// Malformed packet
    malformed_packet = CR_MALFORMED_PACKET,

    /// This client library is licensed only for use with MySQL servers having
    /// '%s' license
    wrong_license = CR_WRONG_LICENSE,

    /// Invalid use of null pointer
    null_pointer = CR_NULL_POINTER,

    /// Statement not prepared
    no_prepare_stmt = CR_NO_PREPARE_STMT,

    /// No data supplied for parameters in prepared statement
    params_not_bound = CR_PARAMS_NOT_BOUND,

    /// Data truncated
    data_truncated = CR_DATA_TRUNCATED,

    /// No parameters exist in the statement
    no_parameters_exists = CR_NO_PARAMETERS_EXISTS,

    /// Invalid parameter number
    invalid_parameter_no = CR_INVALID_PARAMETER_NO,

    /// Can't send long data for non-string/non-binary data types
    /// (parameter: %d)
    invalid_buffer_use = CR_INVALID_BUFFER_USE,

    /// Using unsupported buffer type: %d  (parameter: %d)
    unsupported_param_type = CR_UNSUPPORTED_PARAM_TYPE,

    /// Shared memory: %-.100s
    shared_memory_connection = CR_SHARED_MEMORY_CONNECTION,

    /// Can't open shared memory; client could not create request event (%lu)
    shared_memory_connect_request_error =
        CR_SHARED_MEMORY_CONNECT_REQUEST_ERROR,

    /// Can't open shared memory; no answer event received from server (%lu)
    shared_memory_connect_answer_error = CR_SHARED_MEMORY_CONNECT_ANSWER_ERROR,

    /// Can't open shared memory; server could not allocate file mapping (%lu)
    shared_memory_connect_file_map_error =
        CR_SHARED_MEMORY_CONNECT_FILE_MAP_ERROR,

    /// Can't open shared memory; server could not get pointer to file mapping
    /// (%lu)
    shared_memory_connect_map_error = CR_SHARED_MEMORY_CONNECT_MAP_ERROR,

    /// Can't open shared memory; client could not allocate file mapping (%lu)
    shared_memory_file_map_error = CR_SHARED_MEMORY_FILE_MAP_ERROR,

    /// Can't open shared memory; client could not get pointer to file mapping
    /// (%lu)
    shared_memory_map_error = CR_SHARED_MEMORY_MAP_ERROR,

    /// Can't open shared memory; client could not create %s event (%lu)
    shared_memory_event_error = CR_SHARED_MEMORY_EVENT_ERROR,

    /// Can't open shared memory; no answer from server (%lu)
    shared_memory_connect_abandoned_error =
        CR_SHARED_MEMORY_CONNECT_ABANDONED_ERROR,

    /// Can't open shared memory; cannot send request event to server (%lu)
    shared_memory_connect_set_error = CR_SHARED_MEMORY_CONNECT_SET_ERROR,

    /// Wrong or unknown protocol
    conn_unknow_protocol = CR_CONN_UNKNOW_PROTOCOL,

    /// Invalid connection handle
    invalid_conn_handle = CR_INVALID_CONN_HANDLE,

    /// Connection using old (pre-4.1.1) authentication protocol refused
    /// (client option 'secure_auth' enabled)
    secure_auth = MYSQL_SECURE_AUTH,

    /// Row retrieval was canceled by mysql_stmt_close() call
    fetch_canceled = CR_FETCH_CANCELED,

    /// Attempt to read column without prior row fetch
    no_data = CR_NO_DATA,

    /// Prepared statement contains no metadata
    no_stmt_metadata = CR_NO_STMT_METADATA,

    /// Attempt to read a row while there is no result set associated with the
    /// statement
    no_result_set = CR_NO_RESULT_SET,

    /// This feature is not implemented yet
    not_implemented = CR_NOT_IMPLEMENTED,

    /// Lost connection to MySQL server at '%s' system error: %d
    // server_lost_extended = CR_SERVER_LOST_EXTENDED,

}; // enum client_errors

enum misc_errors {
    // Failed to initialize MySQL handle
    initialization_error = 1,

    // Connection handle not initialized
    not_initialized = 2,

    // No more result sets
    no_more_results = 3,

    // Field value is NULL
    null_field_value = 4,

    // Failed to set autocommit mode
    autocommit_setting_error = 5,

    // Failed to commit
    commit_error = 6,

    // Failed to rollback
    rollback_error = 7,

    // Unknown error
    unknown,

}; // enum misc_errors

namespace detail {

class client_category : public AMY_SYSTEM_NS::error_category {
public:
    explicit client_category() :
        AMY_SYSTEM_NS::error_category()
    {}

    char const* name() const noexcept {
        return "mysql";
    }

    std::string message(int value) const {
        switch (value) {
            case unknown_error:
                return "Unknown MySQL error";

            case socket_create_error:
                return "Can't create UNIX socket";

            case connection_error:
                return "Can't connect to local MySQL server through socket";

            case conn_host_error:
                return "Can't connect to MySQL server";

            case ipsock_error:
                return "Can't create TCP/IP socket";

            case unknown_host:
                return "Unknown MySQL server host";

            case server_gone_error:
                return "MySQL server has gone away";

            case version_error:
                return "Protocol mismatch";

            case out_of_memory:
                return "MySQL client ran out of memory";

            case wrong_host_info:
                return "Wrong host info";

            case localhost_connection:
                return "Localhost via UNIX socket";

            case tcp_connection:
                return "TCP/IP connection failed";

            case server_handshake_err:
                return "Error in server handshake";

            case server_lost:
                return "Lost connection to MySQL server during query";

            case commands_out_of_sync:
                return "Commands out of sync; you can't run this command now";

            case named_pipe_connection:
                return "Named pipe connection failed";

            case named_pipe_wait_error:
                return "Can't wait for named pipe to host";

            case named_pipe_open_error:
                return "Can't open named pipe to host";

            case named_pipe_setstate_error:
                return "Can't set state of named pipe to host";

            case cant_read_charset:
                return "Can't initialize character set";

            case net_packet_too_large:
                return "Got packet bigger than 'max_allowed_packet' bytes";

            case embedded_connection:
                return "Embedded server";

            case probe_slave_status:
                return "Error on SHOW SLAVE STATUS";

            case probe_slave_hosts:
                return "Error on SHOW SLAVE HOSTS";

            case probe_slave_connect:
                 return "Error connecting to slave";

            case probe_master_connect:
                return "Error connecting to master";

            case ssl_connection_error:
                return "SSL connection error";

            case malformed_packet:
                return "Malformed packet";

            case wrong_license:
                return "This client library has wrong license";

            case null_pointer:
                return "Invalid use of null pointer";

            case no_prepare_stmt:
                return "Statement not prepared";

            case params_not_bound:
                return "No data supplied for parameters in prepared statement";

            case data_truncated:
                return "Data truncated";

            case no_parameters_exists:
                return "No parameters exist in the statement";

            case invalid_parameter_no:
                return "Invalid parameter number";

            case invalid_buffer_use:
                return "Can't send long data for non-string/non-binary data types";

            case unsupported_param_type:
                return "Using unsupported buffer type";

            case shared_memory_connection:
                return "Shared memory connection failed";

            case shared_memory_connect_request_error:
                return "Can't open shared memory; client could not create "
                       "request event";

            case shared_memory_connect_answer_error:
                return "Can't open shared memory; no answer event received "
                       "from server";

            case shared_memory_connect_file_map_error:
                return "Can't open shared memory; server could not allocate "
                       "file mapping";

            case shared_memory_connect_map_error:
                return "Can't open shared memory; server could not get pointer "
                       "to file mapping";

            case shared_memory_file_map_error:
                return "Can't open shared memory; client could not allocate "
                       "file mapping";

            case shared_memory_map_error:
                return "Can't open shared memory; client could not get pointer "
                       "to file mapping";

            case shared_memory_event_error:
                return "Can't open shared memory; client could not create "
                       "event";

            case shared_memory_connect_abandoned_error:
                return "Can't open shared memory; no answer from server";

            case shared_memory_connect_set_error:
                return "Can't open shared memory; cannot send request event to "
                       "server";

            case conn_unknow_protocol:
                return "Wrong or unknown protocol";

            case invalid_conn_handle:
                return "Invalid connection handle";

            case secure_auth:
                return "Connection using old (pre-4.1.1) authentication "
                       "protocol refused (client option 'secure_auth' enabled)";

            case fetch_canceled:
                return "Row retrieval was canceled by mysql_stmt_close() call";

            case no_data:
                return "Attempt to read column without prior row fetch";

            case no_stmt_metadata:
                return "Prepared statement contains no metadata";

            case no_result_set:
                return "Attempt to read a row while there is no result set "
                       "associated with the statement";

            case not_implemented:
                return "This feature is not implemented yet";

            default:
                return "Unknown error";
        }
    }

}; // class client_category

} // namespace detail

inline AMY_SYSTEM_NS::error_category& get_client_category() {
    static detail::client_category instance;
    return instance;
}

namespace detail {

class misc_category : public AMY_SYSTEM_NS::error_category {
public:
    char const* name() const noexcept {
        return "mysql misc";
    }

    std::string message(int value) const {
        static char const* messages[] = {
            "",
            "Failed to initialize MySQL handle",
            "Connection handle not initialized",
            "No more result sets",
            "Field value is NULL",
            "Failed to set autocommit mode",
            "Failed to commit",
            "Failed to rollback",
            "Unknown error",
        };

        if (value < 0 || value >= unknown_error) {
            return std::string(messages[error::unknown]);
        }

        return std::string(messages[value]);
    }

}; // class misc_category

} // namespace detail

inline AMY_SYSTEM_NS::error_category const& get_misc_category() {
    static detail::misc_category instance;
    return instance;
}

} // namespace error
} // namespace amy

#if !defined(USE_BOOST_ASIO) || (USE_BOOST_ASIO == 0)
namespace std {
#else
namespace boost {
namespace system {
#endif

template<>
struct is_error_code_enum<amy::error::client_errors> {
    static const bool value = true;

}; // struct is_error_code_enum

template<>
struct is_error_code_enum<amy::error::misc_errors> {
    static const bool value = true;

}; // struct is_error_code_enum

#if !defined(USE_BOOST_ASIO) || (USE_BOOST_ASIO == 0)
} // namespace std
#else
} // namespace system
} // namespace boost
#endif

namespace amy {
namespace error {

inline AMY_SYSTEM_NS::error_code make_error_code(client_errors e) {
    return AMY_SYSTEM_NS::error_code(static_cast<int>(e),
                                      get_client_category());
}

inline AMY_SYSTEM_NS::error_code make_error_code(misc_errors e) {
    return AMY_SYSTEM_NS::error_code(static_cast<int>(e),
                                      get_misc_category());
}

} // namespace error
} // namespace amy

#endif // __AMY_ERROR_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
