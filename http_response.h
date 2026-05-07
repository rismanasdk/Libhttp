#ifndef LIBHTTP_RESPONSE_H
#define LIBHTTP_RESPONSE_H

#include <algorithm>
#include <cctype>
#include <chrono>
#include <sstream>
#include <string>
#include <vector>
#include <functional>

#include "http_request.h"

namespace http
{

    class Response
    {
    private:
        static std::string to_lower_copy(std::string value)
        {
            std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch)
                           { return static_cast<char>(std::tolower(ch)); });
            return value;
        }

    public:
        int status_code = 0;
        std::string reason;
        Headers headers;
        Cookies cookies;
        std::string body;
        std::string url;
        std::string method;
        std::vector<std::string> history;
        double elapsed = 0.0; // request duration in seconds

        bool ok() const
        {
            return status_code >= 200 && status_code < 300;
        }

        std::string text() const
        {
            return body;
        }

        std::vector<char> content() const
        {
            return std::vector<char>(body.begin(), body.end());
        }

        std::vector<std::string> lines() const
        {
            std::vector<std::string> result;
            std::istringstream stream(body);
            std::string line;

            while (std::getline(stream, line))
            {
                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }
                result.push_back(line);
            }

            return result;
        }

        std::string header(const std::string &key) const
        {
            const auto exact = headers.find(key);
            if (exact != headers.end())
            {
                return exact->second;
            }

            const std::string key_lower = to_lower_copy(key);
            for (const auto &entry : headers)
            {
                if (to_lower_copy(entry.first) == key_lower)
                {
                    return entry.second;
                }
            }

            return "";
        }

        bool has_header(const std::string &key) const
        {
            return !header(key).empty();
        }

        std::string cookie(const std::string &key) const
        {
            const auto exact = cookies.find(key);
            if (exact != cookies.end())
            {
                return exact->second;
            }

            return "";
        }

        bool is_redirect() const
        {
            return status_code >= 300 && status_code < 400;
        }

        bool redirected() const
        {
            return !history.empty();
        }

        Json json() const
        {
            Json result;
            std::string text_body = body;

            const auto first = text_body.find('{');
            const auto last = text_body.rfind('}');
            if (first == std::string::npos || last == std::string::npos || last <= first)
            {
                return result;
            }

            text_body = text_body.substr(first + 1, last - first - 1);
            std::stringstream stream(text_body);
            std::string pair;

            while (std::getline(stream, pair, ','))
            {
                const std::size_t colon_pos = pair.find(':');
                if (colon_pos == std::string::npos)
                {
                    continue;
                }

                std::string key = pair.substr(0, colon_pos);
                std::string value = pair.substr(colon_pos + 1);

                auto trim_json = [](std::string text)
                {
                    text.erase(text.begin(), std::find_if(text.begin(), text.end(), [](unsigned char ch)
                                                          { return !std::isspace(ch); }));
                    text.erase(std::find_if(text.rbegin(), text.rend(), [](unsigned char ch)
                                            { return !std::isspace(ch); })
                                   .base(),
                               text.end());

                    if (!text.empty() && text.front() == '"')
                    {
                        text.erase(text.begin());
                    }
                    if (!text.empty() && text.back() == '"')
                    {
                        text.pop_back();
                    }

                    return text;
                };

                result[trim_json(key)] = trim_json(value);
            }

            return result;
        }

        bool is_success() const
        {
            return status_code >= 200 && status_code < 300;
        }

        bool is_error() const
        {
            return status_code >= 400;
        }

        bool is_client_error() const
        {
            return status_code >= 400 && status_code < 500;
        }

        bool is_server_error() const
        {
            return status_code >= 500;
        }

        bool is_informational() const
        {
            return status_code >= 100 && status_code < 200;
        }

        size_t content_length() const
        {
            const std::string length_str = header("Content-Length");
            if (length_str.empty())
            {
                return body.length();
            }
            try
            {
                return std::stoul(length_str);
            }
            catch (...)
            {
                return body.length();
            }
        }

        std::string content_type() const
        {
            std::string ct = header("Content-Type");
            const std::size_t semicolon_pos = ct.find(';');
            if (semicolon_pos != std::string::npos)
            {
                ct = ct.substr(0, semicolon_pos);
            }
            ct.erase(ct.find_last_not_of(" \t\r\n") + 1);
            ct.erase(0, ct.find_first_not_of(" \t\r\n"));
            return ct;
        }

        bool is_json() const
        {
            const std::string ct = content_type();
            return ct.find("application/json") != std::string::npos || ct.find("text/json") != std::string::npos;
        }

        bool is_html() const
        {
            const std::string ct = content_type();
            return ct.find("text/html") != std::string::npos;
        }

        bool is_text() const
        {
            const std::string ct = content_type();
            return ct.find("text/") != std::string::npos;
        }

        void raise_for_status() const
        {
            if (is_error())
            {
                throw std::runtime_error("HTTP " + std::to_string(status_code) + " " + reason);
            }
        }

        // Iterate over response body in chunks
        void iter_content(size_t chunk_size, const std::function<bool(const std::string &)> &processor) const
        {
            if (body.empty())
            {
                return;
            }
            for (size_t i = 0; i < body.size(); i += chunk_size)
            {
                const size_t len = std::min(chunk_size, body.size() - i);
                if (!processor(body.substr(i, len)))
                {
                    break;
                }
            }
        }

        // Iterate over response body line by line
        void iter_lines(const std::function<bool(const std::string &)> &processor) const
        {
            std::istringstream stream(body);
            std::string line;
            while (std::getline(stream, line))
            {
                if (!line.empty() && line.back() == '\r')
                {
                    line.pop_back();
                }
                if (!processor(line))
                {
                    break;
                }
            }
        }
    };

} // namespace http

#endif
