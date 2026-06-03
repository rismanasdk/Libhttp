# LibHTTP v3.0.0

LibHTTP is a **header-only C++ HTTP/1.1 client library** for `http://` and `https://` requests, designed to be intuitive like Python's `requests` library while maintaining simplicity and performance.

**Version 3.0.0** is the stable, production-ready release with professional architecture, comprehensive testing, and optional experimental HTTP/2/WebSocket support.

**Mission**: Bring Python `requests`-like simplicity to C++ HTTP clients with clear, understandable code architecture.

**Note**: HTTP/2 and WebSocket are experimental and disabled by default. See [Feature Flags](#feature-flags) to enable them.

## Current Progress

### Target 1 (Completed)

1. free request functions such as `get()`, `post()`, `put()`, `delete_()`, `options()`, `head()`, and `patch()`
2. a reusable `Session` object
3. query params, headers, form data, raw body, and JSON body
4. redirect handling
5. cookie handling

### Target 2 (Completed)

1. basic auth
2. multipart file upload
3. retry options
4. simple JSON parsing from response body
5. download helper

### Target 3 - v2.0 (Completed)

1. **proxy support** with authentication (HTTP/HTTPS/SOCKS)
2. **streaming download** with progress callbacks
3. **enhanced JSON support** with full parser and pretty printing
4. **status code helpers** and custom exceptions
5. **advanced authentication** (Bearer, API Key, Custom, OAuth2)
6. **HTTPS/TLS support** with automatic certificate handling
7. **automatic compression decompression** (gzip, deflate)
8. **streaming response iteration** (iter_content, iter_lines)
9. **request timing** (elapsed time tracking)

### Target 4 - v3.0 (Completed) 

1. **HTTP/2 support** with frame-based multiplexing and stream management
2. **WebSocket support** with frame handling and message callbacks
3. **Connection pooling** with keep-alive and automatic cleanup
4. **Thread-safe operations** for concurrent access

## Project Structure (v3.0.0)

LibHTTP is professionally organized with headers in logical categories:

### Include Paths

```
include/libhttp/
├── libhttp.hpp              # Main entry point - include this!
├── types.h                  # Shared types (Headers, Cookies, etc.)
├── core/
│   ├── request.h            # RequestOptions struct
│   ├── response.h           # Response class with status/header/cookie methods
│   ├── session.h            # Session class for persistent connections
│   └── detail.h             # Internal utilities and low-level implementations
├── features/
│   ├── auth.h               # Authentication helpers (Basic, Bearer, Digest, OAuth2)
│   ├── json.h               # JSON parsing and serialization
│   ├── stream.h             # Streaming download with progress callbacks
│   ├── http2.h              # HTTP/2 support (EXPERIMENTAL, opt-in via flag)
│   └── websocket.h          # WebSocket support (EXPERIMENTAL, opt-in via flag)
└── utils/
    ├── exceptions.h         # Custom exception types
    ├── status.h             # HTTP status code constants and helpers
    └── connection_pool.h    # Connection pooling for keep-alive
```

### Simple Usage

```cpp
#include "libhttp/libhttp.hpp"

// Just include this one header for everything!
// HTTP/2 and WebSocket are automatically excluded unless built with feature flags
```

---

## Feature Flags

Control experimental features at compile-time:

### Default Build (HTTP/1.1 Production-Ready)

```bash
mkdir build && cd build
cmake ..
make
```

### Enable HTTP/2 (Experimental)

```bash
cmake .. -DLIBHTTP_ENABLE_HTTP2=ON
```

### Enable WebSocket (Experimental)

```bash
cmake .. -DLIBHTTP_ENABLE_WEBSOCKET=ON
```

### Enable All Features

```bash
cmake .. -DLIBHTTP_ENABLE_HTTP2=ON -DLIBHTTP_ENABLE_WEBSOCKET=ON
```

### Build with Tests

```bash
cmake .. -DLIBHTTP_BUILD_TESTS=ON
```

**Note**: HTTP/2 and WebSocket are **not recommended for production use** in v3.0.0. They have incomplete implementations and are provided for experimental use only. For production applications, use the default HTTP/1.1 configuration.

---

## Installation

### Option 1: Using CMake (Recommended)

```bash
# Clone the repository
git clone https://github.com/rismanasdk/Libhttp
cd Libhttp

# Configure and build
mkdir build && cd build
cmake .. -DLIBHTTP_BUILD_TESTS=ON
make

# Install to system
sudo make install

# Optional: Run tests
./tests/test_core
```

### Option 2: Using CMake in Your Project

```bash
# Clone or include the library
git clone https://github.com/rismanasdk/Libhttp

# In your project's CMakeLists.txt
add_subdirectory(Libhttp)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE libhttp::libhttp)
```

### Option 3: Header-Only (Manual)

Simply include the header files directly in your project:

```cpp
#include "http.h"
```

Ensure you link against OpenSSL and zlib:

```bash
g++ your_program.cc -o your_program -lssl -lcrypto -lz
```

Example with the testing program:

```bash
g++ main.cc -o main -lssl -lcrypto -lz
```

## Building and Running

### Prerequisites

Install the required development libraries:

```bash
# On Ubuntu/Debian
sudo apt-get install libssl-dev zlib1g-dev

# On macOS
brew install openssl zlib
```

### Compilation

To compile a program that uses LibHTTP, you need to link against OpenSSL and zlib libraries:

```bash
g++ your_program.cc -o your_program -lssl -lcrypto -lz
```

Example with the testing program:

```bash
g++ testing.cc -o testing -lssl -lcrypto -lz
./testing
```

## Installation via Package Manager

LibHTTP can be packaged for modern C++ package managers instead of being copied manually.

### Option 1: CMake install + `find_package`

If you want to install from this repository directly:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
cmake --install build --prefix ./install
```

Then in another CMake project:

```cmake
find_package(libhttp REQUIRED)
target_link_libraries(your_app PRIVATE libhttp::libhttp)
```

### Option 2: Conan

This repository now includes a `conanfile.py`, so you can consume it as a Conan package.

Example consumer flow:

```bash
conan create . --build=missing
```

Example `CMakeLists.txt`:

```cmake
find_package(libhttp REQUIRED)
target_link_libraries(your_app PRIVATE libhttp::libhttp)
```

Example `conanfile.txt` in a consumer project:

```ini
[requires]
libhttp/3.0.0

[generators]
CMakeDeps
CMakeToolchain
```

### Option 3: vcpkg

This repository also includes starter files for a vcpkg port:

- `vcpkg.json`
- `portfile.cmake`

Before publishing to a real vcpkg registry, update:

- GitHub repository URL in `portfile.cmake`
- release tag
- SHA512 checksum
- homepage field in `vcpkg.json`

After that, users can install it with:

```bash
vcpkg install libhttp
```

### Compiler Flags Explanation

- `-lssl` - Links OpenSSL SSL/TLS library
- `-lcrypto` - Links OpenSSL cryptography library
- `-lz` - Links zlib compression library

These libraries are required for:

- **HTTPS connections** (TLS/SSL handshake, SNI support)
- **Automatic compression decompression** (gzip, deflate)

## Quick Start

### Basic GET Request

```cpp
#include <iostream>
#include "libhttp/libhttp.hpp"

int main() {
    // Simple GET request
    http::Response response = http::get("https://httpbin.org/get");

    std::cout << "Status: " << response.status_code << '\n';
    std::cout << "Body: " << response.text() << '\n';
}
```

### Session with Multiple Requests

```cpp
#include "libhttp/libhttp.hpp"

int main() {
    // Create a session for persistent connections
    http::Session session("https://api.github.com");

    // Set default headers
    session.set_header("Accept", "application/vnd.github.v3+json");
    session.set_header("User-Agent", "libhttp-client");

    // Make multiple requests - connections are reused
    auto resp1 = session.get("/users/github");
    auto resp2 = session.get("/repos/github/libhttp");

    std::cout << "First: " << resp1.status_code << '\n';
    std::cout << "Second: " << resp2.status_code << '\n';
}
```

### POST with JSON

```cpp
#include "libhttp/libhttp.hpp"

int main() {
    http::RequestOptions opts;
    opts.headers["Content-Type"] = "application/json";

    http::Response response = http::post(
        "https://httpbin.org/post",
        R"({"name": "john", "age": 30})",
        opts
    );

    std::cout << response.text() << '\n';
}
```

---

## Requests-Like Usage

### GET

```cpp
http::Response response = http::get("http://example.com");
```

### GET with params

```cpp
http::RequestOptions options;
options.params["page"] = "1";
options.params["search"] = "cpp";

http::Response response = http::get("http://example.com/articles", options);
```

### POST form data

```cpp
http::RequestOptions options;
options.data["username"] = "risman";
options.data["role"] = "student";

http::Response response = http::post("http://example.com/login", options);
```

### POST JSON

```cpp
http::RequestOptions options;
options.json["name"] = "Risman";
options.json["role"] = "student";

http::Response response = http::post("http://example.com/api/users", options);
```

### Basic auth

```cpp
http::RequestOptions options;
options.auth_username = "demo";
options.auth_password = "secret";

http::Response response = http::get("http://example.com/private", options);
```

### Multipart upload

```cpp
http::RequestOptions options;
options.data["title"] = "notes";
options.files["file"] = "note.txt";

http::Response response = http::post("http://example.com/upload", options);
```

### Redirects

```cpp
http::RequestOptions options;
options.allow_redirects = true;
options.max_redirects = 5;

http::Response response = http::get("http://example.com/redirect", options);
```

### Cookies

```cpp
http::RequestOptions options;
options.cookies["theme"] = "light";

http::Response response = http::get("http://example.com", options);
std::string session_id = response.cookie("session_id");
```

### Retry

```cpp
http::RequestOptions options;
options.retry_count = 3;
options.retry_delay_ms = 200;

http::Response response = http::get("http://example.com", options);
```

### Session

```cpp
http::Session session("http://example.com");
session.set_header("User-Agent", "session-demo/1.0");
session.set_cookie("visitor", "true");
session.set_basic_auth("demo", "secret");

http::Response response = session.get("/articles");
```

### Response JSON helper

```cpp
http::Response response = http::get("http://example.com/user");
http::Json data = response.json();
std::string name = data["name"];
```

### Download helper

```cpp
bool ok = http::download("http://example.com/file.txt", "saved.txt");
```

## v2.0 - New Features

### Proxy Support

```cpp
http::Session session("http://example.com");

session.set_proxy("proxy.company.com", "8080");

session.set_proxy_with_auth("proxy.company.com", "8080", "user", "pass");

session.set_proxy_url("http://user:pass@proxy.company.com:8080");

session.disable_proxy();
```

### Advanced Authentication

```cpp
http::Session session("http://example.com");

session.set_bearer_auth("eyJhbGciOiJIUzI1NiI...");

session.set_api_key("sk-1234567890");
session.set_api_key("custom-key-value", "X-Custom-Header");

session.set_auth_type("Digest", "username=user,realm=api");

session.clear_auth();
```

### Streaming Download with Progress

```cpp
auto progress = [](size_t current, size_t total) {
    int percent = (current * 100) / total;
    std::cout << "Downloaded: " << percent << "%" << std::endl;
};

http::RequestOptions options;
options.chunk_size = 4096;

bool ok = session.stream_download(
    "/large-file",
    "/tmp/downloaded-file",
    progress,
    options
);
```

### Enhanced JSON Support

```cpp
#include "http_json.h"

http::json::Value json = http::json::Value::parse(json_string);

http::json::Value obj = http::json::Value::object();
obj.object_value["name"] = http::json::Value("John");
obj.object_value["age"] = http::json::Value(30);

std::string compact = obj.dump();
std::string pretty = obj.dump(2);

if (json.is_object()) {
    auto val = json.object_value["key"];
}
if (json.is_array()) {
    for (const auto& item : json.array_value) {
        // process item
    }
}
```

### Status Code Helpers

```cpp
#include "http_status.h"

int code = http::status::NOT_FOUND;
int code = http::status::OK;
int code = http::status::INTERNAL_SERVER_ERROR;

std::string reason = http::status::reason_phrase(404);

bool is_ok = http::status::is_success(200);
bool is_error = http::status::is_client_error(400);
bool is_server_error = http::status::is_server_error(500);
```

### Better Response Checking

```cpp
http::Response response = session.get("/api/data");

if (response.is_success()) { /* 2xx */ }
if (response.is_client_error()) { /* 4xx */ }
if (response.is_server_error()) { /* 5xx */ }
if (response.is_informational()) { /* 1xx */ }

std::string content_type = response.content_type();
bool is_json = response.is_json();
bool is_html = response.is_html();
bool is_text = response.is_text();

size_t length = response.content_length();

response.raise_for_status();
```

### Exception Handling

```cpp
#include "http_exception.h"

try {
    auto response = session.get("/api/data");
    response.raise_for_status();
} catch (const http::StatusException& e) {
    std::cout << "Status " << e.status_code() << ": " << e.reason() << std::endl;
} catch (const http::ConnectionException& e) {
    std::cerr << "Connection failed: " << e.what() << std::endl;
} catch (const http::TimeoutException& e) {
    std::cerr << "Request timeout: " << e.what() << std::endl;
} catch (const http::HttpException& e) {
    std::cerr << "HTTP error: " << e.what() << std::endl;
}
```

### Streaming Content Iteration

```cpp
http::Response response = http::get("https://api.example.com/data");

// Iterate response body in chunks
response.iter_content(4096, [](const std::string& chunk) {
    std::cout << "Received chunk: " << chunk.size() << " bytes" << std::endl;
    return true;  // return false to stop iteration
});

// Iterate response body line by line
response.iter_lines([](const std::string& line) {
    std::cout << "Line: " << line << std::endl;
    return true;  // return false to stop iteration
});
```

### Request Timing

```cpp
http::Response response = http::get("https://api.example.com/data");
std::cout << "Request took " << response.elapsed << " seconds" << std::endl;
```

## v3.0 - New Features 🚀

### HTTP/2 Support

```cpp
#include "http_http2.h"

// HTTP/2 is designed for multiplexed streaming
http::HttpVersion version = http::HttpVersion::HTTP_2;

// Create HTTP/2 connection
auto conn = http::detail::Http2Connection(sockfd, true);

// Get next stream ID for multiplexing
uint32_t stream_id = conn.get_next_stream_id();

// Encode and send headers
std::string headers = conn.encode_headers({{"Content-Type", "application/json"}});
std::string frame = conn.create_headers_frame(stream_id, headers);
```

**Key Features:**

- Frame-based multiplexing for better performance
- Stream management with state tracking
- Connection preface handling
- Multiple requests over single connection
- Binary protocol for efficiency

### WebSocket Support

```cpp
#include "http_websocket.h"

// Create WebSocket connection
auto ws = http::websocket::Connection(sockfd, true);

// Register message handlers
ws.on_message([](const std::string& msg) {
    std::cout << "Received: " << msg << std::endl;
});

ws.on_binary([](const std::string& data) {
    std::cout << "Binary data received: " << data.size() << " bytes" << std::endl;
});

ws.on_close([](int code, const std::string& reason) {
    std::cout << "Connection closed: " << code << " - " << reason << std::endl;
});

ws.on_error([](const std::string& error) {
    std::cerr << "Error: " << error << std::endl;
});

// Send messages
ws.send_text("Hello WebSocket!");
ws.send_binary(binary_data);
ws.send_ping();

// Start receiving
ws.start_receive_loop();

// Check connection status
if (ws.is_open()) {
    ws.close_connection(1000, "Normal closure");
}
```

**Key Features:**

- Text and binary frame support
- Ping/Pong for keepalive
- Proper frame masking
- Callback-based event handling
- Thread-safe message sending

### Connection Pooling

```cpp
#include "http_connection_pool.h"

// Get global connection pool
auto& pool = http::global_connection_pool();

// Configure pool
pool.set_max_pool_size(20);  // Maximum connections
http::PoolingConfig config(true, 15);  // enable, max size

// Get connection statistics
auto stats = pool.get_stats();
std::cout << "Total: " << stats.total_connections << std::endl;
std::cout << "Active: " << stats.active_connections << std::endl;
std::cout << "Idle: " << stats.idle_connections << std::endl;

// Acquire/release connections
int sockfd = pool.acquire_connection("api.example.com", "443", true);
// Use connection...
pool.release_connection("api.example.com", "443", true, sockfd, true);

// Cleanup
pool.cleanup_expired();
pool.clear();
```

**Key Features:**

- Automatic connection reuse for performance
- Keep-alive support to reduce latency
- Thread-safe pooling with mutex
- Configurable pool size and idle timeout
- Statistics and monitoring
- Automatic cleanup of stale connections

## API Overview

### Free Functions

- `http::request(method, url, options)`
- `http::get(url, options)`
- `http::post(url, options)`
- `http::put(url, options)`
- `http::delete_(url, options)`
- `http::options(url, options)`
- `http::head(url, options)`
- `http::patch(url, options)`
- `http::download(url, output_path, options)`

### RequestOptions

- `headers`
- `params`
- `data`
- `json`
- `files`
- `cookies`
- `body`
- `auth_username`
- `auth_password`
- `auth_type` (v2.0)
- `auth_token` (v2.0)
- `proxy` (v2.0)
- `timeout_seconds`
- `allow_redirects`
- `max_redirects`
- `retry_count`
- `retry_delay_ms`
- `stream` (v2.0)
- `chunk_size` (v2.0)
- `progress_callback` (v2.0)
- `verify_ssl` (v2.0)
- `ssl_cert_path` (v2.0)
- `ssl_key_path` (v2.0)

### Response

- `status_code`
- `reason`
- `headers`
- `cookies`
- `body`
- `url`
- `method`
- `history`
- `ok()`
- `text()`
- `content()`
- `lines()`
- `header(key)`
- `has_header(key)`
- `cookie(key)`
- `is_redirect()`
- `redirected()`
- `json()`
- `is_success()` (v2.0)
- `is_error()` (v2.0)
- `is_client_error()` (v2.0)
- `is_server_error()` (v2.0)
- `is_informational()` (v2.0)
- `content_length()` (v2.0)
- `content_type()` (v2.0)
- `is_json()` (v2.0)
- `is_html()` (v2.0)
- `is_text()` (v2.0)
- `raise_for_status()` (v2.0)
- `elapsed` (v2.0) — request duration in seconds
- `iter_content(chunk_size, processor)` (v2.0) — iterate response body in chunks
- `iter_lines(processor)` (v2.0) — iterate response body line by line

### Session

- `set_base_url(url)`
- `get_base_url()`
- `set_timeout(seconds)`
- `get_timeout()`
- `set_header(key, value)`
- `set_headers(headers)`
- `headers()`
- `set_cookie(key, value)`
- `set_basic_auth(username, password)`
- `set_bearer_auth(token)` (v2.0)
- `set_api_key(key, header_name)` (v2.0)
- `set_auth_type(scheme, token)` (v2.0)
- `clear_auth()` (v2.0)
- `cookies()`
- `clear_cookies()`
- `set_proxy(host, port)` (v2.0)
- `set_proxy_with_auth(host, port, user, pass)` (v2.0)
- `set_proxy_url(proxy_url)` (v2.0)
- `disable_proxy()` (v2.0)
- `get_proxy()` (v2.0)
- `request(method, path, options)`
- `get(path, options)`
- `post(path, options)`
- `put(path, options)`
- `delete_(path, options)`
- `options(path, options)`
- `head(path, options)`
- `patch(path, options)`
- `download(path, output_path, options)`
- `stream_download(path, output_path, progress_callback, options)` (v2.0)

## HTTPS/TLS Support

```cpp
http::Response response = http::get("https://api.github.com/users/github");

http::Session session("https://secure.example.com");
http::Response resp = session.get("/api/data");
```

HTTPS requests are automatically detected and use OpenSSL for TLS negotiation. Gzip-compressed responses are automatically decompressed.

## Notes

- `http://` and `https://` URLs are supported
- HTTP/1.1 is the default protocol over TCP sockets (plain for HTTP, TLS for HTTPS)
- HTTP/2 support available via `http_http2.h` for multiplexed connections
- `delete_()` uses an underscore because `delete` is a reserved C++ keyword
- `Client` is available as an alias of `Session`
- chunked responses are supported
- `Response::json()` provides simple flat JSON object parsing
- `http_json.h` provides full JSON parsing with nested object/array support
- proxy support is available for HTTP/HTTPS/SOCKS protocols
- streaming download API supports progress callbacks
- advanced authentication methods include Bearer tokens, API keys, and custom schemes
- HTTPS/TLS uses OpenSSL library
- automatic decompression supports gzip and deflate formats
- response streaming with `iter_content()` and `iter_lines()` for memory-efficient processing
- request elapsed time is tracked automatically in `response.elapsed`
- WebSocket support for real-time bidirectional communication
- Connection pooling for improved performance with keep-alive

## Build and Link

To compile with HTTPS, automatic compression, and advanced features support, link against OpenSSL and zlib:

```bash
g++ -std=c++17 -Wall -Wextra main.cc -o http_test -lssl -lcrypto -lz
```

## Comparison with Python Requests

LibHTTP v3.0 implements most essential features of Python's `requests` library:

| Feature                                | Python requests              | LibHTTP v3.0        | Notes                                |
| -------------------------------------- | ---------------------------- | ------------------- | ------------------------------------ |
| GET/POST/PUT/DELETE/PATCH/HEAD/OPTIONS | ✅                           | ✅                  | Full HTTP method support             |
| Query parameters                       | ✅                           | ✅                  | Automatic URL encoding               |
| Headers & Cookies                      | ✅                           | ✅                  | Case-insensitive header access       |
| JSON body/parsing                      | ✅                           | ✅                  | Full JSON support via http_json.h    |
| Form data & files                      | ✅                           | ✅                  | Multipart upload supported           |
| Basic Authentication                   | ✅                           | ✅                  | Base64 encoding                      |
| Bearer Token Auth                      | ✅                           | ✅                  | Custom auth support                  |
| Proxy support                          | ✅                           | ✅                  | HTTP/HTTPS/SOCKS with credentials    |
| HTTPS/SSL                              | ✅                           | ✅                  | OpenSSL integration                  |
| Automatic decompression                | ✅ (gzip, deflate, brotli)   | ✅ (gzip, deflate)  | zlib support                         |
| Response streaming                     | ✅ (iter_content/iter_lines) | ✅                  | iter_content/iter_lines methods      |
| Status code checking                   | ✅                           | ✅                  | is_success/is_error/raise_for_status |
| Redirect handling                      | ✅                           | ✅                  | Configurable max_redirects           |
| Timeouts                               | ✅                           | ✅                  | Per-request timeout                  |
| Retries with backoff                   | ✅                           | ✅ (manual backoff) | Retry count & delay                  |
| Session/connection reuse               | ✅                           | ✅ (Session object) | Cookie jar, headers, auth            |
| Request elapsed time                   | ✅                           | ✅                  | response.elapsed in seconds          |
| Connection pooling                     | ✅                           | ✅ (v3.0)           | Keep-alive and automatic cleanup     |
| WebSocket support                      | ✅                           | ✅ (v3.0)           | Frame handling, callbacks            |
| HTTP/2 support                         | ✅                           | ✅ (v3.0)           | Multiplexing, stream management      |
| Prepared requests                      | ✅                           | ⚠️                  | Not yet implemented                  |
| Hooks/callbacks                        | ✅                           | ✅ (WebSocket)      | Message and close callbacks          |
| Client certificates (mTLS)             | ✅                           | ❌                  | Future enhancement                   |
| Brotli compression                     | ✅                           | ❌                  | Can be added if needed               |

## Testing & Documentation

### Running Tests

LibHTTP v3.0.0 includes a comprehensive test suite for validation:

```bash
# Build with tests enabled
cmake .. -DLIBHTTP_BUILD_TESTS=ON

# Run the test suite
./tests/test_core
```

All tests are offline (no network required) and validate:

- Request options and defaults
- Response parsing and helpers
- HTTP status code classification
- Header and cookie access
- Session management
- Proxy configuration
- Custom exception types

See [TESTS.md](TESTS.md) for complete test documentation.

### Documentation

- **[TESTS.md](TESTS.md)** - Comprehensive test suite guide with 13 tests
- **[STABILIZATION.md](STABILIZATION.md)** - v3.0.0 release notes and feature overview
- **[CHANGELOG.md](CHANGELOG.md)** - Complete version history
- **[DEVELOPMENT.md](DEVELOPMENT.md)** - Contributor guidelines
- **[examples/](examples/)** - 7 complete working examples

### Key Documentation Sections

**For v3.0.0 Production Users:**

- Use [STABILIZATION.md](STABILIZATION.md) for stability guarantees
- Check [Feature Flags](#feature-flags) for HTTP/2 and WebSocket opt-in
- Review [Known Limitations](#current-limitations) section below

**For Contributors:**

- See [DEVELOPMENT.md](DEVELOPMENT.md) for project structure
- Check [examples/](examples/) for usage patterns
- Read [TESTS.md](TESTS.md) for test guidelines

---

## Current Limitations

This library implements the core features needed for most HTTP applications:

Potential future enhancements:

- client certificate (mTLS) support
- automatic cookie jar persistence to disk
- Brotli compression support
- prepared requests for advanced use cases
- HPACK compression for HTTP/2 headers
- push promise support for HTTP/2

## License

See [LICENSE](LICENSE) for license details.
