#ifndef LIBHTTP_EXCEPTION_H
#define LIBHTTP_EXCEPTION_H

#include <stdexcept>
#include <string>

namespace http
{

    class HttpException : public std::runtime_error
    {
    public:
        explicit HttpException(const std::string &message) : std::runtime_error(message) {}
    };

    class ConnectionException : public HttpException
    {
    public:
        explicit ConnectionException(const std::string &message) : HttpException("Connection error: " + message) {}
    };

    class TimeoutException : public HttpException
    {
    public:
        explicit TimeoutException(const std::string &message) : HttpException("Timeout error: " + message) {}
    };

    class StatusException : public HttpException
    {
    private:
        int status_code_;
        std::string reason_;

    public:
        StatusException(int status_code, const std::string &reason)
            : HttpException("HTTP " + std::to_string(status_code) + " " + reason),
              status_code_(status_code),
              reason_(reason) {}

        int status_code() const { return status_code_; }
        const std::string &reason() const { return reason_; }
    };

    class BadStatusException : public StatusException
    {
    public:
        BadStatusException(int status_code, const std::string &reason) : StatusException(status_code, reason) {}
    };

    class RedirectException : public HttpException
    {
    private:
        std::string location_;

    public:
        RedirectException(const std::string &location) : HttpException("Redirect to: " + location), location_(location) {}

        const std::string &location() const { return location_; }
    };

} // namespace http

#endif
