#ifndef LIBHTTP_REQUEST_H
#define LIBHTTP_REQUEST_H

#include <map>
#include <string>

namespace http {

using Headers = std::map<std::string, std::string>;
using Params = std::map<std::string, std::string>;
using Cookies = std::map<std::string, std::string>;
using Json = std::map<std::string, std::string>;
using Files = std::map<std::string, std::string>;

struct RequestOptions {
    Headers headers;
    Params params;
    Params data;
    Json json;
    Files files;
    Cookies cookies;
    std::string body;
    std::string auth_username;
    std::string auth_password;
    int timeout_seconds = 10;
    bool allow_redirects = true;
    int max_redirects = 5;
    int retry_count = 0;
    int retry_delay_ms = 0;
};

}  // namespace http

#endif
