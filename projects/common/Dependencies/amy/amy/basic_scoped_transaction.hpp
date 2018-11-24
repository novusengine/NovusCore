#ifndef __AMY_TRANSACTION_HPP__
#define __AMY_TRANSACTION_HPP__

#include <amy/detail/noncopyable.hpp>

#include <amy/asio.hpp>
#include <amy/basic_connector.hpp>

namespace amy {

template<typename MySQLService>
class basic_scoped_transaction : private amy::detail::noncopyable {
public:
    typedef basic_connector<MySQLService> connector_type;

    basic_scoped_transaction(connector_type& connector) :
        connector_(connector),
        committed_(false)
    {
        connector_.autocommit(false);
    }

    void commit() {
        connector_.commit();
        committed_ = true;
    }

    AMY_SYSTEM_NS::error_code commit(AMY_SYSTEM_NS::error_code& ec) {
        committed_ = !!connector_.commit(ec);
        return ec;
    }

    void rollback() {
        connector_.rollback();
    }

    AMY_SYSTEM_NS::error_code rollback(AMY_SYSTEM_NS::error_code& ec) {
        return connector_.rollback(ec);
    }

    ~basic_scoped_transaction() {
        if (!committed_) {
            AMY_SYSTEM_NS::error_code ec;
            connector_.rollback(ec);
        }
        connector_.autocommit(true);
    }

private:
    connector_type& connector_;
    bool committed_;

}; // class basic_scoped_transaction

} // namespace amy

#endif // __AMY_TRANSACTION_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
