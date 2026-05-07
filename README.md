# LibHTTP

LibHTTP is a header-only C++ HTTP/1.1 client for plain `http://` requests.

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

## Notes

- only `http://` URLs are supported
- this library uses HTTP/1.1 over plain TCP sockets
- `delete_()` uses an underscore because `delete` is a reserved C++ keyword
- `Client` is available as an alias of `Session`
- chunked responses are supported
- `Response::json()` provides simple flat JSON object parsing
- `http_json.h` provides full JSON parsing with nested object/array support
- proxy support is available for HTTP/HTTPS/SOCKS protocols
- streaming download API supports progress callbacks
- advanced authentication methods include Bearer tokens, API keys, and custom schemes

## Current Limitations

This library is not yet fully equal to Python `requests`, but v2.0 addresses many gaps:

Still missing for future targets:

- connection pooling / keep-alive
- HTTPS support (currently HTTP only)
- automatic cookie jar persistence
- multipart metadata such as custom content types per file
- WebSocket support

## License

See [LICENSE](LICENSE) for license details.
