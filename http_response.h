#ifndef LIBHTTP_RESPONSE_H
#define LIBHTTP_RESPONSE_H

#include <map>
#include <string>
#include <vector>

namespace http {

class Response {
   public:
    int status_code = 0;
    std::string reason;
    std::map<std::string, std::string> headers;
    std::string body;
    std::string url;
    std::string method;

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
        const auto it = headers.find(key);
        return it == headers.end() ? "" : it->second;
    }
};

}  // namespace http

#endif
