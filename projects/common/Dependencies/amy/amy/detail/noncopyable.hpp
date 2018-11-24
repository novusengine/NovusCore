#ifndef __AMY_DETAIL_NONCOPYABLE_HPP__
#define __AMY_DETAIL_NONCOPYABLE_HPP__

namespace amy {
namespace detail {

class noncopyable {
protected:
    constexpr noncopyable() = default;

    ~noncopyable() = default;

    noncopyable(noncopyable const&) = delete;

    noncopyable& operator=(noncopyable const&) = delete;

}; // class noncopyable

} // namespace detail
} // namespace amy

#endif // __AMY_DETAIL_NONCOPYABLE_HPP__
