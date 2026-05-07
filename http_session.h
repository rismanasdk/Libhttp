#ifndef LIBHTTP_SESSION_H
#define LIBHTTP_SESSION_H

#include <string>

#include "http_detail.h"

namespace http {

class Session {
   private:
    std::string base_url;
    Headers default_headers;
    Cookies cookie_jar;
    int timeout = 10;

    std::string build_url(const std::string& path) const {
        if (path.compare(0, 7, "http://") == 0) {
            return path;
        }

        if (base_url.empty()) {
            return path;
        }

        if (!base_url.empty() && base_url.back() == '/' && !path.empty() && path.front() == '/') {
            return base_url.substr(0, base_url.size() - 1) + path;
        }

        if (!base_url.empty() && base_url.back() != '/' && !path.empty() && path.front() != '/') {
            return base_url + "/" + path;
        }

        return base_url + path;
    }

    RequestOptions merge_options(RequestOptions options) const {
        for (const auto& header : default_headers) {
            if (options.headers.find(header.first) == options.headers.end()) {
                options.headers[header.first] = header.second;
            }
        }

        for (const auto& cookie : cookie_jar) {
            if (options.cookies.find(cookie.first) == options.cookies.end()) {
                options.cookies[cookie.first] = cookie.second;
            }
        }

        if (options.timeout_seconds <= 0) {
            options.timeout_seconds = timeout;
        }

        return options;
    }

   public:
    Session() = default;

    explicit Session(const std::string& url) : base_url(url) {}

    Session(const std::string& url, const Headers& headers, int timeout_seconds = 10)
        : base_url(url), default_headers(headers), timeout(timeout_seconds) {}

    void set_base_url(const std::string& url) {
        base_url = url;
    }

    const std::string& get_base_url() const {
        return base_url;
    }

    void set_timeout(int timeout_seconds) {
        timeout = timeout_seconds;
    }

    int get_timeout() const {
        return timeout;
    }

    void set_header(const std::string& key, const std::string& value) {
        default_headers[key] = value;
    }

    void set_headers(const Headers& headers) {
        default_headers = headers;
    }

    const Headers& headers() const {
        return default_headers;
    }

    void set_cookie(const std::string& key, const std::string& value) {
        cookie_jar[key] = value;
    }

    const Cookies& cookies() const {
        return cookie_jar;
    }

    void clear_cookies() {
        cookie_jar.clear();
    }

    Response request(const std::string& method, const std::string& path, RequestOptions options = {});
    Response get(const std::string& path, RequestOptions options = {});
    Response post(const std::string& path, RequestOptions options = {});
    Response put(const std::string& path, RequestOptions options = {});
    Response delete_(const std::string& path, RequestOptions options = {});
    Response options(const std::string& path, RequestOptions options = {});
    Response head(const std::string& path, RequestOptions options = {});
    Response patch(const std::string& path, RequestOptions options = {});
};

using Client = Session;

}  // namespace http

#endif
