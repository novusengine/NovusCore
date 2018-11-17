#ifndef __AMY_BASIC_CONNECTOR_HPP__
#define __AMY_BASIC_CONNECTOR_HPP__

#include <amy/detail/throw_error.hpp>

#include <amy/asio.hpp>
#include <amy/auth_info.hpp>
#include <amy/client_flags.hpp>
#include <amy/result_set.hpp>

namespace amy {

/// Provides MySQL client functionalities.
template<typename MySQLService>
class basic_connector : public AMY_ASIO_NS::basic_io_object<MySQLService> {
public:
    /// The type of the service that provides actual MySQL client
    /// functionalities.
    typedef MySQLService service_type;

    /// The native MySQL connection handle type.
    typedef typename service_type::native_type native_type;

    /// Constructs a \c basic_connector without opening it.
    explicit basic_connector(AMY_ASIO_NS::io_service& io_service) :
        AMY_ASIO_NS::basic_io_object<MySQLService>(io_service)
    {}

    native_type native() {
        return this->get_service().native(this->get_implementation());
    }

    std::string error_message(AMY_SYSTEM_NS::error_code const& ec) {
        return this->get_service()
            .error_message(this->get_implementation(), ec);
    }

    void open() {
        AMY_SYSTEM_NS::error_code ec;
        detail::throw_error(open(ec), &(this->get_implementation().mysql));
    }

    AMY_SYSTEM_NS::error_code open(AMY_SYSTEM_NS::error_code& ec) {
        return this->get_service().open(this->get_implementation(), ec);
    }

    bool is_open() const {
        return this->get_service().is_open(this->get_implementation());
    }

    void close() {
        this->get_service().close(this->get_implementation());
    }

    template<typename Option>
    void set_option(Option const& option) {
        AMY_SYSTEM_NS::error_code ec;
        detail::throw_error(set_option(option, ec),
                            &(this->get_implementation().mysql));
    }

    template<typename Option>
    AMY_SYSTEM_NS::error_code set_option(Option const& option,
                                         AMY_SYSTEM_NS::error_code& ec)
    {
        return this->get_service()
            .set_option(this->get_implementation(), option, ec);
    }

    void cancel() {
        this->get_service().cancel(this->get_implementation());
    }

    template<typename Endpoint>
    void connect(Endpoint const& endpoint,
                 auth_info const& auth,
                 std::string const& database,
                 client_flags flags)
    {
        AMY_SYSTEM_NS::error_code ec;
        detail::throw_error(connect(endpoint, auth, database, flags, ec),
                            &(this->get_implementation().mysql));
    }

    template<typename Endpoint>
    AMY_SYSTEM_NS::error_code connect(Endpoint const& endpoint,
                                      auth_info const& auth,
                                      std::string const& database,
                                      client_flags flags,
                                      AMY_SYSTEM_NS::error_code& ec)
    {
        return this->get_service().connect(
                this->get_implementation(),
                endpoint, auth, database, flags, ec);
    }

    template<typename Endpoint, typename ConnectHandler>
    void async_connect(Endpoint const& endpoint,
                       auth_info const& auth,
                       std::string const& database,
                       client_flags flags,
                       ConnectHandler handler)
    {
        this->get_service().async_connect(
                this->get_implementation(),
                endpoint, auth, database, flags, handler);
    }

    void query(std::string const& stmt) {
        AMY_SYSTEM_NS::error_code ec;
        detail::throw_error(
                query(stmt, ec), &(this->get_implementation().mysql));
    }

    AMY_SYSTEM_NS::error_code query(std::string const& stmt,
                                    AMY_SYSTEM_NS::error_code& ec)
    {
        return this->get_service().query(this->get_implementation(), stmt, ec);
    }

    result_set query_result(std::string const& stmt) {
        query(stmt);
        return store_result();
    }

    result_set query_result(std::string const& stmt,
                            AMY_SYSTEM_NS::error_code& ec) {
        query(stmt, ec);
        if(ec)
          return amy::result_set::empty_set(
              &(this->get_implementation().mysql));
        return store_result(ec);
    }

    template<typename QueryHandler>
    void async_query(std::string const& stmt, QueryHandler handler) {
        this->get_service().async_query(
                this->get_implementation(), stmt, handler);
    }

    template<typename StoreResultHandler>
    void async_query_result(std::string const& stmt,
                            StoreResultHandler handler) {
      this->get_service().async_query_result(
          this->get_implementation(), stmt, handler);
    }

    bool has_more_results() const {
        return this->get_service().has_more_results(this->get_implementation());
    }

    result_set store_result() {
        AMY_SYSTEM_NS::error_code ec;
        result_set rs = store_result(ec);
        detail::throw_error(ec, &(this->get_implementation().mysql));
        return rs;
    }

    result_set store_result(AMY_SYSTEM_NS::error_code& ec) {
        return this->get_service().store_result(this->get_implementation(), ec);
    }

    template<typename StoreResultHandler>
    void async_store_result(StoreResultHandler handler) {
        this->get_service().async_store_result(
                this->get_implementation(), handler);
    }

    void autocommit(bool mode) {
        AMY_SYSTEM_NS::error_code ec;
        detail::throw_error(autocommit(mode, ec),
                            &(this->get_implementation().mysql));
    }

    AMY_SYSTEM_NS::error_code autocommit(bool mode,
                                         AMY_SYSTEM_NS::error_code& ec)
    {
        this->get_service().autocommit(this->get_implementation(), mode, ec);
        return ec;
    }

    void commit() {
        AMY_SYSTEM_NS::error_code ec;
        detail::throw_error(commit(ec), &(this->get_implementation().mysql));
    }

    AMY_SYSTEM_NS::error_code commit(AMY_SYSTEM_NS::error_code& ec) {
        this->get_service().commit(this->get_implementation(), ec);
        return ec;
    }

    void rollback() {
        AMY_SYSTEM_NS::error_code ec;
        detail::throw_error(rollback(ec), &(this->get_implementation().mysql));
    }

    AMY_SYSTEM_NS::error_code rollback(AMY_SYSTEM_NS::error_code& ec) {
        this->get_service().rollback(this->get_implementation(), ec);
        return ec;
    }

    uint64_t affected_rows() {
        return this->get_service().affected_rows(this->get_implementation());
    }

}; // class basic_connector

} // namespace amy

#endif // __AMY_BASIC_CONNECTOR_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
