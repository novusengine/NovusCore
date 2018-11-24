#ifndef __AMY_DETAIL_THROW_ERROR_HPP__
#define __AMY_DETAIL_THROW_ERROR_HPP__

#include <amy/detail/mysql_types.hpp>

#include <amy/error.hpp>
#include <amy/system_error.hpp>

namespace amy {
namespace detail {

inline void throw_error(AMY_SYSTEM_NS::error_code const& ec,
                        detail::mysql_handle m)
{
    if (ec) {
        if (ec.category() == amy::error::get_client_category()) {
            throw amy::system_error(ec, ::mysql_error(m));
        } else {
            throw amy::system_error(ec);
        }
    }
}

} // namespace detail
} // namespace amy

#endif // __AMY_DETAIL_THROW_ERROR_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
