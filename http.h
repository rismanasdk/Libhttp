#ifndef LIBHTTP_HTTP_H
#define LIBHTTP_HTTP_H

#include <fstream>
#include <sstream>
#include <string>

#include "http_session.h"

namespace http {

inline Response request_once(const std::string& method, const std::string& url, RequestOptions options = {}) {
    const detail::ParsedUrl parsed = detail::parse_url(url);
    const std::string target = detail::merge_target_and_query(parsed.target, options.params);
    const std::string body = detail::prepare_body(options);

    if (options.headers.find("Host") == options.headers.end()) {
        options.headers["Host"] = detail::host_header_value(parsed);
    }
    if (options.headers.find("User-Agent") == options.headers.end()) {
        options.headers["User-Agent"] = "libhttp/1.0";
    }
    if (options.headers.find("Accept") == options.headers.end()) {
        options.headers["Accept"] = "*/*";
    }
    if (options.headers.find("Connection") == options.headers.end()) {
        options.headers["Connection"] = "close";
    }
    if (!options.auth_username.empty() && options.headers.find("Authorization") == options.headers.end()) {
        options.headers["Authorization"] =
            detail::build_basic_auth_header(options.auth_username, options.auth_password);
    }
    if (!options.cookies.empty() && options.headers.find("Cookie") == options.headers.end()) {
        options.headers["Cookie"] = detail::build_cookie_header(options.cookies);
    }
    if (!body.empty() && options.headers.find("Content-Length") == options.headers.end()) {
        options.headers["Content-Length"] = std::to_string(body.size());
    }

    std::ostringstream request_stream;
    request_stream << method << " " << target << " HTTP/1.1\r\n";
    for (const auto& header : options.headers) {
        request_stream << header.first << ": " << header.second << "\r\n";
    }
    request_stream << "\r\n";
    request_stream << body;

    const int sockfd = detail::connect_socket(parsed, options.timeout_seconds);

    try {
        detail::send_all(sockfd, request_stream.str());
        const std::string raw_response = detail::recv_until_close(sockfd);
        close(sockfd);
        return detail::parse_response(raw_response, method, url);
    } catch (...) {
        close(sockfd);
        throw;
    }
}

inline Response request(const std::string& method, const std::string& url, RequestOptions options = {}) {
    Response response;
    std::string current_method = method;
    std::string current_url = url;

    for (int redirects = 0;; ++redirects) {
        for (int attempt = 0;; ++attempt) {
            try {
                response = request_once(current_method, current_url, options);
                break;
            } catch (...) {
                if (attempt >= options.retry_count) {
                    throw;
                }
                detail::sleep_retry_delay(options.retry_delay_ms);
            }
        }

        if (!options.allow_redirects || !detail::is_redirect_status(response.status_code)) {
            break;
        }

        const std::string location = response.header("Location");
        if (location.empty() || redirects >= options.max_redirects) {
            break;
        }

        response.history.push_back(current_url);
        current_url = detail::resolve_redirect_url(current_url, location);

        if (response.status_code == 303) {
            current_method = "GET";
            options.body.clear();
            options.data.clear();
            options.json.clear();
        }
    }

    response.url = current_url;
    return response;
}

inline Response get(const std::string& url, RequestOptions options = {}) {
    return request("GET", url, options);
}

inline Response post(const std::string& url, RequestOptions options = {}) {
    return request("POST", url, options);
}

inline Response put(const std::string& url, RequestOptions options = {}) {
    return request("PUT", url, options);
}

inline Response delete_(const std::string& url, RequestOptions options = {}) {
    return request("DELETE", url, options);
}

inline Response options(const std::string& url, RequestOptions options = {}) {
    return request("OPTIONS", url, options);
}

inline Response head(const std::string& url, RequestOptions options = {}) {
    return request("HEAD", url, options);
}

inline Response patch(const std::string& url, RequestOptions options = {}) {
    return request("PATCH", url, options);
}

inline bool download(const std::string& url, const std::string& output_path, RequestOptions options = {}) {
    const Response response = get(url, options);
    if (!response.ok()) {
        return false;
    }

    std::ofstream output(output_path, std::ios::binary | std::ios::trunc);
    if (!output.is_open()) {
        return false;
    }

    output.write(response.body.data(), static_cast<std::streamsize>(response.body.size()));
    return output.good();
}

inline Response Session::request(const std::string& method, const std::string& path, RequestOptions options) {
    options = merge_options(options);
    Response response = http::request(method, build_url(path), options);

    for (const auto& cookie : response.cookies) {
        cookie_jar[cookie.first] = cookie.second;
    }

    return response;
}

inline Response Session::get(const std::string& path, RequestOptions options) {
    return request("GET", path, options);
}

inline Response Session::post(const std::string& path, RequestOptions options) {
    return request("POST", path, options);
}

inline Response Session::put(const std::string& path, RequestOptions options) {
    return request("PUT", path, options);
}

inline Response Session::delete_(const std::string& path, RequestOptions options) {
    return request("DELETE", path, options);
}

inline Response Session::options(const std::string& path, RequestOptions options) {
    return request("OPTIONS", path, options);
}

inline Response Session::head(const std::string& path, RequestOptions options) {
    return request("HEAD", path, options);
}

inline Response Session::patch(const std::string& path, RequestOptions options) {
    return request("PATCH", path, options);
}

}  // namespace http

#endif
