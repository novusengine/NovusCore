#ifndef __AMY_FIELD_HPP__
#define __AMY_FIELD_HPP__

#include <amy/detail/throw_error.hpp>
#include <amy/detail/value_cast.hpp>

#include <amy/error.hpp>

#include <cstring>

namespace amy {

class field {
    friend std::ostream& operator<<(std::ostream&, field const&);

public:
    field(char const* value, unsigned long length) :
        value_str_(value),
        length_(length)
    {}

    field(char const* value) :
        value_str_(value),
        length_(value ? (unsigned long)::strlen(value) : 0ul)
    {}

    bool is_null() const {
        return value_str_ == 0;
    }

    template<typename SQLType>
    SQLType as() const {
        assert(!is_null());
        return detail::value_cast<SQLType>(value_str_, length_);
    }

    char const* data() const {
        return value_str_;
    }

    unsigned long size() const {
        return length_;
    }

private:
    char const* value_str_;
    unsigned long length_;

}; // class field

inline std::ostream& operator<<(std::ostream& out, field const& f) {
    if (f.is_null()) {
        return out << "null";
    } else {
        return out << f.value_str_;
    }
}

} // namespace amy

#endif // __AMY_FIELD_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
