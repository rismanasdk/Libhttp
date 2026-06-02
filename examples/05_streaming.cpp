#include <iostream>
#include "libhttp/libhttp.hpp"

int main()
{
    try
    {
        std::cout << "=== Streaming Example ===" << std::endl;

        http::RequestOptions opts;
        opts.stream = true;

        std::cout << "\n1. Streaming download with progress:" << std::endl;

        // Example of progress callback
        http::RequestOptions stream_opts;
        stream_opts.progress_callback = [](size_t current, size_t total)
        {
            std::cout << "Downloaded: " << current << " / " << total << " bytes" << std::endl;
        };

        http::Response response = http::get("https://httpbin.org/get", stream_opts);
        std::cout << "Download completed. Status: " << response.status_code << std::endl;

        std::cout << "\nStreaming example completed successfully!" << std::endl;
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
