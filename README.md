# LibHTTP

LibHTTP is a header-only C++ HTTP/1.1 client for plain `http://` requests.

The goal is to move closer to Python `requests`, but in clear feature targets so the code stays understandable and easy to extend.

## Current Target

This batch focuses on 5 requests-like features:

1. free request functions such as `get()`, `post()`, `put()`, `delete_()`, `options()`, `head()`, and `patch()`
2. a reusable `Session` object
3. query params, headers, form data, raw body, and JSON body
4. redirect handling
5. cookie handling

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

### Session

```cpp
http::Session session("http://example.com");
session.set_header("User-Agent", "session-demo/1.0");
session.set_cookie("visitor", "true");

http::Response response = session.get("/articles");
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

### RequestOptions

- `headers`
- `params`
- `data`
- `json`
- `cookies`
- `body`
- `timeout_seconds`
- `allow_redirects`
- `max_redirects`

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
- `header(key)`
- `has_header(key)`
- `cookie(key)`
- `is_redirect()`
- `redirected()`

### Session

- `set_base_url(url)`
- `get_base_url()`
- `set_timeout(seconds)`
- `get_timeout()`
- `set_header(key, value)`
- `set_headers(headers)`
- `headers()`
- `set_cookie(key, value)`
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

## Notes

- only `http://` URLs are supported
- this library uses HTTP/1.1 over plain TCP sockets
- `delete_()` uses an underscore because `delete` is a reserved C++ keyword
- `Client` is available as an alias of `Session`
- chunked responses are supported

## Current Limitations

This is still not fully equal to Python `requests`.

Still missing for later targets:

- authentication helpers such as basic auth
- multipart file upload
- automatic JSON parsing
- proxy support
- connection pooling
- retry helpers
- streaming download API

## License

See [LICENSE](LICENSE) for license details.
