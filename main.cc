#include <iostream>
#include <exception>
#include "http.h"
#include "http_status.h"
#include "http_json.h"

int main()
{
    try
    {
        // Example 1: Basic GET with status checking
        http::Session session("http://httpbin.org");
        session.set_header("User-Agent", "libhttp/2.0");

        http::RequestOptions options;
        options.params["page"] = "1";
        options.allow_redirects = true;
        options.retry_count = 1;
        options.retry_delay_ms = 100;

        http::Response response = session.get("/get", options);

        std::cout << "=== Basic Request ===" << std::endl;
        std::cout << "Status: " << response.status_code << " - " << response.reason << std::endl;
        std::cout << "Success: " << (response.is_success() ? "Yes" : "No") << std::endl;
        std::cout << "Content-Type: " << response.content_type() << std::endl;
        std::cout << std::endl;

        http::Response req = http::get("http://httpbin.org");
        std::cout << "Status code: " << req.status_code << std::endl;
        std::cout << "Body: " req.text() << std:endl;
        std::cout << "Body: " << req.

        // Example 2: Bearer Token Auth
        std::cout << "=== Bearer Token Auth ===" << std::endl;
        session.set_bearer_auth("your-api-token-here");
        std::cout << "Bearer auth header set" << std::endl;
        std::cout << std::endl;

        // Example 3: API Key Auth
        std::cout << "=== API Key Auth ===" << std::endl;
        session.set_api_key("sk-1234567890", "X-API-Key");
        std::cout << "API Key set to header: X-API-Key" << std::endl;
        std::cout << std::endl;

        // Example 4: Proxy Configuration
        std::cout << "=== Proxy Configuration ===" << std::endl;
        session.set_proxy("proxy.example.com", "8080");
        std::cout << "Proxy configured: proxy.example.com:8080" << std::endl;
        session.disable_proxy();
        std::cout << "Proxy disabled" << std::endl;
        std::cout << std::endl;

        // Example 5: Better Status Codes
        std::cout << "=== Status Code Helpers ===" << std::endl;
        std::cout << "Status " << http::status::NOT_FOUND << " = " << http::status::reason_phrase(404) << std::endl;
        std::cout << "Status " << http::status::OK << " = " << http::status::reason_phrase(200) << std::endl;
        std::cout << "Status " << http::status::INTERNAL_SERVER_ERROR << " = "
                  << http::status::reason_phrase(500) << std::endl;
        std::cout << std::endl;

        // Example 6: Response Content Type Checks
        std::cout << "=== Content Type Checks ===" << std::endl;
        response = session.get("/json", options);
        std::cout << "Is JSON: " << (response.is_json() ? "Yes" : "No") << std::endl;
        std::cout << "Is HTML: " << (response.is_html() ? "Yes" : "No") << std::endl;
        std::cout << "Is Text: " << (response.is_text() ? "Yes" : "No") << std::endl;
        std::cout << "Content Length: " << response.content_length() << " bytes" << std::endl;
        std::cout << std::endl;

        // Example 7: JSON Parsing
        std::cout << "=== JSON Parsing ===" << std::endl;
        if (response.is_json())
        {
            http::json::Value parsed = http::json::Value::parse(response.body);
            std::string json_str = parsed.dump(2); // Pretty print with indent
            std::cout << "Parsed JSON (first 200 chars):" << std::endl;
            std::cout << json_str.substr(0, 200) << "..." << std::endl;
        }
        std::cout << std::endl;

        // Example 8: Download with Progress
        std::cout << "=== Streaming Download ===" << std::endl;
        auto progress = [](size_t current, size_t total)
        {
            if (total > 0)
            {
                int percent = (current * 100) / total;
                std::cout << "Download progress: " << percent << "% (" << current << "/" << total << " bytes)" << std::endl;
            }
        };

        bool download_ok = session.stream_download(
            "/image/png",
            "/tmp/test-image.png",
            progress,
            options);
        std::cout << "Download result: " << (download_ok ? "Success" : "Failed") << std::endl;
        std::cout << std::endl;

        // Example 9: Custom Auth
        std::cout << "=== Custom Auth ===" << std::endl;
        session.set_auth_type("Digest", "username=user,realm=test");
        std::cout << "Custom auth set: Digest username=user,realm=test" << std::endl;
        std::cout << std::endl;

        // Example 10: Streaming Response Content
        std::cout << "=== Streaming Response (iter_content) ===" << std::endl;
        response = session.get("/json", options);
        size_t chunk_count = 0;
        response.iter_content(100, [&chunk_count](const std::string &chunk)
                              {
            chunk_count++;
            if (chunk_count <= 3)
            {
                std::cout << "Chunk " << chunk_count << " size: " << chunk.size() << " bytes" << std::endl;
            }
            return true; });
        std::cout << "Total chunks: " << chunk_count << std::endl;
        std::cout << std::endl;

        // Example 11: Streaming Lines
        std::cout << "=== Streaming Lines (iter_lines) ===" << std::endl;
        response = session.get("/json", options);
        size_t line_count = 0;
        response.iter_lines([&line_count](const std::string &line)
                            {
            line_count++;
            if (line_count <= 3)
            {
                std::cout << "Line " << line_count << ": " << (line.size() > 50 ? line.substr(0, 50) + "..." : line) << std::endl;
            }
            return true; });
        std::cout << "Total lines: " << line_count << std::endl;
        std::cout << std::endl;

        // Example 12: Request Timing
        std::cout << "=== Request Timing (elapsed) ===" << std::endl;
        response = session.get("/json", options);
        std::cout << "Request elapsed time: " << response.elapsed << " seconds" << std::endl;
        std::cout << std::endl;

        // Example 10: Error Handling
        std::cout << "=== Error Handling ===" << std::endl;
        try
        {
            http::Response error_resp = session.get("/status/404", options);
            if (error_resp.is_error())
            {
                std::cout << "Got error status: " << error_resp.status_code << std::endl;
            }
            // This would throw: error_resp.raise_for_status();
        }
        catch (const std::exception &e)
        {
            std::cout << "Caught exception: " << e.what() << std::endl;
        }
    }
    catch (const std::exception &error)
    {
        std::cout << "Request failed: " << error.what() << '\n';
    }
    return 0;
}
