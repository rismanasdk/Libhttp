#include <iostream>
#include "libhttp/libhttp.hpp"

int main()
{
    try
    {
        std::cout << "=== Simple GET Request ===" << std::endl;

        http::Response response = http::get("https://httpbin.org/get");

        std::cout << "Status Code: " << response.status_code << std::endl;
        std::cout << "Reason: " << response.reason << std::endl;
        std::cout << "Content-Type: " << response.content_type() << std::endl;
        std::cout << "\nResponse Body (first 500 chars):" << std::endl;
        std::cout << response.body.substr(0, 500) << "..." << std::endl;

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
