#include <iostream>
#include "http.h"

int main() {
    http::Response req = http::get("https://httpbin.org/get");
    std::cout << "Status code: " << req.status_code << std::endl;
    std::cout << "Response body: " << req.text() << std::endl;
    std::cout << "Body:     " << req.body << std::endl;
    return 0;
}