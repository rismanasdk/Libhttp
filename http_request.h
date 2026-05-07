#ifndef LIBHTTP_REQUEST_H
#define LIBHTTP_REQUEST_H

#include <map>
#include <string>
#include <functional>

namespace http
{

    using Headers = std::map<std::string, std::string>;
    using Params = std::map<std::string, std::string>;
    using Cookies = std::map<std::string, std::string>;
    using Json = std::map<std::string, std::string>;
    using Files = std::map<std::string, std::string>;

    struct ProxyConfig
    {
        std::string host;
        std::string port;
        std::string protocol;
        std::string username;
        std::string password;
        bool use_proxy = false;
    };

    using DownloadProgressCallback = std::function<void(size_t current, size_t total)>;

    struct RequestOptions
    {
        Headers headers;
        Params params;
        Params data;
        Json json;
        Files files;
        Cookies cookies;
        std::string body;
        std::string auth_username;
        std::string auth_password;
        std::string auth_type;  // "basic", "bearer", "digest", "custom"
        std::string auth_token; // for bearer and custom auth
        ProxyConfig proxy;
        int timeout_seconds = 10;
        bool allow_redirects = true;
        int max_redirects = 5;
        int retry_count = 0;
        int retry_delay_ms = 0;
        bool stream = false;
        size_t chunk_size = 8192;
        DownloadProgressCallback progress_callback;
        bool verify_ssl = true;
        std::string ssl_cert_path;
        std::string ssl_key_path;
    };

} // namespace http

#endif
