#ifndef LIBHTTP_UTILS_STATUS_H
#define LIBHTTP_UTILS_STATUS_H

#include <string>

namespace http
{
    namespace status
    {

        constexpr int CONTINUE = 100;
        constexpr int SWITCHING_PROTOCOLS = 101;
        constexpr int PROCESSING = 102;

        constexpr int OK = 200;
        constexpr int CREATED = 201;
        constexpr int ACCEPTED = 202;
        constexpr int NON_AUTHORITATIVE_INFO = 203;
        constexpr int NO_CONTENT = 204;
        constexpr int RESET_CONTENT = 205;
        constexpr int PARTIAL_CONTENT = 206;

        constexpr int MULTIPLE_CHOICES = 300;
        constexpr int MOVED_PERMANENTLY = 301;
        constexpr int FOUND = 302;
        constexpr int SEE_OTHER = 303;
        constexpr int NOT_MODIFIED = 304;
        constexpr int USE_PROXY = 305;
        constexpr int TEMPORARY_REDIRECT = 307;
        constexpr int PERMANENT_REDIRECT = 308;

        constexpr int BAD_REQUEST = 400;
        constexpr int UNAUTHORIZED = 401;
        constexpr int PAYMENT_REQUIRED = 402;
        constexpr int FORBIDDEN = 403;
        constexpr int NOT_FOUND = 404;
        constexpr int METHOD_NOT_ALLOWED = 405;
        constexpr int NOT_ACCEPTABLE = 406;
        constexpr int PROXY_AUTH_REQUIRED = 407;
        constexpr int REQUEST_TIMEOUT = 408;
        constexpr int CONFLICT = 409;
        constexpr int GONE = 410;
        constexpr int LENGTH_REQUIRED = 411;
        constexpr int PRECONDITION_FAILED = 412;
        constexpr int PAYLOAD_TOO_LARGE = 413;
        constexpr int URI_TOO_LONG = 414;
        constexpr int UNSUPPORTED_MEDIA_TYPE = 415;
        constexpr int RANGE_NOT_SATISFIABLE = 416;
        constexpr int EXPECTATION_FAILED = 417;
        constexpr int UNPROCESSABLE_ENTITY = 422;
        constexpr int TOO_MANY_REQUESTS = 429;

        constexpr int INTERNAL_SERVER_ERROR = 500;
        constexpr int NOT_IMPLEMENTED = 501;
        constexpr int BAD_GATEWAY = 502;
        constexpr int SERVICE_UNAVAILABLE = 503;
        constexpr int GATEWAY_TIMEOUT = 504;
        constexpr int HTTP_VERSION_NOT_SUPPORTED = 505;

        inline std::string reason_phrase(int status_code)
        {
            switch (status_code)
            {
            // 1xx
            case CONTINUE:
                return "Continue";
            case SWITCHING_PROTOCOLS:
                return "Switching Protocols";
            case PROCESSING:
                return "Processing";

            // 2xx
            case OK:
                return "OK";
            case CREATED:
                return "Created";
            case ACCEPTED:
                return "Accepted";
            case NON_AUTHORITATIVE_INFO:
                return "Non-Authoritative Information";
            case NO_CONTENT:
                return "No Content";
            case RESET_CONTENT:
                return "Reset Content";
            case PARTIAL_CONTENT:
                return "Partial Content";

            // 3xx
            case MULTIPLE_CHOICES:
                return "Multiple Choices";
            case MOVED_PERMANENTLY:
                return "Moved Permanently";
            case FOUND:
                return "Found";
            case SEE_OTHER:
                return "See Other";
            case NOT_MODIFIED:
                return "Not Modified";
            case USE_PROXY:
                return "Use Proxy";
            case TEMPORARY_REDIRECT:
                return "Temporary Redirect";
            case PERMANENT_REDIRECT:
                return "Permanent Redirect";

            // 4xx
            case BAD_REQUEST:
                return "Bad Request";
            case UNAUTHORIZED:
                return "Unauthorized";
            case PAYMENT_REQUIRED:
                return "Payment Required";
            case FORBIDDEN:
                return "Forbidden";
            case NOT_FOUND:
                return "Not Found";
            case METHOD_NOT_ALLOWED:
                return "Method Not Allowed";
            case NOT_ACCEPTABLE:
                return "Not Acceptable";
            case PROXY_AUTH_REQUIRED:
                return "Proxy Authentication Required";
            case REQUEST_TIMEOUT:
                return "Request Timeout";
            case CONFLICT:
                return "Conflict";
            case GONE:
                return "Gone";
            case LENGTH_REQUIRED:
                return "Length Required";
            case PRECONDITION_FAILED:
                return "Precondition Failed";
            case PAYLOAD_TOO_LARGE:
                return "Payload Too Large";
            case URI_TOO_LONG:
                return "URI Too Long";
            case UNSUPPORTED_MEDIA_TYPE:
                return "Unsupported Media Type";
            case RANGE_NOT_SATISFIABLE:
                return "Range Not Satisfiable";
            case EXPECTATION_FAILED:
                return "Expectation Failed";
            case UNPROCESSABLE_ENTITY:
                return "Unprocessable Entity";
            case TOO_MANY_REQUESTS:
                return "Too Many Requests";

            // 5xx
            case INTERNAL_SERVER_ERROR:
                return "Internal Server Error";
            case NOT_IMPLEMENTED:
                return "Not Implemented";
            case BAD_GATEWAY:
                return "Bad Gateway";
            case SERVICE_UNAVAILABLE:
                return "Service Unavailable";
            case GATEWAY_TIMEOUT:
                return "Gateway Timeout";
            case HTTP_VERSION_NOT_SUPPORTED:
                return "HTTP Version Not Supported";

            default:
                return "Unknown";
            }
        }

        inline bool is_informational(int status_code) { return status_code >= 100 && status_code < 200; }
        inline bool is_success(int status_code) { return status_code >= 200 && status_code < 300; }
        inline bool is_redirect(int status_code) { return status_code >= 300 && status_code < 400; }
        inline bool is_client_error(int status_code) { return status_code >= 400 && status_code < 500; }
        inline bool is_server_error(int status_code) { return status_code >= 500 && status_code < 600; }
        inline bool is_error(int status_code) { return status_code >= 400; }

    } // namespace status
} // namespace http

#endif
