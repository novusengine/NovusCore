#ifndef __AMY_PLACEHOLDERS_HPP__
#define __AMY_PLACEHOLDERS_HPP__

#include <amy/asio.hpp>

#include <type_traits>

namespace amy {
namespace placeholders {

struct _ph_error {};
struct _ph_result_set {};
struct _ph_affected_rows {};

constexpr _ph_error         error {};
constexpr _ph_result_set    result_set {};
constexpr _ph_affected_rows affected_rows {};

} // namespace placeholders
} // namespace amy

namespace std {

template<>
struct is_placeholder<amy::placeholders::_ph_error> :
    std::integral_constant<int, 1>
{};

template<>
struct is_placeholder<amy::placeholders::_ph_result_set> :
    std::integral_constant<int, 2>
{};

template<>
struct is_placeholder<amy::placeholders::_ph_affected_rows> :
    std::integral_constant<int, 2>
{};

} // std

#endif // __AMY_PLACEHOLDERS_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
