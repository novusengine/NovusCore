#ifndef __AMY_FIELD_INFO_HPP__
#define __AMY_FIELD_INFO_HPP__

#include <amy/detail/mysql_types.hpp>

#include <cassert>
#include <string>

namespace amy {

class field_info {
public:
    explicit field_info(detail::field_handle f) :
        field_(f)
    {
        assert(nullptr != f);
    }

    field_info(field_info const& other) :
        field_(other.field_)
    {
        assert(nullptr != field_);
    }

    field_info& operator=(field_info const& other) {
        field_ = other.field_;
        assert(nullptr != field_);
        return *this;
    }

    std::string name() const {
        return std::string(field_->name, field_->name_length);
    }

    unsigned long length() const {
        return field_->length;
    }

    unsigned long max_length() const {
        return field_->max_length;
    }

    bool is_nullable() const {
        return !(field_->flags & NOT_NULL_FLAG);
    }

    bool is_primary_key() const {
        return field_->flags & PRI_KEY_FLAG;
    }

    bool is_unique_key() const {
        return field_->flags & UNIQUE_KEY_FLAG;
    }

    bool is_multiple_key() const {
        return field_->flags & MULTIPLE_KEY_FLAG;
    }

    bool is_unsigned() const {
        return field_->flags & UNSIGNED_FLAG;
    }

    bool is_zerofill() const {
        return field_->flags & ZEROFILL_FLAG;
    }

    bool is_binary() const {
        return field_->flags & BINARY_FLAG;
    }

    bool is_auto_increment() const {
        return field_->flags & AUTO_INCREMENT_FLAG;
    }

    bool has_default_value() const {
        return is_nullable() ||
               is_auto_increment() ||
               !(field_->flags & NO_DEFAULT_VALUE_FLAG);
    }

private:
    detail::field_handle field_;

}; // class field_info

} // namespace amy

#endif // __AMY_FIELD_INFO_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
