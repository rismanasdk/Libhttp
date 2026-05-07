#ifndef LIBHTTP_DETAIL_H
#define LIBHTTP_DETAIL_H

#include <arpa/inet.h>
#include <cctype>
#include <cerrno>
#include <cstring>
#include <netdb.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

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

inline std::string escape_json(const std::string& value) {
    std::string result;

    for (char ch : value) {
        switch (ch) {
            case '\\':
                result += "\\\\";
                break;
            case '"':
                result += "\\\"";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += ch;
                break;
        }
    }

    return result;
}

inline std::string json_dump(const Json& json) {
    std::string result = "{";
    bool first = true;

    for (const auto& item : json) {
        if (!first) {
            result += ",";
        }

        result += "\"";
        result += escape_json(item.first);
        result += "\":\"";
        result += escape_json(item.second);
        result += "\"";
        first = false;
    }

    result += "}";
    return result;
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

inline std::string build_query(const Params& params) {
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

inline std::string build_cookie_header(const Cookies& cookies) {
    std::string value;
    bool first = true;

    for (const auto& item : cookies) {
        if (!first) {
            value += "; ";
        }

        value += item.first + "=" + item.second;
        first = false;
    }

    return value;
}

inline bool contains_token_case_insensitive(const std::string& value, const std::string& token) {
    const std::string lower_value = to_lower(value);
    const std::string lower_token = to_lower(token);
    return lower_value.find(lower_token) != std::string::npos;
}

inline ParsedUrl parse_url(const std::string& url) {
    const std::string prefix = "http://";
    if (url.compare(0, prefix.size(), prefix) != 0) {
        throw std::runtime_error("Only http:// URLs are supported.");
    }

    ParsedUrl parsed;
    parsed.scheme = "http";
    parsed.port = "80";

    const std::string remaining = url.substr(prefix.size());
    const std::size_t slash_pos = remaining.find('/');
    const std::string host_port = slash_pos == std::string::npos ? remaining : remaining.substr(0, slash_pos);
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

inline std::string host_header_value(const ParsedUrl& parsed) {
    if (parsed.port == "80") {
        return parsed.host;
    }

    return parsed.host + ":" + parsed.port;
}

inline std::string merge_target_and_query(const std::string& target, const Params& params) {
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

inline std::string resolve_redirect_url(const std::string& base_url, const std::string& location) {
    if (location.compare(0, 7, "http://") == 0) {
        return location;
    }

    const ParsedUrl parsed = parse_url(base_url);
    const std::string origin = parsed.scheme + "://" + parsed.host + (parsed.port == "80" ? "" : ":" + parsed.port);

    if (!location.empty() && location.front() == '/') {
        return origin + location;
    }

    std::string base_target = parsed.target;
    const std::size_t slash_pos = base_target.find_last_of('/');
    const std::string parent = slash_pos == std::string::npos ? "/" : base_target.substr(0, slash_pos + 1);
    return origin + parent + location;
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

        if (chunk_size == 0 || position + chunk_size > chunked_body.size()) {
            break;
        }

        decoded.append(chunked_body, position, chunk_size);
        position += chunk_size + 2;
    }

    return decoded;
}

inline Cookies parse_set_cookie(const std::string& header_value) {
    Cookies cookies;
    const std::size_t semicolon_pos = header_value.find(';');
    const std::string first_part = semicolon_pos == std::string::npos ? header_value : header_value.substr(0, semicolon_pos);
    const std::size_t equal_pos = first_part.find('=');

    if (equal_pos == std::string::npos) {
        return cookies;
    }

    const std::string key = trim(first_part.substr(0, equal_pos));
    const std::string value = trim(first_part.substr(equal_pos + 1));

    if (!key.empty()) {
        cookies[key] = value;
    }

    return cookies;
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

        if (to_lower(key) == "set-cookie") {
            const Cookies parsed_cookies = parse_set_cookie(value);
            response.cookies.insert(parsed_cookies.begin(), parsed_cookies.end());
        }
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

inline bool is_redirect_status(int status_code) {
    return status_code == 301 || status_code == 302 || status_code == 303 || status_code == 307 || status_code == 308;
}

inline std::string prepare_body(RequestOptions& options) {
    if (!options.body.empty()) {
        return options.body;
    }

    if (!options.json.empty()) {
        if (options.headers.find("Content-Type") == options.headers.end()) {
            options.headers["Content-Type"] = "application/json";
        }
        return json_dump(options.json);
    }

    if (!options.data.empty()) {
        if (options.headers.find("Content-Type") == options.headers.end()) {
            options.headers["Content-Type"] = "application/x-www-form-urlencoded";
        }
        return build_query(options.data);
    }

    return "";
}

}  // namespace detail
}  // namespace http

#endif
