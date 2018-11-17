#ifndef __AMY_RESULT_SET_HPP__
#define __AMY_RESULT_SET_HPP__

#include <amy/detail/mysql_types.hpp>
#include <amy/detail/throw_error.hpp>

#include <amy/field_info.hpp>
#include <amy/row.hpp>

#include <memory>

namespace amy {

/// Provides MySQL query result set functionality.
/**
 * The \c result_set class wraps the underlying \c MYSQL_RES* pointer returned
 * by a \c mysql_store_result() call. It also provides STL compatible random
 * access iterator over rows in the result set.
 */
class result_set {
private:
    typedef std::vector<row> values_type;

    typedef
        values_type::const_iterator
        values_const_iterator;

    typedef
        values_type::const_reverse_iterator
        values_const_reverse_iterator;

public:
    /// The random access iterator over rows.
    typedef values_const_iterator const_iterator;

    /// The random access reverse iterator over rows.
    typedef values_const_reverse_iterator const_reverse_iterator;

    /// The native representation of a MySQL query result set.
    typedef detail::result_set_handle native_type;

    typedef detail::mysql_handle native_mysql_type;

    typedef std::vector<field_info> fields_info_type;

    explicit result_set() :
        mysql_(nullptr),
        values_(new values_type),
        fields_info_(new fields_info_type)
    {}

    result_set(result_set const& other) :
        mysql_(other.mysql_),
        result_set_(other.result_set_),
        values_(other.values_),
        fields_info_(other.fields_info_)
    {}

    explicit result_set(native_mysql_type mysql) :
        mysql_(mysql),
        values_(new values_type()),
        fields_info_(new fields_info_type)
    {}

    result_set const& operator=(result_set const& other) {
        mysql_ = other.mysql_;
        result_set_ = other.result_set_;
        values_ = other.values_;
        fields_info_ = other.fields_info_;

        return *this;
    }

    void assign(native_mysql_type mysql,
                std::shared_ptr<detail::result_set_type> rs)
    {
        AMY_SYSTEM_NS::error_code ec;
        assign(mysql, rs, ec);
        detail::throw_error(ec, mysql);
    }

    AMY_SYSTEM_NS::error_code assign(
            native_mysql_type mysql,
            std::shared_ptr<detail::result_set_type> rs,
            AMY_SYSTEM_NS::error_code& ec )
    {
        namespace ops = amy::detail::mysql_ops;

        mysql_ = mysql;

        if (!rs) {
            return ec;
        }

        result_set_ = rs;
        uint64_t row_count = ops::mysql_num_rows(rs.get());

        if (row_count == 0) {
            return ec;
        }

        // Fetch fields information.
        uint32_t field_count = ops::mysql_num_fields(rs.get());
        fields_info_.reset(new fields_info_type);
        fields_info_->reserve(field_count);
        detail::field_handle f = nullptr;

        while ((f = ops::mysql_fetch_field(rs.get()))) {
            fields_info_->push_back(field_info(f));
        }

        // Fetch rows.
        values_->reserve(static_cast<size_t>(row_count));
        detail::row_type r;

        while ((r = ops::mysql_fetch_row(mysql_, rs.get(), ec))) {
            unsigned long* lengths = ops::mysql_fetch_lengths(rs.get());
            values_->push_back(row(rs.get(), r, lengths, fields_info_));
        }

        if (ec) {
            values_.reset();
            fields_info_.reset();
            result_set_.reset();
        }

        return ec;
    }

    static result_set empty_set(native_mysql_type mysql) {
        return result_set(mysql);
    }

    const_iterator begin() const {
        return values_->begin();
    }

    const_iterator end() const {
        return values_->end();
    }

    const_reverse_iterator rbegin() const {
        return values_->rbegin();
    }

    const_reverse_iterator rend() const {
        return values_->rend();
    }

    bool empty() const {
        std::shared_ptr<detail::result_set_type> p = result_set_.lock();
        return !p.get() || !size();
    }

    uint64_t size() const {
        std::shared_ptr<detail::result_set_type> p = result_set_.lock();
        return p.get() ?  detail::mysql_ops::mysql_num_rows(p.get()) : 0u;
    }

    row const& operator[](const_iterator::difference_type index) const {
        assert(!expired());
        return values_->at(index);
    }

    row const& at(const_iterator::difference_type index) const {
		assert(!expired());
        return values_->at(index);
    }

    native_type native() const {
        std::shared_ptr<detail::result_set_type> p = result_set_.lock();
        return p.get();
    }

    fields_info_type const& fields_info() const {
		assert(!expired());
        return *fields_info_;
    }

    uint32_t field_count() const {
        return detail::mysql_ops::mysql_field_count(mysql_);
    }

    uint64_t affected_rows() const {
        return detail::mysql_ops::mysql_affected_rows(mysql_);
    }

    bool expired() const {
        return result_set_.expired();
    }

private:
    native_mysql_type mysql_;
    std::weak_ptr<detail::result_set_type> result_set_;
    std::shared_ptr<values_type> values_;
    std::shared_ptr<fields_info_type> fields_info_;

}; // class result_set

} // namespace amy

#endif // __AMY_RESULT_SET_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
