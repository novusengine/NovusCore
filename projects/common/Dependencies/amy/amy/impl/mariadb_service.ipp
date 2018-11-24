#ifndef __AMY_IMPL_MARIADB_SERVICE_IPP__
#define __AMY_IMPL_MARIADB_SERVICE_IPP__

#include <amy/detail/mariadb_ops.hpp>

#include <amy/mariadb_options.hpp>
#include <amy/client_flags.hpp>
#include <amy/endpoint_traits.hpp>
#include <amy/noop_deleter.hpp>
#include <boost/beast/core/handler_ptr.hpp>
#include <boost/beast/core/bind_handler.hpp>

#include <functional>

namespace amy {

inline mariadb_service::mariadb_service(AMY_ASIO_NS::io_service& io_service)
    : detail::service_base<mariadb_service>(io_service) {}

inline mariadb_service::~mariadb_service() { shutdown_service(); }

inline void mariadb_service::shutdown_service() {}

inline void mariadb_service::construct(implementation_type& impl) {
  impl.ev_ =
      std::make_unique<AMY_ASIO_NS::posix::stream_descriptor>(get_io_service());
  impl.timer_ = std::make_unique<AMY_ASIO_NS::steady_timer>(get_io_service());
}

inline void mariadb_service::destroy(implementation_type& impl) { close(impl); }

inline mariadb_service::native_type mariadb_service::native(
    implementation_type& impl) {
  return &impl.mysql;
}

inline std::string mariadb_service::error_message(
    implementation_type& impl, AMY_SYSTEM_NS::error_code const& ec) {
  uint32_t ev = static_cast<uint32_t>(ec.value());

  if (::mysql_errno(native(impl)) == ev) {
    return ::mysql_error(native(impl));
  } else {
    return ec.message();
  }
}

inline AMY_SYSTEM_NS::error_code mariadb_service::open(
    implementation_type& impl, AMY_SYSTEM_NS::error_code& ec) {
  namespace ops = amy::detail::mysql_ops;

  ops::clear_error(ec);

  if (!is_open(impl)) {
    impl.initialized = !!ops::mysql_init(&impl.mysql, ec);
  }

  return ec;
}

inline bool mariadb_service::is_open(implementation_type const& impl) const {
  return impl.initialized;
}

inline void mariadb_service::close(implementation_type& impl) {
  if (is_open(impl)) {
    impl.close();
  }
}

template <typename Endpoint>
AMY_SYSTEM_NS::error_code mariadb_service::connect(implementation_type& impl,
    Endpoint const& endpoint, auth_info const& auth,
    std::string const& database, client_flags client_flag,
    AMY_SYSTEM_NS::error_code& ec) {
  if (!is_open(impl)) {
    if (open(impl, ec)) {
      return ec;
    }
  }

  amy::endpoint_traits<Endpoint> traits(endpoint);

  namespace ops = amy::detail::mysql_ops;

  ops::mysql_real_connect(&impl.mysql, traits.host(), auth.user(),
      auth.password(), database.c_str(), traits.port(), traits.unix_socket(),
      client_flag, ec);

  impl.flags = client_flag;

  return ec;
}

template <typename Endpoint, typename ConnectHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(ConnectHandler, void(AMY_SYSTEM_NS::error_code))
mariadb_service::async_connect(implementation_type& impl,
    Endpoint const& endpoint, auth_info const& auth,
    std::string const& database, client_flags flags, ConnectHandler handler) {
  if (!is_open(impl)) {
    AMY_SYSTEM_NS::error_code ec;
    if (!!open(impl, ec)) {
      AMY_ASIO_NS::post(this->get_io_service().get_executor(),
          boost::beast::bind_handler(handler, ec));
      return;
    }
  }

  AMY_SYSTEM_NS::error_code ec;
  set_option(impl, options::nonblock_default(), ec);
  if (ec) {
    AMY_ASIO_NS::post(this->get_io_service().get_executor(),
        boost::beast::bind_handler(handler, ec));
    return;
  }

  connect_handler<ConnectHandler, Endpoint>(this->get_io_service(), handler,
      impl, endpoint, auth, database, flags)(ec, 0);
}

inline AMY_SYSTEM_NS::error_code mariadb_service::query(
    implementation_type& impl, std::string const& stmt,
    AMY_SYSTEM_NS::error_code& ec) {
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

template <typename QueryHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(QueryHandler, void(AMY_SYSTEM_NS::error_code))
mariadb_service::async_query(
    implementation_type& impl, std::string const& stmt, QueryHandler handler) {
  if (!is_open(impl)) {
    AMY_ASIO_NS::post(this->get_io_service().get_executor(),
        boost::beast::bind_handler(handler, amy::error::not_initialized));
    return;
  }

  query_handler<QueryHandler>(this->get_io_service(), handler, impl, stmt)(
      {}, 0);
}

inline bool mariadb_service::has_more_results(
    implementation_type const& impl) const {
  namespace ops = amy::detail::mysql_ops;

  if (!is_open(impl)) {
    return false;
  }

  detail::mysql_handle m = const_cast<detail::mysql_handle>(&impl.mysql);

  bool multi_results =
      impl.flags & (amy::client_multi_results | amy::client_multi_statements);

  return multi_results ? !impl.first_result_stored || ops::mysql_more_results(m)
                       : !impl.first_result_stored && ops::mysql_field_count(m);
}

inline result_set mariadb_service::store_result(
    implementation_type& impl, AMY_SYSTEM_NS::error_code& ec) {
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
  impl.last_result.reset(
      ops::mysql_store_result(&impl.mysql, ec), result_set_deleter());

  result_set rs;
  rs.assign(&impl.mysql, impl.last_result, ec);

  return rs;
}

template <typename StoreResultHandler>
BOOST_ASIO_INITFN_RESULT_TYPE(
    StoreResultHandler, void(AMY_SYSTEM_NS::error_code, amy::result_set))
mariadb_service::async_store_result(
    implementation_type& impl, StoreResultHandler handler) {
  if (!is_open(impl)) {
    AMY_ASIO_NS::post(this->get_io_service().get_executor(),
        boost::beast::bind_handler(handler, amy::error::not_initialized,
            result_set::empty_set(&impl.mysql)));
    return;
  }

  store_result_handler<StoreResultHandler>(
      this->get_io_service(), handler, impl)({}, 0);
}

template <typename Handler>
BOOST_ASIO_INITFN_RESULT_TYPE(
    Handler, void(AMY_SYSTEM_NS::error_code, amy::result_set))
mariadb_service::async_query_result(
    implementation_type& impl, std::string const& stmt, Handler handler) {
  if (!is_open(impl)) {
    AMY_ASIO_NS::post(this->get_io_service().get_executor(),
        boost::beast::bind_handler(handler, amy::error::not_initialized,
            result_set::empty_set(&impl.mysql)));
    return;
  }

  query_result_handler<Handler>(this->get_io_service(), handler, impl, stmt)(
      {}, 0);
}

inline AMY_SYSTEM_NS::error_code mariadb_service::autocommit(
    implementation_type& impl, bool mode, AMY_SYSTEM_NS::error_code& ec) {
  namespace ops = amy::detail::mysql_ops;
  ops::mysql_autocommit(&impl.mysql, mode, ec);
  return ec;
}

inline AMY_SYSTEM_NS::error_code mariadb_service::commit(
    implementation_type& impl, AMY_SYSTEM_NS::error_code& ec) {
  namespace ops = amy::detail::mysql_ops;

  if (!is_open(impl)) {
    ec = amy::error::not_initialized;
    return ec;
  }

  ops::mysql_commit(&impl.mysql, ec);
  return ec;
}

inline AMY_SYSTEM_NS::error_code mariadb_service::rollback(
    implementation_type& impl, AMY_SYSTEM_NS::error_code& ec) {
  namespace ops = amy::detail::mysql_ops;

  if (!is_open(impl)) {
    ec = amy::error::not_initialized;
    return ec;
  }

  ops::mysql_rollback(&impl.mysql, ec);
  return ec;
}

inline uint64_t mariadb_service::affected_rows(implementation_type& impl) {
  return amy::detail::mysql_ops::mysql_affected_rows(&impl.mysql);
}

inline mariadb_service::implementation::implementation()
    : flags(amy::default_flags), initialized(false), first_result_stored(false),
      last_result(static_cast<detail::result_set_handle>(nullptr),
          result_set_deleter()),
      cancelation_token(static_cast<void*>(nullptr), noop_deleter()) {}

inline mariadb_service::implementation::~implementation() { close(); }

inline void mariadb_service::implementation::close() {
  if (this->initialized) {
    amy::detail::mysql_ops::mysql_close(&this->mysql);
    this->initialized = false;
  }

  ev_->release();
  if (timer_) timer_->cancel();

  this->first_result_stored = false;
  free_result();
  cancel();
}

template <typename Option>
AMY_SYSTEM_NS::error_code mariadb_service::set_option(implementation_type& impl,
    Option const& option, AMY_SYSTEM_NS::error_code& ec) {
  namespace ops = detail::mysql_ops;

  if (!is_open(impl)) {
    ec = amy::error::not_initialized;
    return ec;
  }

  ops::mysql_options(native(impl), option.option(), option.data(), ec);
  return ec;
}

inline void mariadb_service::cancel(implementation_type& impl) {
  impl.cancel();
}

inline void mariadb_service::implementation::free_result() {
  this->last_result.reset();
}

inline void mariadb_service::implementation::cancel() {
  this->cancelation_token.reset(static_cast<void*>(nullptr), noop_deleter());
}

namespace {
template <typename T1, typename T2>
void async_wait_mysql(int& status, T1& p, T2& self) {
  namespace ops = amy::detail::mysql_ops;

  auto& ev = *p.impl_.ev_;
  if (status & ops::wait_type::read_or_write) {
    int fd = ops::mysql_get_socket(&p.impl_.mysql);
    if (ev.native_handle() != fd) {
      ev.release();
      ev.assign(fd);
    }
  }

  if (p.impl_.timer_) p.impl_.timer_->cancel();
  if (ev.native_handle() != -1) ev.cancel();

  using AMY_ASIO_NS::posix::descriptor_base;
  using namespace std::placeholders;
  if (status & ops::wait_type::read) {
    ev.async_wait(descriptor_base::wait_read,
        boost::beast::bind_handler(std::move(self), _1, ops::wait_type::read));
    return;
  }
  if (status & ops::wait_type::write) {
    ev.async_wait(descriptor_base::wait_write,
        boost::beast::bind_handler(std::move(self), _1, ops::wait_type::write));
    return;
  }
  if (status & ops::wait_type::timeout) {
    auto& impl = p.impl_;
    if (!impl.timer_)
      impl.timer_ = std::make_unique<AMY_ASIO_NS::steady_timer>(p.ioc_);

    auto timeout = ops::mysql_get_timeout_value(&impl.mysql);

    auto& timer = *impl.timer_;
    timer.expires_after(timeout);
    timer.async_wait(boost::beast::bind_handler(
        std::move(self), _1, ops::wait_type::timeout));
    return;
  }
};
} // namespace

// This composed operation mysql_real_connect_[start|cont]
template <class Handler, class Endpoint>
class mariadb_service::connect_handler {
  // This holds all of the state information required by the operation.
  struct state {
    // The executor to execute mysql nonblocking api
    io_context& ioc_;

    // Boost.Asio and the Networking TS require an object of
    // type executor_work_guard<T>, where T is the type of
    // executor returned by the stream's get_executor function,
    // to persist for the duration of the asynchronous operation.
    boost::asio::executor_work_guard<decltype(
        std::declval<io_context&>().get_executor())>
        work;

    // Indicates what step in the operation's state machine
    // to perform next, starting from zero.
    int step = 0;

    implementation_type& impl_;
    std::weak_ptr<void> cancelation_token_{impl_.cancelation_token};

    Endpoint endpoint_;
    amy::auth_info auth_;
    std::string database_;
    client_flags flags_;
    detail::mysql_type* result_ = nullptr;

    // handler_ptr requires that the first parameter to the
    // contained object constructor is a reference to the
    // managed final completion handler.
    //
    explicit state(Handler const&, io_context& ioc, implementation_type& impl,
        Endpoint const& endpoint, amy::auth_info const& auth,
        std::string const& database, client_flags flags)
        : ioc_(ioc), work(ioc_.get_executor()), impl_(impl),
          endpoint_(endpoint), auth_(auth), database_(database), flags_(flags) {
    }
  };

  // The operation's data is kept in a cheap-to-copy smart
  // pointer container called `handler_ptr`. This efficiently
  // satisfies the CopyConstructible requirements of completion
  // handlers with expensive-to-copy state.
  //
  // `handler_ptr` uses the allocator associated with the final
  // completion handler, in order to allocate the storage for `state`.
  //
  boost::beast::handler_ptr<state, Handler> p_;

public:
  // Boost.Asio requires that handlers are CopyConstructible.
  // In some cases, it takes advantage of handlers that are
  // MoveConstructible. This operation supports both.
  //
  connect_handler(connect_handler&&)      = default;
  connect_handler(connect_handler const&) = default;

  // The constructor simply creates our state variables in
  // the smart pointer container.
  //
  template <class DeducedHandler>
  connect_handler(io_context& ioc, DeducedHandler&& handler,
      implementation_type& impl, Endpoint const& endpoint,
      amy::auth_info const& auth, std::string const& database,
      client_flags flags)
      : p_(std::forward<DeducedHandler>(handler), ioc, impl, endpoint, auth,
            database, flags) {}

  // Associated allocator support. This is Asio's system for
  // allowing the final completion handler to customize the
  // memory allocation strategy used for composed operation
  // states. A composed operation should use the same allocator
  // as the final handler. These declarations achieve that.

  using allocator_type = boost::asio::associated_allocator_t<Handler>;

  allocator_type get_allocator() const noexcept {
    return (boost::asio::get_associated_allocator)(p_.handler());
  }

  // io_context hook. This is Asio's system for customizing the
  // manner in which asynchronous completion handlers are invoked.
  // A composed operation needs to use the same executor to invoke
  // intermediate completion handlers as that used to invoke the
  // final handler.

  using executor_type = boost::asio::associated_executor_t<Handler,
      decltype(std::declval<io_context&>().get_executor())>;

  executor_type get_executor() const noexcept {
    return (boost::asio::get_associated_executor)(p_.handler(), p_->ioc_);
  }

  // The entry point for this handler. This will get called
  // as our intermediate operations complete. Definition below.
  //
  void operator()(boost::beast::error_code ec, int status) {
    // Store a reference to our state. The address of the state won't
    // change, and this solves the problem where dereferencing the
    // data member is undefined after a move.
    auto& p = *p_;

    using namespace amy::error;
    namespace ops = amy::detail::mysql_ops;
    if (p.cancelation_token_.expired())
      ec = AMY_ASIO_NS::error::operation_aborted;

    // Now perform the next step in the state machine
    switch (ec ? 2 : p.step) {
    // initial entry
    case 0: {

      amy::endpoint_traits<Endpoint> traits(p.endpoint_);

      status = ops::mysql_real_connect_start(&p.result_, &p.impl_.mysql,
          traits.host(), p.auth_.user(), p.auth_.password(),
          p.database_.c_str(), traits.port(), traits.unix_socket(), p.flags_,
          ec);

      p.impl_.flags = p.flags_;
    } /* FALLTHRU */
    case 1: {
      if (p.step == 1)
        status = ops::mysql_real_connect_cont(
            &p.result_, &p.impl_.mysql, status, ec);
      p.step = 1;

      if (status == ops::wait_type::finish || ec) break;

      async_wait_mysql(status, p, *this);
      return;
    }

    case 2: break;
    }
    // Invoke the final handler. The implementation of `handler_ptr`
    // will deallocate the storage for the state before the handler
    // is invoked. This is necessary to provide the
    // destroy-before-invocation guarantee on handler memory
    // customizations.
    //
    // If we wanted to pass any arguments to the handler which come
    // from the `state`, they would have to be moved to the stack
    // first or else undefined behavior results.
    //
    // The work guard is moved to the stack first, otherwise it would
    // be destroyed before the handler is invoked.
    //
    auto work = std::move(p.work);
    p_.invoke(ec);
    return;
  }
};

// This composed operation mysql_real_query_[start|cont]
template <class Handler>
class mariadb_service::query_handler {
  struct state {
    io_context& ioc_;

    boost::asio::executor_work_guard<decltype(
        std::declval<io_context&>().get_executor())>
        work;

    int step = 0;

    implementation_type& impl_;
    std::weak_ptr<void> cancelation_token_{impl_.cancelation_token};

    std::string stmt_;
    int result_ = -1;

    explicit state(Handler const&, io_context& ioc, implementation_type& impl,
        std::string const& stmt)
        : ioc_(ioc), work(ioc_.get_executor()), impl_(impl), stmt_(stmt) {}
  };

  boost::beast::handler_ptr<state, Handler> p_;

public:
  query_handler(query_handler&&)      = default;
  query_handler(query_handler const&) = default;

  template <class DeducedHandler>
  query_handler(io_context& ioc, DeducedHandler&& handler,
      implementation_type& impl, std::string const& stmt)
      : p_(std::forward<DeducedHandler>(handler), ioc, impl, stmt) {}

  using allocator_type = boost::asio::associated_allocator_t<Handler>;

  allocator_type get_allocator() const noexcept {
    return (boost::asio::get_associated_allocator)(p_.handler());
  }

  using executor_type = boost::asio::associated_executor_t<Handler,
      decltype(std::declval<io_context&>().get_executor())>;

  executor_type get_executor() const noexcept {
    return (boost::asio::get_associated_executor)(p_.handler(), p_->ioc_);
  }

  void operator()(boost::beast::error_code ec, int status) {
    auto& p = *p_;

    using namespace amy::error;
    namespace ops = amy::detail::mysql_ops;
    if (p.cancelation_token_.expired())
      ec = AMY_ASIO_NS::error::operation_aborted;

    switch (ec ? 2 : p.step) {
    case 0: {
      p.impl_.free_result();
      p.impl_.first_result_stored = false;

      status = ops::mysql_real_query_start(
          &p.result_, &p.impl_.mysql, p.stmt_.c_str(), p.stmt_.size(), ec);

    } /* FALLTHRU */
    case 1: {
      if (p.step == 1)
        status =
            ops::mysql_real_query_cont(&p.result_, &p.impl_.mysql, status, ec);

      p.step = 1;

      if (status == ops::wait_type::finish || ec) break;

      async_wait_mysql(status, p, *this);
      return;
    }

    case 2: break;
    }
    auto work = std::move(p.work);
    p_.invoke(ec);
    return;
  }
};

// This composed operation mysql_[store|next]_result_[start|cont]
template <class Handler>
class mariadb_service::store_result_handler {
  struct state {
    io_context& ioc_;

    boost::asio::executor_work_guard<decltype(
        std::declval<io_context&>().get_executor())>
        work;

    int step = 0;

    implementation_type& impl_;
    std::weak_ptr<void> cancelation_token_{impl_.cancelation_token};

    int next_result_                 = -1;
    detail::result_set_type* result_ = nullptr;

    explicit state(Handler const&, io_context& ioc, implementation_type& impl)
        : ioc_(ioc), work(ioc_.get_executor()), impl_(impl) {}
  };

  boost::beast::handler_ptr<state, Handler> p_;

public:
  store_result_handler(store_result_handler&&)      = default;
  store_result_handler(store_result_handler const&) = default;

  template <class DeducedHandler>
  store_result_handler(
      io_context& ioc, DeducedHandler&& handler, implementation_type& impl)
      : p_(std::forward<DeducedHandler>(handler), ioc, impl) {}

  using allocator_type = boost::asio::associated_allocator_t<Handler>;

  allocator_type get_allocator() const noexcept {
    return (boost::asio::get_associated_allocator)(p_.handler());
  }

  using executor_type = boost::asio::associated_executor_t<Handler,
      decltype(std::declval<io_context&>().get_executor())>;

  executor_type get_executor() const noexcept {
    return (boost::asio::get_associated_executor)(p_.handler(), p_->ioc_);
  }

  void operator()(boost::beast::error_code ec, int status) {
    auto& p = *p_;
    auto rs = result_set::empty_set(&p.impl_.mysql);

    using namespace amy::error;
    namespace ops = amy::detail::mysql_ops;

    for (;;) {
      if (p.cancelation_token_.expired())
        ec = AMY_ASIO_NS::error::operation_aborted;

      enum {
        S_ENTRY = 0,
        S_CONT_STORE,
        S_ERROR,
        S_STORE_START,
        S_CONT_NEXT,
        S_WAIT_NEXT,
      };
      switch (ec ? S_ERROR : p.step) {
      case S_ENTRY: {

        if (p.impl_.first_result_stored) {
          // Frees the last result set.
          p.impl_.free_result();

          mariadb_service& service =
              AMY_ASIO_NS::use_service<mariadb_service>(p.ioc_);

          if (!service.has_more_results(p.impl_)) {
            ec = amy::error::no_more_results;
            break;
          } else {
            status = ops::mysql_next_result_start(
                &p.next_result_, &p.impl_.mysql, ec);
            if (ec) break;
            if (status != ops::wait_type::finish) {
              p.step = S_WAIT_NEXT;
              continue; // goto mysql_next_result_cont
            }
          }
        } else {
          p.impl_.first_result_stored = true;
        }
      } /* FALLTHRU */
      case S_STORE_START: {
        p.step = S_STORE_START;
        p.impl_.free_result();

        status = ops::mysql_store_result_start(&p.result_, &p.impl_.mysql, ec);

        if (ec || status == ops::wait_type::finish) break;
      } /* FALLTHRU */
      case S_WAIT_NEXT:
      case S_CONT_NEXT:
      case S_CONT_STORE: {
        if (p.step == S_STORE_START)
          p.step = S_CONT_STORE;
        else if (p.step == S_CONT_STORE)
          status = ops::mysql_store_result_cont(
              &p.result_, &p.impl_.mysql, status, ec);
        else if (p.step == S_CONT_NEXT)
          status = ops::mysql_next_result_cont(
              &p.next_result_, &p.impl_.mysql, status, ec);
        else if (p.step == S_WAIT_NEXT)
          p.step = S_CONT_NEXT;

        if (ec) break;

        if (status == ops::wait_type::finish) {
          if (p.step == S_CONT_NEXT) {
            p.step = S_STORE_START;
            continue; // goto mysql_store_result_start
          }
          BOOST_ASSERT(p.step == S_CONT_STORE);
          break;
        }

        async_wait_mysql(status, p, *this);
        return;
      }

      case S_ERROR: break;
      }

      auto work = std::move(p.work);

      result_set rs;
      if (!ec) {
        // Retrieves the next result set.
        p.impl_.last_result.reset(p.result_, result_set_deleter());

        rs.assign(&p.impl_.mysql, p.impl_.last_result, ec);
      } else {
        // If anything went wrong, invokes the user-defined handler with the
        // error code and an empty result set.
        rs = result_set::empty_set(&p.impl_.mysql);
      }
      p_.invoke(ec, rs);
      return;
    } // for(;;)
  }
};

// This composed operation mysql_[real_query|store_result]_[start|cont]
template <class Handler>
class mariadb_service::query_result_handler {
  struct state {
    io_context& ioc_;

    boost::asio::executor_work_guard<decltype(
        std::declval<io_context&>().get_executor())>
        work;

    int step = 0;

    implementation_type& impl_;
    std::weak_ptr<void> cancelation_token_{impl_.cancelation_token};

    std::string stmt_;
    int query_result_                = -1;
    detail::result_set_type* result_ = nullptr;

    explicit state(Handler const&, io_context& ioc, implementation_type& impl,
        std::string const& stmt)
        : ioc_(ioc), work(ioc_.get_executor()), impl_(impl), stmt_(stmt) {}
  };

  boost::beast::handler_ptr<state, Handler> p_;

public:
  query_result_handler(query_result_handler&&)      = default;
  query_result_handler(query_result_handler const&) = default;

  template <class DeducedHandler>
  query_result_handler(io_context& ioc, DeducedHandler&& handler,
      implementation_type& impl, std::string const& stmt)
      : p_(std::forward<DeducedHandler>(handler), ioc, impl, stmt) {}

  using allocator_type = boost::asio::associated_allocator_t<Handler>;

  allocator_type get_allocator() const noexcept {
    return (boost::asio::get_associated_allocator)(p_.handler());
  }

  using executor_type = boost::asio::associated_executor_t<Handler,
      decltype(std::declval<io_context&>().get_executor())>;

  executor_type get_executor() const noexcept {
    return (boost::asio::get_associated_executor)(p_.handler(), p_->ioc_);
  }

  void operator()(boost::beast::error_code ec, int status) {
    auto& p = *p_;
    auto rs = result_set::empty_set(&p.impl_.mysql);

    using namespace amy::error;
    namespace ops = amy::detail::mysql_ops;

    for (;;) {
      if (p.cancelation_token_.expired())
        ec = AMY_ASIO_NS::error::operation_aborted;
      enum {
        S_ENTRY = 0,
        S_ERROR,
        S_STORE_START,
        S_CONT_STORE,
        S_WAIT_QUERY,
        S_CONT_QUERY,
      };
      switch (ec ? S_ERROR : p.step) {
      case S_ENTRY: {
        p.impl_.free_result();
        p.impl_.first_result_stored = false;

        status = ops::mysql_real_query_start(&p.query_result_, &p.impl_.mysql,
            p.stmt_.c_str(), p.stmt_.size(), ec);
        if (ec) break;
        if (status != ops::wait_type::finish) {
          p.step = S_WAIT_QUERY;
          continue;
        }
        p.step = S_STORE_START;
      } /* FALLTHRU */
      case S_STORE_START: {
        p.impl_.free_result();
        p.impl_.first_result_stored = true;

        status = ops::mysql_store_result_start(&p.result_, &p.impl_.mysql, ec);

        if (ec || status == ops::wait_type::finish) break;
      } /* FALLTHRU */
      case S_WAIT_QUERY:
      case S_CONT_QUERY:
      case S_CONT_STORE: {
        if (p.step == S_WAIT_QUERY)
          p.step = S_CONT_QUERY;
        else if (p.step == S_CONT_QUERY)
          status = ops::mysql_real_query_cont(
              &p.query_result_, &p.impl_.mysql, status, ec);
        else if (p.step == S_STORE_START)
          p.step = S_CONT_STORE;
        else if (p.step == S_CONT_STORE)
          status = ops::mysql_store_result_cont(
              &p.result_, &p.impl_.mysql, status, ec);

        if (ec) break;

        if (status == ops::wait_type::finish) {
          if (p.step == S_CONT_QUERY) {
            p.step = S_STORE_START;
            continue; // goto mysql_store_result_start
          }
          BOOST_ASSERT(p.step == S_CONT_STORE);
          break;
        }

        async_wait_mysql(status, p, *this);
        return;
      }

      case S_ERROR: break;
      }

      auto work = std::move(p.work);

      result_set rs;
      if (!ec) {
        // Retrieves the next result set.
        p.impl_.last_result.reset(p.result_, result_set_deleter());

        rs.assign(&p.impl_.mysql, p.impl_.last_result, ec);
      } else {
        // If anything went wrong, invokes the user-defined handler with the
        // error code and an empty result set.
        rs = result_set::empty_set(&p.impl_.mysql);
      }
      p_.invoke(ec, rs);
      return;
    } // for(;;)
  }
};

inline void mariadb_service::result_set_deleter::operator()(void* p) {
  namespace ops = detail::mysql_ops;

  if (!!p) {
    ops::mysql_free_result(static_cast<detail::result_set_handle>(p));
  }
}

} // namespace amy

#endif // __AMY_IMPL_MARIADB_SERVICE_IPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
