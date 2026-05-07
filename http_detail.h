#ifndef LIBHTTP_DETAIL_H
#define LIBHTTP_DETAIL_H

#include <arpa/inet.h>
#include <cctype>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <fstream>
#include <netdb.h>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <thread>
#include <unistd.h>
#include <functional>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <zlib.h>

#include "http_response.h"

namespace http
{
    namespace detail
    {

        struct ParsedUrl
        {
            std::string scheme;
            std::string host;
            std::string port;
            std::string target;
        };

        inline std::string trim(const std::string &value)
        {
            std::size_t start = 0;
            std::size_t end = value.size();

            while (start < end && std::isspace(static_cast<unsigned char>(value[start])))
            {
                ++start;
            }

            while (end > start && std::isspace(static_cast<unsigned char>(value[end - 1])))
            {
                --end;
            }

            return value.substr(start, end - start);
        }

        inline std::string to_lower(std::string value)
        {
            for (char &ch : value)
            {
                ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }

            return value;
        }

        inline std::string escape_json(const std::string &value)
        {
            std::string result;

            for (char ch : value)
            {
                switch (ch)
                {
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

        inline std::string json_dump(const Json &json)
        {
            std::string result = "{";
            bool first = true;

            for (const auto &item : json)
            {
                if (!first)
                {
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

        inline std::string url_encode(const std::string &value)
        {
            static const char *hex = "0123456789ABCDEF";
            std::string result;

            for (unsigned char ch : value)
            {
                if (std::isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~')
                {
                    result.push_back(static_cast<char>(ch));
                }
                else if (ch == ' ')
                {
                    result.push_back('+');
                }
                else
                {
                    result.push_back('%');
                    result.push_back(hex[ch >> 4]);
                    result.push_back(hex[ch & 15]);
                }
            }

            return result;
        }

        inline std::string base64_encode(const std::string &value)
        {
            static const char *table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
            std::string result;
            int val = 0;
            int valb = -6;

            for (unsigned char ch : value)
            {
                val = (val << 8) + ch;
                valb += 8;
                while (valb >= 0)
                {
                    result.push_back(table[(val >> valb) & 0x3F]);
                    valb -= 6;
                }
            }

            if (valb > -6)
            {
                result.push_back(table[((val << 8) >> (valb + 8)) & 0x3F]);
            }

            while (result.size() % 4 != 0)
            {
                result.push_back('=');
            }

            return result;
        }

        inline std::string build_basic_auth_header(const std::string &username, const std::string &password)
        {
            return "Basic " + base64_encode(username + ":" + password);
        }

        inline std::string build_bearer_auth_header(const std::string &token)
        {
            return "Bearer " + token;
        }

        inline std::string build_custom_auth_header(const std::string &scheme, const std::string &credentials)
        {
            return scheme + " " + credentials;
        }

        inline std::string build_query(const Params &params)
        {
            std::string query;
            bool first = true;

            for (const auto &item : params)
            {
                if (!first)
                {
                    query += '&';
                }

                query += url_encode(item.first);
                query += '=';
                query += url_encode(item.second);
                first = false;
            }

            return query;
        }

        inline std::string build_cookie_header(const Cookies &cookies)
        {
            std::string value;
            bool first = true;

            for (const auto &item : cookies)
            {
                if (!first)
                {
                    value += "; ";
                }

                value += item.first + "=" + item.second;
                first = false;
            }

            return value;
        }

        inline std::string read_file_text(const std::string &path)
        {
            std::ifstream file(path, std::ios::binary);
            if (!file.is_open())
            {
                throw std::runtime_error("Failed to open upload file: " + path);
            }

            std::ostringstream buffer;
            buffer << file.rdbuf();
            return buffer.str();
        }

        inline std::string filename_from_path(const std::string &path)
        {
            const std::size_t slash_pos = path.find_last_of("/\\");
            return slash_pos == std::string::npos ? path : path.substr(slash_pos + 1);
        }

        inline std::string multipart_boundary()
        {
            return "----libhttp-boundary-20260507";
        }

        inline std::string build_multipart_body(const Params &data, const Files &files, const std::string &boundary)
        {
            std::string body;

            for (const auto &field : data)
            {
                body += "--" + boundary + "\r\n";
                body += "Content-Disposition: form-data; name=\"" + field.first + "\"\r\n\r\n";
                body += field.second + "\r\n";
            }

            for (const auto &field : files)
            {
                body += "--" + boundary + "\r\n";
                body += "Content-Disposition: form-data; name=\"" + field.first + "\"; filename=\"" +
                        filename_from_path(field.second) + "\"\r\n";
                body += "Content-Type: application/octet-stream\r\n\r\n";
                body += read_file_text(field.second);
                body += "\r\n";
            }

            body += "--" + boundary + "--\r\n";
            return body;
        }

        inline bool contains_token_case_insensitive(const std::string &value, const std::string &token)
        {
            const std::string lower_value = to_lower(value);
            const std::string lower_token = to_lower(token);
            return lower_value.find(lower_token) != std::string::npos;
        }

        inline ParsedUrl parse_url(const std::string &url)
        {
            ParsedUrl parsed;
            parsed.port = "80";
            parsed.scheme = "http";

            std::string remaining;
            const std::string http_prefix = "http://";
            const std::string https_prefix = "https://";

            if (url.compare(0, http_prefix.size(), http_prefix) == 0)
            {
                remaining = url.substr(http_prefix.size());
                parsed.scheme = "http";
                parsed.port = "80";
            }
            else if (url.compare(0, https_prefix.size(), https_prefix) == 0)
            {
                remaining = url.substr(https_prefix.size());
                parsed.scheme = "https";
                parsed.port = "443";
            }
            else
            {
                throw std::runtime_error("Only http:// and https:// URLs are supported.");
            }

            const std::size_t slash_pos = remaining.find('/');
            const std::string host_port = slash_pos == std::string::npos ? remaining : remaining.substr(0, slash_pos);
            parsed.target = slash_pos == std::string::npos ? "/" : remaining.substr(slash_pos);

            const std::size_t colon_pos = host_port.find(':');
            if (colon_pos == std::string::npos)
            {
                parsed.host = host_port;
            }
            else
            {
                parsed.host = host_port.substr(0, colon_pos);
                parsed.port = host_port.substr(colon_pos + 1);
            }

            if (parsed.host.empty())
            {
                throw std::runtime_error("Invalid URL host.");
            }

            return parsed;
        }

        inline std::string host_header_value(const ParsedUrl &parsed)
        {
            if (parsed.port == "80")
            {
                return parsed.host;
            }

            return parsed.host + ":" + parsed.port;
        }

        // Initialize OpenSSL and provide a shared SSL_CTX
        inline SSL_CTX *global_ssl_ctx()
        {
            static SSL_CTX *ctx = nullptr;
            if (!ctx)
            {
                SSL_library_init();
                SSL_load_error_strings();
                const SSL_METHOD *method = TLS_client_method();
                ctx = SSL_CTX_new(method);
                if (!ctx)
                {
                    throw std::runtime_error("Failed to create SSL_CTX");
                }
            }
            return ctx;
        }

        // Wrap socket with TLS and perform handshake
        inline SSL *tls_wrap_socket(int sockfd)
        {
            SSL_CTX *ctx = global_ssl_ctx();
            SSL *ssl = SSL_new(ctx);
            if (!ssl)
            {
                throw std::runtime_error("Failed to create SSL object");
            }
            SSL_set_fd(ssl, sockfd);
            if (SSL_connect(ssl) != 1)
            {
                long err = ERR_get_error();
                SSL_free(ssl);
                throw std::runtime_error(std::string("TLS handshake failed: ") + ERR_error_string(err, nullptr));
            }
            return ssl;
        }

        inline void send_all_ssl(SSL *ssl, const std::string &request_text)
        {
            size_t sent_total = 0;
            while (sent_total < request_text.size())
            {
                int sent = SSL_write(ssl, request_text.data() + sent_total, static_cast<int>(request_text.size() - sent_total));
                if (sent <= 0)
                {
                    throw std::runtime_error("Failed to send HTTP request over TLS.");
                }
                sent_total += static_cast<size_t>(sent);
            }
        }

        inline std::string recv_until_close_ssl(SSL *ssl)
        {
            std::string response;
            char buffer[4096];
            while (true)
            {
                int received = SSL_read(ssl, buffer, sizeof(buffer));
                if (received == 0)
                    break;
                if (received < 0)
                {
                    int err = SSL_get_error(ssl, received);
                    if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE)
                    {
                        continue;
                    }
                    throw std::runtime_error("Failed to receive HTTP response over TLS.");
                }
                response.append(buffer, static_cast<size_t>(received));
            }
            return response;
        }

        // Streaming receive into processor (supports plain socket and SSL)
        inline bool recv_stream_to_processor(int sockfd, SSL *ssl, const std::function<bool(const char *, size_t)> &processor)
        {
            char buffer[8192];
            while (true)
            {
                ssize_t received = 0;
                if (ssl)
                {
                    received = SSL_read(ssl, buffer, sizeof(buffer));
                    if (received <= 0)
                        break;
                }
                else
                {
                    received = recv(sockfd, buffer, sizeof(buffer), 0);
                    if (received <= 0)
                    {
                        if (received == 0)
                            break;
                        if (errno == EWOULDBLOCK || errno == EAGAIN)
                            break;
                        throw std::runtime_error("Failed to receive HTTP response.");
                    }
                }

                if (!processor(buffer, static_cast<size_t>(received)))
                {
                    return false;
                }
            }
            return true;
        }

        // Decompress gzip data
        inline std::string decompress_gzip(const std::string &data)
        {
            z_stream strm;
            std::memset(&strm, 0, sizeof(strm));
            strm.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(data.data()));
            strm.avail_in = static_cast<uInt>(data.size());

            if (inflateInit2(&strm, 16 + MAX_WBITS) != Z_OK)
            {
                throw std::runtime_error("Failed to initialize zlib for gzip decompression");
            }

            std::string out;
            char outbuf[8192];
            int ret;
            do
            {
                strm.next_out = reinterpret_cast<Bytef *>(outbuf);
                strm.avail_out = sizeof(outbuf);
                ret = inflate(&strm, Z_NO_FLUSH);
                if (ret != Z_OK && ret != Z_STREAM_END)
                {
                    inflateEnd(&strm);
                    throw std::runtime_error("Error during gzip decompression");
                }
                out.append(outbuf, sizeof(outbuf) - strm.avail_out);
            } while (ret != Z_STREAM_END);

            inflateEnd(&strm);
            return out;
        }

        // Decompress deflate compressed data
        inline std::string decompress_deflate(const std::string &data)
        {
            z_stream strm;
            std::memset(&strm, 0, sizeof(strm));
            strm.next_in = reinterpret_cast<Bytef *>(const_cast<char *>(data.data()));
            strm.avail_in = static_cast<uInt>(data.size());

            if (inflateInit2(&strm, MAX_WBITS) != Z_OK)
            {
                throw std::runtime_error("Failed to initialize zlib for deflate decompression");
            }

            std::string out;
            char outbuf[8192];
            int ret;
            do
            {
                strm.next_out = reinterpret_cast<Bytef *>(outbuf);
                strm.avail_out = sizeof(outbuf);
                ret = inflate(&strm, Z_NO_FLUSH);
                if (ret != Z_OK && ret != Z_STREAM_END)
                {
                    inflateEnd(&strm);
                    throw std::runtime_error("Error during deflate decompression");
                }
                out.append(outbuf, sizeof(outbuf) - strm.avail_out);
            } while (ret != Z_STREAM_END);

            inflateEnd(&strm);
            return out;
        }

        inline std::string merge_target_and_query(const std::string &target, const Params &params)
        {
            if (params.empty())
            {
                return target.empty() ? "/" : target;
            }

            const std::string query = build_query(params);
            const std::string base = target.empty() ? "/" : target;

            if (base.find('?') == std::string::npos)
            {
                return base + "?" + query;
            }

            return base + "&" + query;
        }

        inline std::string resolve_redirect_url(const std::string &base_url, const std::string &location)
        {
            if (location.compare(0, 7, "http://") == 0)
            {
                return location;
            }

            const ParsedUrl parsed = parse_url(base_url);
            const std::string origin = parsed.scheme + "://" + parsed.host + (parsed.port == "80" ? "" : ":" + parsed.port);

            if (!location.empty() && location.front() == '/')
            {
                return origin + location;
            }

            std::string base_target = parsed.target;
            const std::size_t slash_pos = base_target.find_last_of('/');
            const std::string parent = slash_pos == std::string::npos ? "/" : base_target.substr(0, slash_pos + 1);
            return origin + parent + location;
        }

        inline int connect_socket(const ParsedUrl &parsed, int timeout_seconds)
        {
            struct addrinfo hints;
            std::memset(&hints, 0, sizeof(hints));
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;

            struct addrinfo *result = nullptr;
            const int status = getaddrinfo(parsed.host.c_str(), parsed.port.c_str(), &hints, &result);
            if (status != 0)
            {
                throw std::runtime_error("Failed to resolve host: " + parsed.host);
            }

            int sockfd = -1;

            for (struct addrinfo *rp = result; rp != nullptr; rp = rp->ai_next)
            {
                sockfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
                if (sockfd == -1)
                {
                    continue;
                }

                timeval timeout;
                timeout.tv_sec = timeout_seconds;
                timeout.tv_usec = 0;
                setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
                setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

                if (connect(sockfd, rp->ai_addr, rp->ai_addrlen) == 0)
                {
                    freeaddrinfo(result);
                    return sockfd;
                }

                close(sockfd);
                sockfd = -1;
            }

            freeaddrinfo(result);
            throw std::runtime_error("Failed to connect to " + parsed.host + ":" + parsed.port);
        }

        inline void send_all(int sockfd, const std::string &request_text)
        {
            std::size_t sent_total = 0;

            while (sent_total < request_text.size())
            {
                const ssize_t sent_now = send(sockfd, request_text.data() + sent_total, request_text.size() - sent_total, 0);
                if (sent_now <= 0)
                {
                    throw std::runtime_error("Failed to send HTTP request.");
                }
                sent_total += static_cast<std::size_t>(sent_now);
            }
        }

        inline std::string recv_until_close(int sockfd)
        {
            std::string response;
            char buffer[4096];

            while (true)
            {
                const ssize_t received = recv(sockfd, buffer, sizeof(buffer), 0);
                if (received == 0)
                {
                    break;
                }
                if (received < 0)
                {
                    if (errno == EWOULDBLOCK || errno == EAGAIN)
                    {
                        break;
                    }
                    throw std::runtime_error("Failed to receive HTTP response.");
                }

                response.append(buffer, static_cast<std::size_t>(received));
            }

            return response;
        }

        inline std::string decode_chunked_body(const std::string &chunked_body)
        {
            std::string decoded;
            std::size_t position = 0;

            while (position < chunked_body.size())
            {
                const std::size_t line_end = chunked_body.find("\r\n", position);
                if (line_end == std::string::npos)
                {
                    break;
                }

                std::string size_text = chunked_body.substr(position, line_end - position);
                const std::size_t extension_pos = size_text.find(';');
                if (extension_pos != std::string::npos)
                {
                    size_text = size_text.substr(0, extension_pos);
                }

                std::stringstream converter;
                converter << std::hex << size_text;

                std::size_t chunk_size = 0;
                converter >> chunk_size;
                position = line_end + 2;

                if (chunk_size == 0 || position + chunk_size > chunked_body.size())
                {
                    break;
                }

                decoded.append(chunked_body, position, chunk_size);
                position += chunk_size + 2;
            }

            return decoded;
        }

        inline Cookies parse_set_cookie(const std::string &header_value)
        {
            Cookies cookies;
            const std::size_t semicolon_pos = header_value.find(';');
            const std::string first_part = semicolon_pos == std::string::npos ? header_value : header_value.substr(0, semicolon_pos);
            const std::size_t equal_pos = first_part.find('=');

            if (equal_pos == std::string::npos)
            {
                return cookies;
            }

            const std::string key = trim(first_part.substr(0, equal_pos));
            const std::string value = trim(first_part.substr(equal_pos + 1));

            if (!key.empty())
            {
                cookies[key] = value;
            }

            return cookies;
        }

        inline Response parse_response(const std::string &raw_response, const std::string &method, const std::string &url)
        {
            Response response;
            response.method = method;
            response.url = url;

            const std::size_t header_end = raw_response.find("\r\n\r\n");
            if (header_end == std::string::npos)
            {
                response.body = raw_response;
                return response;
            }

            const std::string header_block = raw_response.substr(0, header_end);
            std::string body = raw_response.substr(header_end + 4);
            std::istringstream header_stream(header_block);

            std::string status_line;
            std::getline(header_stream, status_line);
            if (!status_line.empty() && status_line.back() == '\r')
            {
                status_line.pop_back();
            }

            std::istringstream status_parser(status_line);
            std::string http_version;
            status_parser >> http_version;
            status_parser >> response.status_code;
            std::getline(status_parser, response.reason);
            response.reason = trim(response.reason);

            std::string line;
            while (std::getline(header_stream, line))
            {
                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }

                const std::size_t colon_pos = line.find(':');
                if (colon_pos == std::string::npos)
                {
                    continue;
                }

                const std::string key = trim(line.substr(0, colon_pos));
                const std::string value = trim(line.substr(colon_pos + 1));
                response.headers[key] = value;

                if (to_lower(key) == "set-cookie")
                {
                    const Cookies parsed_cookies = parse_set_cookie(value);
                    response.cookies.insert(parsed_cookies.begin(), parsed_cookies.end());
                }
            }

            if (contains_token_case_insensitive(response.header("Transfer-Encoding"), "chunked"))
            {
                body = decode_chunked_body(body);
            }

            if (to_lower(method) == "head")
            {
                body.clear();
            }

            response.body = body;
            return response;
        }

        inline bool is_redirect_status(int status_code)
        {
            return status_code == 301 || status_code == 302 || status_code == 303 || status_code == 307 || status_code == 308;
        }

        inline std::string prepare_body(RequestOptions &options)
        {
            if (!options.body.empty())
            {
                return options.body;
            }

            if (!options.files.empty())
            {
                const std::string boundary = multipart_boundary();
                options.headers["Content-Type"] = "multipart/form-data; boundary=" + boundary;
                return build_multipart_body(options.data, options.files, boundary);
            }

            if (!options.json.empty())
            {
                if (options.headers.find("Content-Type") == options.headers.end())
                {
                    options.headers["Content-Type"] = "application/json";
                }
                return json_dump(options.json);
            }

            if (!options.data.empty())
            {
                if (options.headers.find("Content-Type") == options.headers.end())
                {
                    options.headers["Content-Type"] = "application/x-www-form-urlencoded";
                }
                return build_query(options.data);
            }

            return "";
        }

        inline void sleep_retry_delay(int retry_delay_ms)
        {
            if (retry_delay_ms > 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay_ms));
            }
        }

    } // namespace detail
} // namespace http

#endif
