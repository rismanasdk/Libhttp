# LibHTTP

LibHTTP is a header-only C++ HTTP/1.1 client library for `http://` and `https://` requests, designed to be intuitive like Python's `requests` library while maintaining simplicity and performance.

The goal is to move closer to Python `requests`, but in clear feature targets so the code stays understandable and easy to extend.

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

## File Layout

The library is split into smaller headers:

### Core Headers

- `http.h` as the main include
- `http_request.h` for request options and shared types
- `http_response.h` for the response object
- `http_detail.h` for low-level helpers
- `http_session.h` for the `Session` class

### v2.0 Feature Headers

- `http_exception.h` for custom exception types
- `http_status.h` for HTTP status code constants and helpers
- `http_proxy.h` for proxy configuration
- `http_auth.h` for advanced authentication helpers
- `http_json.h` for full JSON parsing and serialization
- `http_stream.h` for streaming download support

## Installation

### Option 1: Using vcpkg (Recommended)

vcpkg makes it easy to install LibHTTP as a package dependency.

```bash
# Add libhttp to your vcpkg.json
{
  "dependencies": [ "libhttp" ]
}
```

Then install:

```bash
vcpkg install
```

In your `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.15)
project(MyApp)

find_package(libhttp REQUIRED)

add_executable(myapp main.cpp)
target_link_libraries(myapp PRIVATE libhttp::libhttp)
```

### Option 2: Using CMake (Direct)

```bash
# Clone or include the library
git clone https://github.com/your-username/lib-http

# In your project's CMakeLists.txt
add_subdirectory(lib-http)

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
libhttp/2.0.0

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

```cpp
#include <iostream>
#include "http.h"

int main() {
    http::Response response = http::get("http://example.com");

    std::cout << response.status_code << '\n';
    std::cout << response.reason << '\n';
    std::cout << response.text() << '\n';
}
```

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
- `verify_ssl` (v2.0, reserved for future custom certificate support)
- `ssl_cert_path` (v2.0, reserved for future custom certificate support)
- `ssl_key_path` (v2.0, reserved for future custom certificate support)

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
- this library uses HTTP/1.1 over TCP sockets (plain for HTTP, TLS for HTTPS)
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

## Build and Link

To compile with HTTPS and automatic compression support, link against OpenSSL and zlib:

```bash
g++ -std=c++17 -Wall -Wextra main.cc -o http_test -lssl -lcrypto -lz
```

## Comparison with Python Requests

LibHTTP v2.0 implements most essential features of Python's `requests` library:

| Feature                                | Python requests              | LibHTTP v2.0        | Notes                                |
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
| Prepared requests                      | ✅                           | ⚠️                  | Not yet implemented                  |
| Hooks/callbacks                        | ✅                           | ✅ (partial)        | Progress callbacks for downloads     |
| Connection pooling                     | ✅                           | ❌                  | Future enhancement                   |
| Client certificates (mTLS)             | ✅                           | ❌                  | Future enhancement                   |
| Brotli compression                     | ✅                           | ❌                  | Can be added if needed               |

## Current Limitations

This library implements the core features needed for most HTTP applications:

Potential future enhancements:

- connection pooling / keep-alive for better performance
- client certificate (mTLS) support
- automatic cookie jar persistence to disk
- Brotli compression support
- WebSocket support
- prepared requests for advanced use cases

## License

See [LICENSE](LICENSE) for license details.
