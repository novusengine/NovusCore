#ifndef __AMY_OPTIONS_HPP__
#define __AMY_OPTIONS_HPP__

#include <amy/detail/mysql_option.hpp>

namespace amy {
namespace options {

using namespace detail::mysql_option;

using init_command            = char_sequence<detail::init_command>;
using compress                = switcher<detail::compress>;
using connect_timeout         = unsigned_integer<detail::connect_timeout>;
using guess_connection        = switcher<detail::guess_connection>;
using local_infile            = unsigned_integer<detail::local_infile>;
using named_pipe              = switcher<detail::named_pipe>;
using protocol                = unsigned_integer<detail::protocol>;
using read_timeout            = unsigned_integer<detail::read_timeout>;
using reconnect               = boolean<detail::reconnect>;
using set_client_ip           = char_sequence<detail::set_client_ip>;
using ssl_verify_server_cert  = boolean<detail::ssl_verify_server_cert>;
using use_embedded_connection = switcher<detail::use_embedded_connection>;
using use_remote_connection   = switcher<detail::use_remote_connection>;
using use_result              = switcher<detail::use_result>;
using write_timeout           = unsigned_integer<detail::write_timeout>;
using read_default_file       = char_sequence<detail::read_default_file>;
using read_default_group      = char_sequence<detail::read_default_group>;
using report_data_truncation  = boolean<detail::report_data_truncation>;
using secure_auth             = boolean<detail::secure_auth>;
using set_charset_dir         = char_sequence<detail::set_charset_dir>;
using set_charset_name        = char_sequence<detail::set_charset_name>;
using shared_memory_base_name = char_sequence<detail::shared_memory_base_name>;

} // namespace options
} // namespace amy

#endif /* __AMY_OPTIONS_HPP__ */
