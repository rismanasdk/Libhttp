# LibHTTP v3.0.0 - Test Suite Documentation

## Overview

LibHTTP v3.0.0 includes a comprehensive test suite designed to validate HTTP/1.1 functionality and ensure production readiness. All tests are **offline** (require no network connectivity) and focus on core functionality validation.

## Running Tests

### Standard Build (HTTP/1.1 only)

```bash
mkdir build && cd build
cmake .. -DLIBHTTP_BUILD_TESTS=ON
make
./tests/test_core
```

### Build with HTTP/2 Support (Experimental)

```bash
mkdir build && cd build
cmake .. -DLIBHTTP_BUILD_TESTS=ON -DLIBHTTP_ENABLE_HTTP2=ON
make
```

### Build with WebSocket Support (Experimental)

```bash
mkdir build && cd build
cmake .. -DLIBHTTP_BUILD_TESTS=ON -DLIBHTTP_ENABLE_WEBSOCKET=ON
make
```

### Build with All Features

```bash
mkdir build && cd build
cmake .. -DLIBHTTP_BUILD_TESTS=ON -DLIBHTTP_ENABLE_HTTP2=ON -DLIBHTTP_ENABLE_WEBSOCKET=ON
make
```

## Test Categories

### 1. Request Options Tests

- `RequestOptions creation` - Validates default option values (10s timeout, 5 redirects, no retries, SSL verification enabled)

### 2. Response Status Tests

- `Response status helpers` - Tests status code classification methods:
  - `ok()` for 2xx responses
  - `is_success()` for 2xx responses
  - `is_error()` for 4xx/5xx responses
  - `is_client_error()` for 4xx responses
  - `is_server_error()` for 5xx responses
  - `is_informational()` for 1xx responses
  - `is_redirect()` for 3xx responses

### 3. Response Content Tests

- `Response header access` - Tests header access with case-insensitivity
- `Response content type detection` - Tests MIME type detection (JSON, HTML, plain text)
- `Response text content access` - Tests text content and line splitting
- `Response binary content access` - Tests binary content handling
- `Response cookie access` - Tests cookie retrieval from response

### 4. Response Metadata Tests

- `Response elapsed time tracking` - Tests request duration recording
- `Redirect detection` - Tests 301/302/307 status detection

### 5. Session Tests

- `Session creation and base URL` - Tests session initialization with base URL
- `Session default headers` - Tests header persistence across requests

### 6. Configuration Tests

- `Proxy URL parsing` - Tests proxy configuration parsing for:
  - HTTP proxies with custom ports
  - HTTPS proxies with authentication
  - SOCKS5 proxies with default ports
  - Protocol detection

### 7. Exception Tests

- `Custom exceptions` - Tests all custom exception types:
  - `ConnectionException` for connection failures
  - `TimeoutException` for request timeouts
  - `BadStatusException` for HTTP error responses with status code tracking
  - Proper exception inheritance from `std::runtime_error`

## Test Results Format

### Success Output

```
=== LibHTTP v3.0.0 - Unit Tests ===
(No network required - offline tests only)

Testing: RequestOptions creation... ✓ PASS
Testing: Response status helpers... ✓ PASS
...

=== Test Summary ===
✓ Passed: 13
✗ Failed: 0

✅ All tests passed!
```

### Failure Output

If any test fails, it will show:

```
Testing: <test name>... ✗ FAIL: <error message>

=== Test Summary ===
✓ Passed: 12
✗ Failed: 1

❌ Some tests failed!
```

## Test Coverage

The current test suite covers:

| Component           | Coverage                                                                      |
| ------------------- | ----------------------------------------------------------------------------- |
| Request Options     | ✅ Default values, timeout, redirects, retries, SSL                           |
| Response            | ✅ Status codes, headers, content types, text/binary content, cookies, timing |
| Session             | ✅ Creation, base URL, header management                                      |
| Proxy Configuration | ✅ URL parsing, authentication, protocol detection                            |
| Exceptions          | ✅ All custom exception types and inheritance                                 |
| Feature Flags       | ✅ Conditional compilation of HTTP/2 and WebSocket                            |

## Missing Network-Based Tests (v3.1.0+)

Future releases will include integration tests for:

- Actual HTTP requests to test servers
- SSL/TLS certificate validation
- Compression (gzip/deflate) handling
- HTTP redirects (301/302/307)
- Cookie persistence
- Authentication flows
- Streaming downloads
- Connection pooling and keep-alive

## Building Without Tests

To skip test building:

```bash
mkdir build && cd build
cmake .. -DLIBHTTP_BUILD_TESTS=OFF
make
```

## CI/CD Integration

For CI/CD pipelines, use:

```bash
cmake .. -DLIBHTTP_BUILD_TESTS=ON \
         -DLIBHTTP_ENABLE_HTTP2=ON \
         -DLIBHTTP_ENABLE_WEBSOCKET=ON
make
./tests/test_core
```

The test executable returns:

- Exit code `0` if all tests pass
- Exit code `1` if any test fails

## Extending the Test Suite

To add new tests:

1. Create a new test function in `tests/test_core.cpp`:

   ```cpp
   void test_my_feature()
   {
       TEST("My feature description");
       try {
           // Test code here
           assert(condition);
           PASS();
       } catch (const std::exception &e) {
           FAIL(e.what());
       }
   }
   ```

2. Call the test in `main()`:

   ```cpp
   tests::test_my_feature();
   ```

3. Rebuild and run tests

## Performance Notes

- All tests complete in < 100ms
- No external dependencies required
- No network latency
- Suitable for CI/CD pipelines

## Troubleshooting

### Build Fails with Missing OpenSSL

```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# macOS
brew install openssl
```

### Build Fails with Missing ZLIB

```bash
# Ubuntu/Debian
sudo apt-get install zlib1g-dev

# macOS
brew install zlib
```

### Tests Not Found After Build

Ensure tests are enabled:

```bash
cmake .. -DLIBHTTP_BUILD_TESTS=ON
```

## Version Info

- **LibHTTP Version**: 3.0.0
- **Test Suite Version**: 1.0
- **C++ Standard**: C++11 or later
- **Supported Platforms**: Linux, macOS, Windows (with appropriate compilers)

---

Last Updated: v3.0.0 Stabilization Release
