#ifndef LIBHTTP_HTTP_H
#define LIBHTTP_HTTP_H

#include <arpa/inet.h>
#include <cctype>
#include <cerrno>
#include <cstring>
#include <map>
#include <netdb.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

#include "http_request.h"
#include "http_response.h"

namespace http {

namespace detail {

struct ParsedUrl {
    std::string scheme;
    std::string host;
    std::string port;
    std::string target;
};

inline std::string trim(const std::string& value) {
    std::size_t start = 0;
    std::size_t end = value.size();

    while (start < end && std::isspace(static_cast<unsigned char>(value[start]))) {
        ++start;
    }

    while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(start, end - start);
}

inline std::string to_lower(std::string value) {
    for (char& ch : value) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }

    return value;
}

inline std::string url_encode(const std::string& value) {
    static const char* hex = "0123456789ABCDEF";
    std::string result;

    for (unsigned char ch : value) {
        if (std::isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
            result.push_back(static_cast<char>(ch));
        } else if (ch == ' ') {
            result.push_back('+');
        } else {
            result.push_back('%');
            result.push_back(hex[ch >> 4]);
            result.push_back(hex[ch & 15]);
        }
    }

    return result;
}

inline std::string build_query(const std::map<std::string, std::string>& params) {
    std::string query;
    bool first = true;

    for (const auto& item : params) {
        if (!first) {
            query += '&';
        }

        query += url_encode(item.first);
        query += '=';
        query += url_encode(item.second);
        first = false;
    }

    return query;
}

inline std::string merge_target_and_query(const std::string& target, const std::map<std::string, std::string>& params) {
    if (params.empty()) {
        return target.empty() ? "/" : target;
    }

    const std::string query = build_query(params);
    const std::string base = target.empty() ? "/" : target;

    if (base.find('?') == std::string::npos) {
        return base + "?" + query;
    }

    return base + "&" + query;
}

inline bool contains_token_case_insensitive(const std::string& value, const std::string& token) {
    const std::string lower_value = to_lower(value);
    const std::string lower_token = to_lower(token);
    return lower_value.find(lower_token) != std::string::npos;
}

inline std::string host_header_value(const ParsedUrl& parsed) {
    if (parsed.port == "80") {
        return parsed.host;
    }

    return parsed.host + ":" + parsed.port;
}

inline ParsedUrl parse_url(const std::string& url) {
    const std::string prefix = "http://";
    if (url.compare(0, prefix.size(), prefix) != 0) {
        throw std::runtime_error("Only http:// URLs are supported.");
    }

    ParsedUrl parsed;
    parsed.scheme = "http";
    parsed.port = "80";

    std::string remaining = url.substr(prefix.size());
    const std::size_t slash_pos = remaining.find('/');
    std::string host_port = slash_pos == std::string::npos ? remaining : remaining.substr(0, slash_pos);
    parsed.target = slash_pos == std::string::npos ? "/" : remaining.substr(slash_pos);

    const std::size_t colon_pos = host_port.find(':');
    if (colon_pos == std::string::npos) {
        parsed.host = host_port;
    } else {
        parsed.host = host_port.substr(0, colon_pos);
        parsed.port = host_port.substr(colon_pos + 1);
    }

    if (parsed.host.empty()) {
        throw std::runtime_error("Invalid URL host.");
    }

    return parsed;
}

inline int connect_socket(const ParsedUrl& parsed, int timeout_seconds) {
    struct addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* result = nullptr;
    const int status = getaddrinfo(parsed.host.c_str(), parsed.port.c_str(), &hints, &result);
    if (status != 0) {
        throw std::runtime_error("Failed to resolve host: " + parsed.host);
    }

    int sockfd = -1;

    for (struct addrinfo* rp = result; rp != nullptr; rp = rp->ai_next) {
        sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sockfd == -1) {
            continue;
        }

        timeval timeout;
        timeout.tv_sec = timeout_seconds;
        timeout.tv_usec = 0;
        setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

        if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            freeaddrinfo(result);
            return sockfd;
        }

        close(sockfd);
        sockfd = -1;
    }

    freeaddrinfo(result);
    throw std::runtime_error("Failed to connect to " + parsed.host + ":" + parsed.port);
}

inline void send_all(int sockfd, const std::string& request_text) {
    std::size_t sent_total = 0;

    while (sent_total < request_text.size()) {
        const ssize_t sent_now = send(sockfd, request_text.data() + sent_total, request_text.size() - sent_total, 0);
        if (sent_now <= 0) {
            throw std::runtime_error("Failed to send HTTP request.");
        }
        sent_total += static_cast<std::size_t>(sent_now);
    }
}

inline std::string recv_until_close(int sockfd) {
    std::string response;
    char buffer[4096];

    while (true) {
        const ssize_t received = recv(sockfd, buffer, sizeof(buffer), 0);
        if (received == 0) {
            break;
        }
        if (received < 0) {
            if (errno == EWOULDBLOCK || errno == EAGAIN) {
                break;
            }
            throw std::runtime_error("Failed to receive HTTP response.");
        }
        response.append(buffer, static_cast<std::size_t>(received));
    }

    return response;
}

