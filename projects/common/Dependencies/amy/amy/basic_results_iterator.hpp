#ifndef __AMY_BASIC_RESULTS_ITERATOR_HPP__
#define __AMY_BASIC_RESULTS_ITERATOR_HPP__

#include <amy/detail/throw_error.hpp>

#include <amy/result_set.hpp>

#include <cassert>
#include <iterator>

namespace amy {

template<typename MySQLService>
class basic_results_iterator : public std::iterator<
    std::forward_iterator_tag,
    result_set,
    void,
    result_set*,
    result_set&
> {
private:
    typedef MySQLService service_type;

    typedef
        basic_results_iterator<service_type>
        this_type;

    typedef basic_connector<service_type> connector_type;

public:
    explicit basic_results_iterator() :
        connector_(nullptr),
        end_(true)
    {}

    explicit basic_results_iterator(connector_type& connector) :
        connector_(&connector),
        end_(false)
    {
        if (!connector_->is_open()) {
            AMY_SYSTEM_NS::error_code ec = amy::error::not_initialized;
            amy::detail::throw_error(ec, connector_->native());
        }

        increment();
    }

    this_type& operator++() {
        increment();
        return *this;
    }

    this_type operator++(int) {
        this_type last = *this;
        ++(*this);
        return last;
    }

    bool operator==(this_type const& other) const {
        return equal(other);
    }

    bool operator!=(this_type const& other) const {
        return !(*this == other);
    }

    reference operator*() const {
        return dereference();
    }

private:
    connector_type* connector_;
    mutable result_set result_set_;
    bool end_;

    void increment() {
        if (!connector_->has_more_results()) {
            end_ = true;
        } else {
            store_result();
        }
    }

    bool equal(this_type const& other) const {
        return (end_ == other.end_) && (end_ || connector_ == other.connector_);
    }

    reference dereference() const {
        assert(!end_);
        return result_set_;
    }

    void store_result() {
        assert(connector_);
        result_set_ = connector_->store_result();
    }

}; // class basic_results_iterator

} // namespace amy

#endif // __AMY_BASIC_RESULTS_ITERATOR_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
