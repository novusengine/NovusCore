#ifndef __AMY_ROW_HPP__
#define __AMY_ROW_HPP__

#include <amy/detail/mysql_ops.hpp>

#include <amy/field.hpp>
#include <amy/field_info.hpp>

#include <ostream>

namespace amy {

class row {
private:
    typedef std::vector<field> fields_type;

    typedef std::vector<field_info> fields_info_type;

public:
    typedef fields_type::const_iterator const_iterator;

    typedef
        fields_type::const_reverse_iterator
        const_reverse_iterator;

    explicit row(detail::result_set_handle rs,
                 detail::row_type row,
                 unsigned long* lengths,
                 std::shared_ptr<fields_info_type> fields_info)
      : result_set_(rs),
        row_(row),
        lengths_(lengths),
        fields_info_(fields_info)
    {
        build_fields();
    }

    detail::row_type native() const {
        return row_;
    }

    const_iterator begin() const {
        return fields_.begin();
    }

    const_iterator end() const {
        return fields_.end();
    }

    const_reverse_iterator rbegin() const {
        return fields_.rbegin();
    }

    const_reverse_iterator rend() const {
        return fields_.rend();
    }

    uint32_t size() const {
        return detail::mysql_ops::mysql_num_fields(result_set_);
    }

    field const& operator[](int index) const {
        return fields_.at(index);
    }

    field const& at(const_iterator::difference_type index) const {
        return fields_.at(index);
    }

    fields_info_type const& fields_info() const {
        return *fields_info_;
    }

private:
    detail::result_set_handle result_set_;
    detail::row_type row_;
    unsigned long* lengths_;
    fields_type fields_;
    std::shared_ptr<fields_info_type> fields_info_;

    void build_fields() {
        uint32_t field_count = size();

        fields_.reserve(field_count);
        for (uint32_t i = 0; i < field_count; ++i) {
            fields_.push_back(field(row_[i], lengths_[i]));
        }
    }

}; // class row

inline std::ostream& operator<<(std::ostream& out, row const& row) {
    for (uint32_t i = 0u; i < row.size(); ++i) {
        out << row[i] << ' ';
    }

    return out;
}

} // namespace amy

#endif // __AMY_ROW_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