inline std::string decode_chunked_body(const std::string& chunked_body) {
    std::string decoded;
    std::size_t position = 0;

    while (position < chunked_body.size()) {
        const std::size_t line_end = chunked_body.find("\r\n", position);
        if (line_end == std::string::npos) {
            break;
        }

        std::string size_text = chunked_body.substr(position, line_end - position);
        const std::size_t extension_pos = size_text.find(';');
        if (extension_pos != std::string::npos) {
            size_text = size_text.substr(0, extension_pos);
        }

        std::stringstream converter;
        converter << std::hex << size_text;

        std::size_t chunk_size = 0;
        converter >> chunk_size;

        position = line_end + 2;
        if (chunk_size == 0) {
            break;
        }

        if (position + chunk_size > chunked_body.size()) {
            break;
        }

        decoded.append(chunked_body, position, chunk_size);
        position += chunk_size + 2;
    }

    return decoded;
}

inline Response parse_response(const std::string& raw_response, const std::string& method, const std::string& url) {
    Response response;
    response.method = method;
    response.url = url;

    const std::size_t header_end = raw_response.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        response.body = raw_response;
        return response;
    }

    const std::string header_block = raw_response.substr(0, header_end);
    std::string body = raw_response.substr(header_end + 4);
    std::istringstream header_stream(header_block);

    std::string status_line;
    std::getline(header_stream, status_line);
    if (!status_line.empty() && status_line.back() == '\r') {
        status_line.pop_back();
    }

    std::istringstream status_parser(status_line);
    std::string http_version;
    status_parser >> http_version;
    status_parser >> response.status_code;
    std::getline(status_parser, response.reason);
    response.reason = trim(response.reason);

    std::string line;
    while (std::getline(header_stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        const std::size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
            continue;
        }

        const std::string key = trim(line.substr(0, colon_pos));
        const std::string value = trim(line.substr(colon_pos + 1));
        response.headers[key] = value;
    }

    if (contains_token_case_insensitive(response.header("Transfer-Encoding"), "chunked")) {
        body = decode_chunked_body(body);
    }

    if (to_lower(method) == "head") {
        body.clear();
    }

    response.body = body;
    return response;
}

}  // namespace detail

inline Response request(const std::string& method, const std::string& url, RequestOptions options = {}) {
    const detail::ParsedUrl parsed = detail::parse_url(url);
    const std::string target = detail::merge_target_and_query(parsed.target, options.params);

    std::string body = options.body;
    if (body.empty() && !options.data.empty()) {
        body = detail::build_query(options.data);
        if (options.headers.find("Content-Type") == options.headers.end()) {
            options.headers["Content-Type"] = "application/x-www-form-urlencoded";
        }
    }

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

inline Response head(const std::string& url, RequestOptions options = {}) {
    return request("HEAD", url, options);
}

inline Response options(const std::string& url, RequestOptions options = {}) {
    return request("OPTIONS", url, options);
}

inline Response patch(const std::string& url, RequestOptions options = {}) {
    return request("PATCH", url, options);
}

class Client {
   private:
    std::string base_url;
    std::map<std::string, std::string> default_headers;
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

    RequestOptions prepare(RequestOptions options) const {
        for (const auto& header : default_headers) {
            if (options.headers.find(header.first) == options.headers.end()) {
                options.headers[header.first] = header.second;
            }
        }

        if (options.timeout_seconds <= 0) {
            options.timeout_seconds = timeout;
        }

        return options;
    }

   public:
    Client() = default;

    explicit Client(const std::string& url) : base_url(url) {}

    Client(const std::string& url, const std::map<std::string, std::string>& headers, int timeout_seconds = 10)
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

    void set_headers(const std::map<std::string, std::string>& headers) {
        default_headers = headers;
    }

    const std::map<std::string, std::string>& headers() const {
        return default_headers;
    }

    Response request(const std::string& method, const std::string& path, RequestOptions options = {}) const {
        return http::request(method, build_url(path), prepare(options));
    }

    Response get(const std::string& path, RequestOptions options = {}) const {
        return request("GET", path, options);
    }

    Response post(const std::string& path, RequestOptions options = {}) const {
        return request("POST", path, options);
    }

    Response put(const std::string& path, RequestOptions options = {}) const {
        return request("PUT", path, options);
    }

    Response delete_(const std::string& path, RequestOptions options = {}) const {
        return request("DELETE", path, options);
    }

    Response head(const std::string& path, RequestOptions options = {}) const {
        return request("HEAD", path, options);
    }

    Response options(const std::string& path, RequestOptions options = {}) const {
        return request("OPTIONS", path, options);
    }

    Response patch(const std::string& path, RequestOptions options = {}) const {
        return request("PATCH", path, options);
    }
};

}  // namespace http

#endif
