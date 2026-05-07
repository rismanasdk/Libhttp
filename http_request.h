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

        ProxyConfig() = default;

        explicit ProxyConfig(const std::string &proxy_url) : use_proxy(true)
        {
            parse_url(proxy_url);
        }

    private:
        void parse_url(const std::string &proxy_url)
        {
            std::string url = proxy_url;

            const std::size_t protocol_pos = url.find("://");
            if (protocol_pos != std::string::npos)
            {
                protocol = url.substr(0, protocol_pos);
                url = url.substr(protocol_pos + 3);
            }
            else
            {
                protocol = "http";
            }

            const std::size_t at_pos = url.find('@');
            if (at_pos != std::string::npos)
            {
                const std::string credentials = url.substr(0, at_pos);
                const std::size_t colon_pos = credentials.find(':');
                if (colon_pos != std::string::npos)
                {
                    username = credentials.substr(0, colon_pos);
                    password = credentials.substr(colon_pos + 1);
                }
                else
                {
                    username = credentials;
                }
                url = url.substr(at_pos + 1);
            }

            const std::size_t colon_pos = url.find(':');
            if (colon_pos != std::string::npos)
            {
                host = url.substr(0, colon_pos);
                port = url.substr(colon_pos + 1);
            }
            else
            {
                host = url;
                port = protocol == "https" ? "443" : (protocol.find("socks") == 0 ? "1080" : "8080");
            }
        }
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
