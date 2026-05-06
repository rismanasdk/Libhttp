#ifndef LIBHTTP_REQUEST_H
#define LIBHTTP_REQUEST_H

#include <map>
#include <string>

namespace http {

struct RequestOptions {
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
    std::map<std::string, std::string> data;
    std::string body;
    int timeout_seconds = 10;
};

}  // namespace http

#endif
