# LibHTTP

LibHTTP is a header-only C++ HTTP/1.1 client for plain `http://` requests.

The goal is to move closer to Python `requests`, but in clear feature targets so the code stays understandable and easy to extend.

## Current Progress

Target 1:

1. free request functions such as `get()`, `post()`, `put()`, `delete_()`, `options()`, `head()`, and `patch()`
2. a reusable `Session` object
3. query params, headers, form data, raw body, and JSON body
4. redirect handling
5. cookie handling

Target 2:

1. basic auth
2. multipart file upload
3. retry options
4. simple JSON parsing from response body
5. download helper

## File Layout

The library is split into smaller headers:

- `http.h` as the main include
- `http_request.h` for request options and shared types
- `http_response.h` for the response object
- `http_detail.h` for low-level helpers
- `http_session.h` for the `Session` class

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
- `timeout_seconds`
- `allow_redirects`
- `max_redirects`
- `retry_count`
- `retry_delay_ms`

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
- `cookies()`
- `clear_cookies()`
- `request(method, path, options)`
- `get(path, options)`
- `post(path, options)`
- `put(path, options)`
- `delete_(path, options)`
- `options(path, options)`
- `head(path, options)`
- `patch(path, options)`
- `download(path, output_path, options)`

## Notes

- only `http://` URLs are supported
- this library uses HTTP/1.1 over plain TCP sockets
- `delete_()` uses an underscore because `delete` is a reserved C++ keyword
- `Client` is available as an alias of `Session`
- chunked responses are supported
- `Response::json()` currently supports simple flat JSON objects

## Current Limitations

This is still not fully equal to Python `requests`.

Still missing for later targets:

- proxy support
- connection pooling
- streaming download API
- richer JSON support than flat key-value objects
- multipart metadata such as custom content types per file

## License

See [LICENSE](LICENSE) for license details.
