#include <iostream>
#include <exception>
#include "http.h"

int main() {
    http::RequestOptions options;
    options.params["page"] = "1";
    options.headers["Accept"] = "text/html";

    http::Client client("http://example.com");
    client.set_header("User-Agent", "requests-like-demo/1.0");

    try {
        http::Response response = client.get("/", options);
        std::cout << response.status_code << '\n';
        std::cout << response.reason << '\n';
        std::cout << response.text() << '\n';
    } catch (const std::exception& error) {
        std::cout << "Request failed: " << error.what() << '\n';
    }
    return 0;
}
