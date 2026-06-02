#include <iostream>
#include "libhttp/libhttp.hpp"

int main()
{
    try
    {
        std::cout << "=== Session Example ===" << std::endl;

        http::Session session("https://httpbin.org");
        session.set_header("User-Agent", "libhttp/3.0");

        std::cout << "\n1. GET request with session:" << std::endl;
        http::Response res1 = session.get("/get");
        std::cout << "Status: " << res1.status_code << std::endl;

        std::cout << "\n2. Another GET request (session reuses headers):" << std::endl;
        http::Response res2 = session.get("/get?param=value");
        std::cout << "Status: " << res2.status_code << std::endl;

        std::cout << "\nSession examples completed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
