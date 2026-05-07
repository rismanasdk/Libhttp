#ifndef LIBHTTP_SESSION_H
#define LIBHTTP_SESSION_H

#include <fstream>
#include <string>

#include "http_detail.h"

namespace http
{

    class Session
    {
    private:
        std::string base_url;
        Headers default_headers;
        Cookies cookie_jar;
        std::string auth_username;
        std::string auth_password;
        ProxyConfig proxy_config;
        int timeout = 10;

        std::string build_url(const std::string &path) const
        {
            if (path.compare(0, 7, "http://") == 0)
            {
                return path;
            }

            if (base_url.empty())
            {
                return path;
            }

            if (!base_url.empty() && base_url.back() == '/' && !path.empty() && path.front() == '/')
            {
                return base_url.substr(0, base_url.size() - 1) + path;
            }

            if (!base_url.empty() && base_url.back() != '/' && !path.empty() && path.front() != '/')
            {
                return base_url + "/" + path;
            }

            return base_url + path;
        }

        RequestOptions merge_options(RequestOptions options) const
        {
            for (const auto &header : default_headers)
            {
                if (options.headers.find(header.first) == options.headers.end())
                {
                    options.headers[header.first] = header.second;
                }
            }

            for (const auto &cookie : cookie_jar)
            {
                if (options.cookies.find(cookie.first) == options.cookies.end())
                {
                    options.cookies[cookie.first] = cookie.second;
                }
            }

            if (options.timeout_seconds <= 0)
            {
                options.timeout_seconds = timeout;
            }

            if (options.auth_username.empty() && !auth_username.empty())
            {
                options.auth_username = auth_username;
                options.auth_password = auth_password;
            }

            if (!options.proxy.use_proxy && proxy_config.use_proxy)
            {
                options.proxy = proxy_config;
            }

            return options;
        }

    public:
        Session() = default;

        explicit Session(const std::string &url) : base_url(url) {}

        Session(const std::string &url, const Headers &headers, int timeout_seconds = 10)
            : base_url(url), default_headers(headers), timeout(timeout_seconds) {}

        void set_base_url(const std::string &url)
        {
            base_url = url;
        }

        const std::string &get_base_url() const
        {
            return base_url;
        }

        void set_timeout(int timeout_seconds)
        {
            timeout = timeout_seconds;
        }

        int get_timeout() const
        {
            return timeout;
        }

        void set_header(const std::string &key, const std::string &value)
        {
            default_headers[key] = value;
        }

        void set_headers(const Headers &headers)
        {
            default_headers = headers;
        }

        const Headers &headers() const
        {
            return default_headers;
        }

        void set_basic_auth(const std::string &username, const std::string &password)
        {
            auth_username = username;
            auth_password = password;
        }

        void set_cookie(const std::string &key, const std::string &value)
        {
            cookie_jar[key] = value;
        }

        const Cookies &cookies() const
        {
            return cookie_jar;
        }

        void clear_cookies()
        {
            cookie_jar.clear();
        }

        void set_proxy(const std::string &proxy_host, const std::string &proxy_port = "8080")
        {
            proxy_config.use_proxy = true;
            proxy_config.host = proxy_host;
            proxy_config.port = proxy_port;
            proxy_config.protocol = "http";
        }

        void set_proxy_with_auth(const std::string &proxy_host, const std::string &proxy_port,
                                 const std::string &proxy_user, const std::string &proxy_pass)
        {
            set_proxy(proxy_host, proxy_port);
            proxy_config.username = proxy_user;
            proxy_config.password = proxy_pass;
        }

        void set_proxy_url(const std::string &proxy_url)
        {
            ProxyConfig config(proxy_url);
            proxy_config = config;
        }

        void disable_proxy()
        {
            proxy_config.use_proxy = false;
        }

        const ProxyConfig &get_proxy() const
        {
            return proxy_config;
        }

        void set_bearer_auth(const std::string &token)
        {
            set_header("Authorization", "Bearer " + token);
        }

        void set_api_key(const std::string &key, const std::string &header_name = "X-API-Key")
        {
            set_header(header_name, key);
        }

        void set_auth_type(const std::string &type, const std::string &token)
        {
            set_header("Authorization", type + " " + token);
        }

        void clear_auth()
        {
            auth_username.clear();
            auth_password.clear();
            default_headers.erase("Authorization");
        }

        Response request(const std::string &method, const std::string &path, RequestOptions options = {});
        Response get(const std::string &path, RequestOptions options = {});
        Response post(const std::string &path, RequestOptions options = {});
        Response put(const std::string &path, RequestOptions options = {});
        Response delete_(const std::string &path, RequestOptions options = {});
        Response options(const std::string &path, RequestOptions options = {});
        Response head(const std::string &path, RequestOptions options = {});
        Response patch(const std::string &path, RequestOptions options = {});

        bool download(const std::string &path, const std::string &output_path, RequestOptions options = {})
        {
            const Response response = get(path, options);
            if (!response.ok())
            {
                return false;
            }

            std::ofstream output(output_path, std::ios::binary | std::ios::trunc);
            if (!output.is_open())
            {
                return false;
            }

            output.write(response.body.data(), static_cast<std::streamsize>(response.body.size()));
            return output.good();
        }

        bool stream_download(const std::string &path, const std::string &output_path,
                             const DownloadProgressCallback &progress_callback = nullptr,
                             RequestOptions options = {})
        {
            options.stream = true;
            options.progress_callback = progress_callback;

            const Response response = get(path, options);
            if (!response.ok())
            {
                return false;
            }

            std::ofstream output(output_path, std::ios::binary | std::ios::trunc);
            if (!output.is_open())
            {
                return false;
            }

            size_t total = response.content_length();
            size_t chunk_size = options.chunk_size;
            for (size_t offset = 0; offset < response.body.length(); offset += chunk_size)
            {
                size_t len = std::min(chunk_size, response.body.length() - offset);
                output.write(response.body.data() + offset, static_cast<std::streamsize>(len));

                if (progress_callback)
                {
                    progress_callback(offset + len, total);
                }
            }

            return output.good();
        }
    };

    using Client = Session;

} // namespace http

#endif
