#include <iostream>
#include "libhttp/libhttp.hpp"

int main()
{
    try
    {
        std::cout << "=== HTTP/2 Example ===" << std::endl;

        std::cout << "\nHTTP/2 support is currently in development." << std::endl;
        std::cout << "The library will automatically upgrade to HTTP/2 when connecting to HTTP/2 servers." << std::endl;

        // Standard request (may upgrade to HTTP/2 automatically)
        http::Response response = http::get("https://httpbin.org/get");
        std::cout << "\nRequest completed. Status: " << response.status_code << std::endl;

        std::cout << "\nHTTP/2 example completed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
