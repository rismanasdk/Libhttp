#include <iostream>
#include <cassert>
#include <stdexcept>
#include "libhttp/libhttp.hpp"

// Test helpers
int tests_passed = 0;
int tests_failed = 0;

#define TEST(name) std::cout << "Testing: " << name << "... "
#define PASS()                          \
    std::cout << "✓ PASS" << std::endl; \
    tests_passed++
#define FAIL(msg)                                \
    std::cout << "✗ FAIL: " << msg << std::endl; \
    tests_failed++

// Unit tests that don't require network
namespace tests
{
    void test_request_options_creation()
    {
        TEST("RequestOptions creation");
        try
        {
            http::RequestOptions opts;
            assert(opts.timeout_seconds == 10);
            assert(opts.allow_redirects == true);
            assert(opts.max_redirects == 5);
            assert(opts.retry_count == 0);
            assert(opts.verify_ssl == true);
            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_response_status_helpers()
    {
        TEST("Response status helpers");
        try
        {
            http::Response resp;

            resp.status_code = 200;
            assert(resp.ok() == true);
            assert(resp.is_success() == true);
            assert(resp.is_error() == false);

            resp.status_code = 404;
            assert(resp.ok() == false);
            assert(resp.is_client_error() == true);
            assert(resp.is_error() == true);

            resp.status_code = 500;
            assert(resp.is_server_error() == true);
            assert(resp.is_error() == true);

            resp.status_code = 100;
            assert(resp.is_informational() == true);

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_response_headers()
    {
        TEST("Response header access");
        try
        {
            http::Response resp;
            resp.headers["Content-Type"] = "application/json";
            resp.headers["X-Custom-Header"] = "value";

            assert(resp.header("Content-Type") == "application/json");
            assert(resp.header("content-type") == "application/json"); // case-insensitive
            assert(resp.has_header("X-Custom-Header") == true);
            assert(resp.has_header("Non-Existent") == false);

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_response_content_type()
    {
        TEST("Response content type detection");
        try
        {
            http::Response resp;

            resp.headers["Content-Type"] = "application/json; charset=utf-8";
            assert(resp.content_type() == "application/json");
            assert(resp.is_json() == true);
            assert(resp.is_html() == false);

            resp.headers["Content-Type"] = "text/html; charset=utf-8";
            assert(resp.is_html() == true);
            assert(resp.is_json() == false);

            resp.headers["Content-Type"] = "text/plain";
            assert(resp.is_text() == true);

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_response_text_content()
    {
        TEST("Response text content access");
        try
        {
            http::Response resp;
            resp.body = "Line 1\nLine 2\nLine 3";

            std::string text = resp.text();
            assert(text == "Line 1\nLine 2\nLine 3");

            std::vector<std::string> lines = resp.lines();
            assert(lines.size() == 3);
            assert(lines[0] == "Line 1");
            assert(lines[1] == "Line 2");
            assert(lines[2] == "Line 3");

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_response_binary_content()
    {
        TEST("Response binary content access");
        try
        {
            http::Response resp;
            resp.body = "binarydata";

            std::vector<char> content = resp.content();
            assert(content.size() == resp.body.size());
            assert(content[0] == 'b');

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_response_cookies()
    {
        TEST("Response cookie access");
        try
        {
            http::Response resp;
            resp.cookies["session_id"] = "abc123";
            resp.cookies["user_pref"] = "dark_mode";

            assert(resp.cookie("session_id") == "abc123");
            assert(resp.cookie("user_pref") == "dark_mode");
            assert(resp.cookie("non_existent").empty());

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_session_creation()
    {
        TEST("Session creation and base URL");
        try
        {
            http::Session session("https://api.example.com");
            assert(session.get_base_url() == "https://api.example.com");

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_session_headers()
    {
        TEST("Session default headers");
        try
        {
            http::Session session("https://api.example.com");
            session.set_header("Authorization", "Bearer token");
            session.set_header("X-API-Key", "secret");

            // Headers should persist in session
            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_proxy_config_parsing()
    {
        TEST("Proxy URL parsing");
        try
        {
            http::ProxyConfig proxy_http("http://proxy.example.com:8080");
            assert(proxy_http.protocol == "http");
            assert(proxy_http.host == "proxy.example.com");
            assert(proxy_http.port == "8080");
            assert(proxy_http.use_proxy == true);

            http::ProxyConfig proxy_https("https://user:pass@proxy.example.com:3128");
            assert(proxy_https.protocol == "https");
            assert(proxy_https.username == "user");
            assert(proxy_https.password == "pass");
            assert(proxy_https.host == "proxy.example.com");
            assert(proxy_https.port == "3128");

            http::ProxyConfig proxy_socks("socks5://proxy.example.com");
            assert(proxy_socks.protocol == "socks5");
            assert(proxy_socks.port == "1080"); // default SOCKS port

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_exceptions()
    {
        TEST("Custom exceptions");
        try
        {
            // Test ConnectionException
            try
            {
                throw http::ConnectionException("Failed to connect");
            }
            catch (const http::ConnectionException &e)
            {
                std::string msg = e.what();
                assert(msg.find("Connection error") != std::string::npos);
            }

            // Test TimeoutException
            try
            {
                throw http::TimeoutException("Request timeout");
            }
            catch (const http::TimeoutException &e)
            {
                std::string msg = e.what();
                assert(msg.find("Timeout error") != std::string::npos);
            }

            // Test BadStatusException
            try
            {
                throw http::BadStatusException(404, "Not Found");
            }
            catch (const http::BadStatusException &e)
            {
                assert(e.status_code() == 404);
                assert(e.reason() == "Not Found");
            }

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_response_elapsed_time()
    {
        TEST("Response elapsed time tracking");
        try
        {
            http::Response resp;
            resp.elapsed = 0.250;

            assert(resp.elapsed > 0.0);
            assert(resp.elapsed < 1.0);

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }

    void test_redirect_detection()
    {
        TEST("Redirect status detection");
        try
        {
            http::Response resp;

            resp.status_code = 301;
            assert(resp.is_redirect() == true);

            resp.status_code = 302;
            assert(resp.is_redirect() == true);

            resp.status_code = 307;
            assert(resp.is_redirect() == true);

            resp.status_code = 200;
            assert(resp.is_redirect() == false);

            PASS();
        }
        catch (const std::exception &e)
        {
            FAIL(e.what());
        }
    }
}

int main()
{
    std::cout << "\n=== LibHTTP v3.0.0 - Unit Tests ===" << std::endl;
    std::cout << "(No network required - offline tests only)\n"
              << std::endl;

    // Request options tests
    tests::test_request_options_creation();

    // Response tests
    tests::test_response_status_helpers();
    tests::test_response_headers();
    tests::test_response_content_type();
    tests::test_response_text_content();
    tests::test_response_binary_content();
    tests::test_response_cookies();
    tests::test_response_elapsed_time();
    tests::test_redirect_detection();

    // Session tests
    tests::test_session_creation();
    tests::test_session_headers();

    // Configuration tests
    tests::test_proxy_config_parsing();

    // Exception tests
    tests::test_exceptions();

    // Summary
    std::cout << "\n=== Test Summary ===" << std::endl;
    std::cout << "✓ Passed: " << tests_passed << std::endl;
    std::cout << "✗ Failed: " << tests_failed << std::endl;

    if (tests_failed == 0)
    {
        std::cout << "\n✅ All tests passed!" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "\n❌ Some tests failed!" << std::endl;
        return 1;
    }
}
