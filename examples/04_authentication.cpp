#include <iostream>
#include "libhttp/libhttp.hpp"

int main()
{
    try
    {
        std::cout << "=== Authentication Example ===" << std::endl;

        http::RequestOptions opts;
        opts.auth_type = "bearer";
        opts.auth_token = "your-token-here";

        std::cout << "\n1. Bearer token authentication:" << std::endl;
        std::cout << "Bearer token will be sent in Authorization header" << std::endl;

        // Note: This would make a real request, so we just show the setup
        // http::Response response = http::get("https://api.example.com/protected", opts);

        std::cout << "\nAuthentication example completed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
