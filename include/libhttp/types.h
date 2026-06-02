#ifndef LIBHTTP_TYPES_H
#define LIBHTTP_TYPES_H

#include <map>
#include <string>
#include <functional>
#include <cstdint>

namespace http
{
    // Common type aliases
    using Headers = std::map<std::string, std::string>;
    using Params = std::map<std::string, std::string>;
    using Cookies = std::map<std::string, std::string>;
    using Json = std::map<std::string, std::string>;
    using Files = std::map<std::string, std::string>;

    // Callbacks
    using DownloadProgressCallback = std::function<void(size_t current, size_t total)>;
    using Handler = std::function<void()>;

    // Proxy configuration
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

} // namespace http

#endif
