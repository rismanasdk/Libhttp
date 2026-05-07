#ifndef LIBHTTP_RESPONSE_H
#define LIBHTTP_RESPONSE_H

#include <algorithm>
#include <cctype>
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
};

}  // namespace http

#endif
