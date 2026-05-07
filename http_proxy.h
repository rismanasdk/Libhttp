#ifndef LIBHTTP_PROXY_H
#define LIBHTTP_PROXY_H

#include <string>

namespace http
{

    struct ProxyConfig
    {
        std::string host;
        std::string port;
        std::string protocol; // http, https, socks5, socks4
        std::string username;
        std::string password;
        bool use_proxy = false;

        ProxyConfig() = default;

        ProxyConfig(const std::string &proxy_url) : use_proxy(true)
        {
            parse(proxy_url);
        }

        std::string full_url() const
        {
            if (!use_proxy || host.empty())
            {
                return "";
            }

            std::string url = protocol.empty() ? "http" : protocol;
            url += "://";

            if (!username.empty())
            {
                url += username;
                if (!password.empty())
                {
                    url += ":" + password;
                }
                url += "@";
            }

            url += host;
            if (!port.empty())
            {
                url += ":" + port;
            }

            return url;
        }

    private:
        void parse(const std::string &proxy_url)
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
