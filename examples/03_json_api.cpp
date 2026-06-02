#include <iostream>
#include "libhttp/libhttp.hpp"

int main()
{
    try
    {
        std::cout << "=== JSON API Example ===" << std::endl;

        http::RequestOptions opts;
        opts.headers["Content-Type"] = "application/json";

        std::cout << "\n1. GET JSON response:" << std::endl;
        http::Response response = http::get("https://httpbin.org/json", opts);
        std::cout << "Status: " << response.status_code << std::endl;
        std::cout << "Is JSON: " << (response.is_json() ? "Yes" : "No") << std::endl;

        std::cout << "\nJSON API example completed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
