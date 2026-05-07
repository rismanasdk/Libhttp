#include <iostream>
#include <exception>
#include "http.h"

int main() {
    http::Session session("http://example.com");
    session.set_header("User-Agent", "requests-like-demo/1.0");
    session.set_cookie("visitor", "true");

    http::RequestOptions options;
    options.params["page"] = "1";
    options.allow_redirects = true;

    try {
        http::Response response = session.get("/", options);
        std::cout << response.status_code << '\n';
        std::cout << response.reason << '\n';
        std::cout << response.text() << '\n';
    } catch (const std::exception& error) {
        std::cout << "Request failed: " << error.what() << '\n';
    }
    return 0;
}
