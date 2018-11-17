#ifndef __AMY_MARIADB_OPTIONS_HPP__
#define __AMY_MARIADB_OPTIONS_HPP__

#include <amy/options.hpp>
#include <amy/detail/mariadb_types.hpp>
#include <amy/detail/mysql_option.hpp>

namespace amy {
namespace options {

using namespace detail::mysql_option;

using nonblock = unsigned_integer<detail::nonblock, std::size_t>;
using nonblock_default = switcher<detail::nonblock>;

} // namespace options
} // namespace amy

#endif /* __AMY_MARIADB_OPTIONS_HPP__ */
