#ifndef __AMY_DETAIL_MYSQL_OPTION_HPP__
#define __AMY_DETAIL_MYSQL_OPTION_HPP__

#include <amy/detail/mysql_types.hpp>

#include <string>

namespace amy {
namespace detail {
namespace mysql_option {

template<int Option, typename Uint = unsigned int>
class unsigned_integer {
public:
    unsigned_integer() :
        value_(0u)
    {}

    explicit unsigned_integer(unsigned int value) :
        value_(value)
    {}

    int option() const {
        return Option;
    }

    char const* data() const {
        return reinterpret_cast<char const*>(&value_);
    }

private:
    Uint value_;
}; // class unsigned_integer

template<int Option>
class char_sequence {
public:
    char_sequence() :
        value_()
    {}

    explicit char_sequence(std::string const& value) :
        value_(value)
    {}

    int option() const {
        return Option;
    }

    char const* data() const {
        return value_.c_str();
    }

private:
    std::string value_;

}; // class char_sequence

template<int Option>
class boolean {
public:
    boolean() :
        value_(false)
    {}

    explicit boolean(bool value) :
        value_(value ? 1 : 0)
    {}

    int option() const {
        return Option;
    }

    char const* data() const {
        return reinterpret_cast<char const*>(&value_);
    }

private:
    detail::my_bool value_;

}; // class boolean

template<int Option>
class switcher {
public:
    switcher() {
    }

    int option() const {
        return Option;
    }

    char* data() const {
        return 0;
    }

}; // class switcher

} // namespace mysql_option
} // namespace detail
} // namespace amy

#endif // __AMY_DETAIL_MYSQL_OPTION_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
