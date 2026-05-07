#ifndef LIBHTTP_RESPONSE_H
#define LIBHTTP_RESPONSE_H

#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "http_request.h"

namespace http {

class Response {
   private:
    static std::string to_lower_copy(std::string value) {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });
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

    bool ok() const {
        return status_code >= 200 && status_code < 300;
    }

    std::string text() const {
        return body;
    }

    std::vector<char> content() const {
        return std::vector<char>(body.begin(), body.end());
    }

    std::vector<std::string> lines() const {
        std::vector<std::string> result;
        std::istringstream stream(body);
        std::string line;

        while (std::getline(stream, line)) {
            if (!line.empty() && line.back() == '\r') {
                line.pop_back();
            }
            result.push_back(line);
        }

        return result;
    }

    std::string header(const std::string& key) const {
        const auto exact = headers.find(key);
        if (exact != headers.end()) {
            return exact->second;
        }

        const std::string key_lower = to_lower_copy(key);
        for (const auto& entry : headers) {
            if (to_lower_copy(entry.first) == key_lower) {
                return entry.second;
            }
        }

        return "";
    }

    bool has_header(const std::string& key) const {
        return !header(key).empty();
    }

    std::string cookie(const std::string& key) const {
        const auto exact = cookies.find(key);
        if (exact != cookies.end()) {
            return exact->second;
        }

        return "";
    }

    bool is_redirect() const {
        return status_code >= 300 && status_code < 400;
    }

    bool redirected() const {
        return !history.empty();
    }

    Json json() const {
        Json result;
        std::string text_body = body;

        const auto first = text_body.find('{');
        const auto last = text_body.rfind('}');
        if (first == std::string::npos || last == std::string::npos || last <= first) {
            return result;
        }

        text_body = text_body.substr(first + 1, last - first - 1);
        std::stringstream stream(text_body);
        std::string pair;

        while (std::getline(stream, pair, ',')) {
            const std::size_t colon_pos = pair.find(':');
            if (colon_pos == std::string::npos) {
                continue;
            }

            std::string key = pair.substr(0, colon_pos);
            std::string value = pair.substr(colon_pos + 1);

            auto trim_json = [](std::string text) {
                text.erase(text.begin(), std::find_if(text.begin(), text.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
                text.erase(std::find_if(text.rbegin(), text.rend(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }).base(), text.end());

                if (!text.empty() && text.front() == '"') {
                    text.erase(text.begin());
                }
                if (!text.empty() && text.back() == '"') {
                    text.pop_back();
                }

                return text;
            };

            result[trim_json(key)] = trim_json(value);
        }

        return result;
    }
};

}  // namespace http

#endif
