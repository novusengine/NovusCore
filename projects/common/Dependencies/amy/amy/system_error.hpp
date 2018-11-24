#ifndef __AMY_SYSTEM_ERROR_HPP__
#define __AMY_SYSTEM_ERROR_HPP__

#include <amy/asio.hpp>

namespace amy {

class system_error : public AMY_SYSTEM_NS::system_error {
public:
    explicit system_error(AMY_SYSTEM_NS::error_code const& ec) :
        AMY_SYSTEM_NS::system_error(ec)
    {}

    explicit system_error(AMY_SYSTEM_NS::error_code const& ec,
                          std::string const& message) :
        AMY_SYSTEM_NS::system_error(ec),
        message_(message)
    {}

    virtual ~system_error() throw() {
    }

    virtual char const* what() const throw() {
        return message_.c_str();
    }

private:
    std::string message_;

}; // class system_error

} // namespace amy

#endif // __AMY_SYSTEM_ERROR_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
