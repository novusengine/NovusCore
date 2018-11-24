#ifndef __AMY_IMPL_MYSQL_SERVICE_IPP__
#define __AMY_IMPL_MYSQL_SERVICE_IPP__

#include <amy/detail/mysql_ops.hpp>

#include <amy/client_flags.hpp>
#include <amy/endpoint_traits.hpp>
#include <amy/noop_deleter.hpp>

#include <functional>

namespace amy {

inline mysql_service::mysql_service(AMY_ASIO_NS::io_service& io_service) :
    detail::service_base<mysql_service>(io_service),
    work_mutex_(),
    work_io_service_(new AMY_ASIO_NS::io_service),
    work_(new AMY_ASIO_NS::io_service::work(*work_io_service_)),
    work_thread_()
{}

inline mysql_service::~mysql_service() {
    shutdown_service();
}

inline void mysql_service::shutdown_service() {
    work_.reset();

    if (!!work_io_service_) {
        work_io_service_->stop();

        if (!!work_thread_) {
            work_thread_->join();
            work_thread_.reset();
        }

        work_io_service_.reset();
    }
}

inline void mysql_service::construct(implementation_type&) {
}

inline void mysql_service::destroy(implementation_type& impl) {
    close(impl);
}

inline mysql_service::native_type
mysql_service::native(implementation_type& impl) {
    return &impl.mysql;
}

inline std::string
mysql_service::error_message(implementation_type& impl,
                             AMY_SYSTEM_NS::error_code const& ec)
{
    uint32_t ev = static_cast<uint32_t>(ec.value());

    if (::mysql_errno(native(impl)) == ev) {
        return ::mysql_error(native(impl));
    } else {
        return ec.message();
    }
}

inline AMY_SYSTEM_NS::error_code
mysql_service::open(implementation_type& impl, AMY_SYSTEM_NS::error_code& ec) {
    namespace ops = amy::detail::mysql_ops;

    ops::clear_error(ec);

    if (!is_open(impl)) {
        impl.initialized = !!ops::mysql_init(&impl.mysql, ec);
    }

    return ec;
}

inline bool mysql_service::is_open(implementation_type const& impl) const {
    return impl.initialized;
}

inline void mysql_service::close(implementation_type& impl) {
    if (is_open(impl)) {
        impl.close();
    }
}

inline void mysql_service::start_work_thread() {
    std::lock_guard<std::mutex> lock(work_mutex_);

    typedef size_t(AMY_ASIO_NS::io_service::*run_function)();

    if (!work_thread_) {
        work_thread_.reset(
                new std::thread(
                    std::bind<run_function>(
                        &AMY_ASIO_NS::io_service::run,
                        work_io_service_.get())));
    }
}

template<typename Endpoint>
AMY_SYSTEM_NS::error_code
mysql_service::connect(implementation_type& impl,
                       Endpoint const& endpoint,
                       auth_info const& auth,
                       std::string const& database,
                       client_flags client_flag,
                       AMY_SYSTEM_NS::error_code& ec)
{
    if (!is_open(impl)) {
        if (open(impl, ec)) {
            return ec;
        }
    }

    amy::endpoint_traits<Endpoint> traits(endpoint);

    namespace ops = amy::detail::mysql_ops;

    ops::mysql_real_connect(&impl.mysql, traits.host(), auth.user(),
                            auth.password(), database.c_str(), traits.port(),
                            traits.unix_socket(), client_flag, ec);

    impl.flags = client_flag;

    return ec;
}

template<typename Endpoint, typename ConnectHandler>
void mysql_service::async_connect(implementation_type& impl,
                                  Endpoint const& endpoint,
                                  auth_info const& auth,
                                  std::string const& database,
                                  client_flags flags,
                                  ConnectHandler handler)
{
    if (!is_open(impl)) {
        AMY_SYSTEM_NS::error_code ec;
        if (!!open(impl, ec)) {
            this->get_io_service().post(std::bind(handler, ec));
            return;
        }
    }

    if (!!work_io_service_) {
        start_work_thread();
        work_io_service_->post(
                connect_handler<Endpoint, ConnectHandler>(
                    impl, endpoint, auth, database, flags,
                    this->get_io_service(), handler));
    }
}

inline AMY_SYSTEM_NS::error_code
mysql_service::query(implementation_type& impl,
                     std::string const& stmt,
                     AMY_SYSTEM_NS::error_code& ec)
{
    if (!is_open(impl)) {
        ec = amy::error::not_initialized;
        return ec;
    }

    impl.free_result();
    impl.first_result_stored = false;

    namespace ops = detail::mysql_ops;

    ops::mysql_real_query(&impl.mysql, stmt.c_str(), stmt.length(), ec);
    return ec;
}

template<typename QueryHandler>
void mysql_service::async_query(implementation_type& impl,
                                std::string const& stmt,
                                QueryHandler handler)
{
    if (!is_open(impl)) {
        this->get_io_service().post(
                std::bind(handler, amy::error::not_initialized));
    } else {
        if (!!work_io_service_) {
            start_work_thread();
            work_io_service_->post(query_handler<QueryHandler>(
                        impl, stmt, this->get_io_service(), handler));
        }
    }
}

inline bool
mysql_service::has_more_results(implementation_type const& impl) const {
    namespace ops = amy::detail::mysql_ops;

    if (!is_open(impl)) {
        return false;
    }

    detail::mysql_handle m = const_cast<detail::mysql_handle>(&impl.mysql);

    bool multi_results = impl.flags & (amy::client_multi_results |
                                       amy::client_multi_statements);

    return multi_results ?
        !impl.first_result_stored || ops::mysql_more_results(m) :
        !impl.first_result_stored && ops::mysql_field_count(m);
}

inline result_set mysql_service::store_result(implementation_type& impl,
                                              AMY_SYSTEM_NS::error_code& ec)
{
    namespace ops = amy::detail::mysql_ops;

    if (impl.first_result_stored) {
        // Frees the last result set.
        impl.free_result();

        if (!has_more_results(impl)) {
            ec = amy::error::no_more_results;
        } else {
            ops::mysql_next_result(&impl.mysql, ec);
        }
    } else {
        impl.first_result_stored = true;
    }

    if (ec) {
        // If anything went wrong, returns an empty result set.
        return result_set::empty_set(&impl.mysql);
    }

    // Retrieves the next result set.
    impl.last_result.reset(ops::mysql_store_result(&impl.mysql, ec),
                           result_set_deleter());

    result_set rs;
    rs.assign(&impl.mysql, impl.last_result, ec);

    return rs;
}

template<typename StoreResultHandler>
void mysql_service::async_store_result(implementation_type& impl,
                                       StoreResultHandler handler)
{
    if (!is_open(impl)) {
        this->get_io_service().post(
                std::bind(handler,
                          amy::error::not_initialized,
                          result_set::empty_set(&impl.mysql)));
    } else {
        if (!!work_io_service_) {
            start_work_thread();
            work_io_service_->post(
                    store_result_handler<StoreResultHandler>(
                        impl, this->get_io_service(), handler));
        }
    }
}

inline AMY_SYSTEM_NS::error_code
mysql_service::autocommit(implementation_type& impl,
                          bool mode,
                          AMY_SYSTEM_NS::error_code& ec)
{
    namespace ops = amy::detail::mysql_ops;
    ops::mysql_autocommit(&impl.mysql, mode, ec);
    return ec;
}

inline AMY_SYSTEM_NS::error_code
mysql_service::commit(implementation_type& impl,
                      AMY_SYSTEM_NS::error_code& ec)
{
    namespace ops = amy::detail::mysql_ops;

    if (!is_open(impl)) {
        ec = amy::error::not_initialized;
        return ec;
    }

    ops::mysql_commit(&impl.mysql, ec);
    return ec;
}

inline AMY_SYSTEM_NS::error_code
mysql_service::rollback(implementation_type& impl,
                        AMY_SYSTEM_NS::error_code& ec)
{
    namespace ops = amy::detail::mysql_ops;

    if (!is_open(impl)) {
        ec = amy::error::not_initialized;
        return ec;
    }

    ops::mysql_rollback(&impl.mysql, ec);
    return ec;
}

inline uint64_t mysql_service::affected_rows(implementation_type& impl) {
    return amy::detail::mysql_ops::mysql_affected_rows(&impl.mysql);
}

inline mysql_service::implementation::implementation() :
    flags(amy::default_flags),
    initialized(false),
    first_result_stored(false),
    last_result(static_cast<detail::result_set_handle>(nullptr),
                result_set_deleter()),
    cancelation_token(static_cast<void*>(nullptr), noop_deleter())
{}

inline mysql_service::implementation::~implementation() {
    close();
}

inline void mysql_service::implementation::close() {
    if (this->initialized) {
        amy::detail::mysql_ops::mysql_close(&this->mysql);
        this->initialized = false;
    }

    this->first_result_stored = false;
    free_result();
    cancel();
}

template<typename Option>
AMY_SYSTEM_NS::error_code
mysql_service::set_option(implementation_type& impl,
                          Option const& option,
                          AMY_SYSTEM_NS::error_code& ec)
{
    namespace ops = detail::mysql_ops;

    if (!is_open(impl)) {
        ec = amy::error::not_initialized;
    }

    ops::mysql_options(native(impl), option.option(), option.data(), ec);
    return ec;
}

inline void mysql_service::cancel(implementation_type& impl) {
    impl.cancel();
}

inline void mysql_service::implementation::free_result() {
    this->last_result.reset();
}

inline void mysql_service::implementation::cancel() {
    this->cancelation_token.reset(static_cast<void*>(nullptr), noop_deleter());
}

template<typename Handler>
mysql_service::handler_base<Handler>::handler_base(
        implementation_type& impl,
        AMY_ASIO_NS::io_service& io_service,
        Handler handler)
  : impl_(impl),
    cancelation_token_(impl.cancelation_token),
    io_service_(io_service),
    work_(io_service),
    handler_(handler)
{}

template<typename Endpoint, typename ConnectHandler>
mysql_service::connect_handler<Endpoint, ConnectHandler>::connect_handler(
        implementation_type& impl,
        Endpoint const& endpoint,
        amy::auth_info const& auth,
        std::string const& database,
        client_flags flags,
        AMY_ASIO_NS::io_service& io_service,
        ConnectHandler handler)
  : handler_base<ConnectHandler>(impl, io_service, handler),
    endpoint_(endpoint),
    auth_(auth),
    database_(database),
    flags_(flags)
{}

template<typename Endpoint, typename ConnectHandler>
void mysql_service::connect_handler<Endpoint, ConnectHandler>::operator()() {
    using namespace amy::error;
    namespace ops = amy::detail::mysql_ops;

    if (this->cancelation_token_.expired()) {
        this->io_service_.post(
                std::bind(this->handler_,
                          AMY_ASIO_NS::error::operation_aborted));
        return;
    }

    amy::endpoint_traits<Endpoint> traits(this->endpoint_);

    AMY_SYSTEM_NS::error_code ec;
    ops::mysql_real_connect(&this->impl_.mysql,
                            traits.host(),
                            auth_.user(),
                            auth_.password(),
                            database_.c_str(),
                            traits.port(),
                            traits.unix_socket(),
                            flags_,
                            ec);

    this->impl_.flags = flags_;
    this->io_service_.post(std::bind(this->handler_, ec));
}

template<typename QueryHandler>
mysql_service::query_handler<QueryHandler>::query_handler(
        implementation_type& impl,
        std::string const& stmt,
        AMY_ASIO_NS::io_service& io_service,
        QueryHandler handler)
  : handler_base<QueryHandler>(impl, io_service, handler),
    stmt_(stmt)
{}

template<typename QueryHandler>
void mysql_service::query_handler<QueryHandler>::operator()() {
    using namespace amy::error;
    namespace ops = amy::detail::mysql_ops;

    if (this->cancelation_token_.expired()) {
        this->io_service_.post(
                std::bind(this->handler_,
                          AMY_ASIO_NS::error::operation_aborted));
        return;
    }

    this->impl_.free_result();
    this->impl_.first_result_stored = false;

    AMY_SYSTEM_NS::error_code ec;
    ops::mysql_real_query(&this->impl_.mysql,
                          stmt_.c_str(),
                          stmt_.length(),
                          ec);

    this->io_service_.post(std::bind(this->handler_, ec));
}

template<typename StoreResultHandler>
mysql_service::store_result_handler<StoreResultHandler>::store_result_handler(
        implementation_type& impl,
        AMY_ASIO_NS::io_service& io_service,
        StoreResultHandler handler)
  : handler_base<StoreResultHandler>(impl, io_service, handler)
{}

template<typename StoreResultHandler>
void mysql_service::store_result_handler<StoreResultHandler>::operator()() {
    namespace ops = amy::detail::mysql_ops;

    if (this->cancelation_token_.expired()) {
        this->io_service_.post(
                std::bind(this->handler_,
                          AMY_ASIO_NS::error::operation_aborted,
                          result_set::empty_set(&this->impl_.mysql)));
        return;
    }

    AMY_SYSTEM_NS::error_code ec;

    if (this->impl_.first_result_stored) {
        // Frees the last result set.
        this->impl_.free_result();

        mysql_service& service =
            AMY_ASIO_NS::use_service<mysql_service>(this->io_service_);

        if (!service.has_more_results(this->impl_)) {
            ec = amy::error::no_more_results;
        } else {
            ops::mysql_next_result(&this->impl_.mysql, ec);
        }
    } else {
        this->impl_.first_result_stored = true;
    }

    if (ec) {
        // If anything went wrong, invokes the user-defined handler with the
        // error code and an empty result set.
        this->io_service_.post(
                std::bind(this->handler_,
                          ec,
                          result_set::empty_set(&this->impl_.mysql)));
        return;
    }

    // Retrieves the next result set.
    this->impl_.last_result.reset(
            ops::mysql_store_result(&this->impl_.mysql, ec),
            result_set_deleter());

    result_set rs;
    rs.assign(&this->impl_.mysql, this->impl_.last_result, ec);

    this->io_service_.post(std::bind(this->handler_, ec, rs));
}

inline void mysql_service::result_set_deleter::operator()(void* p) {
    namespace ops = detail::mysql_ops;

    if (!!p) {
        ops::mysql_free_result(static_cast<detail::result_set_handle>(p));
    }
}

} // namespace amy

#endif // __AMY_IMPL_MYSQL_SERVICE_IPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
