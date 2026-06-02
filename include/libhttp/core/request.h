#ifndef LIBHTTP_CORE_REQUEST_H
#define LIBHTTP_CORE_REQUEST_H

#include "../types.h"

namespace http
{
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
