# LibHTTP

LibHTTP is a small header-only C++ HTTP/1.1 client library for plain `http://` requests.

The API is designed to feel familiar if you have used Python `requests` before:

- simple free functions such as `http::get(...)`
- a reusable `http::Client` object
- a `http::Response` object with `status_code`, `headers`, `body`, and `text()`

This library supports plain HTTP only. It does not support HTTPS.

## Features

- `GET`
- `POST`
- `PUT`
- `DELETE` through `delete_()`
- `OPTIONS`
- `HEAD`
- `PATCH`
- query parameters
- custom headers
- form data
- raw body payloads
- reusable client with default headers and timeout

## Headers

- `http.h` for the full library
- `http_request.h` for request options
- `http_response.h` for the response object

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

## Requests-Like Examples

### GET

```cpp
http::Response response = http::get("http://example.com");
```

### GET with query params

```cpp
http::RequestOptions options;
options.params["page"] = "1";
options.params["search"] = "cpp";

http::Response response = http::get("http://example.com/articles", options);
```

### POST with form data

```cpp
http::RequestOptions options;
options.data["username"] = "risman";
options.data["role"] = "student";

http::Response response = http::post("http://example.com/login", options);
```

### POST with raw body

```cpp
http::RequestOptions options;
options.headers["Content-Type"] = "application/json";
options.body = "{\"name\":\"Risman\"}";

http::Response response = http::post("http://example.com/api/users", options);
```

### PUT

```cpp
http::RequestOptions options;
options.body = "updated value";

http::Response response = http::put("http://example.com/items/1", options);
```

### DELETE

```cpp
http::Response response = http::delete_("http://example.com/items/1");
```

### OPTIONS

```cpp
http::Response response = http::options("http://example.com");
```

### HEAD

```cpp
http::Response response = http::head("http://example.com");
std::string server = response.header("Server");
```

### PATCH

```cpp
http::RequestOptions options;
options.body = "partial update";

http::Response response = http::patch("http://example.com/items/1", options);
```

## Using Client

```cpp
#include <iostream>
#include "http.h"

int main() {
    http::Client client("http://example.com");
    client.set_header("User-Agent", "my-client/1.0");
    client.set_timeout(5);

    http::RequestOptions options;
    options.params["page"] = "2";

    http::Response response = client.get("/articles", options);
    std::cout << response.text() << '\n';
}
```

## API Reference

### `http::RequestOptions`

- `headers` for custom request headers
- `params` for query string parameters
- `data` for `application/x-www-form-urlencoded` form data
- `body` for raw request body
- `timeout_seconds` for socket timeout

### `http::Response`

- `status_code`
- `reason`
- `headers`
- `body`
- `url`
- `method`
- `ok()`
- `text()`
- `content()`
- `header(key)`

### Free Functions

- `http::request(method, url, options)`
- `http::get(url, options)`
- `http::post(url, options)`
- `http::put(url, options)`
- `http::delete_(url, options)`
- `http::options(url, options)`
- `http::head(url, options)`
- `http::patch(url, options)`

### `http::Client`

- constructor `Client()`
- constructor `Client(base_url)`
- constructor `Client(base_url, headers, timeout_seconds)`
- `set_base_url(url)`
- `get_base_url()`
- `set_timeout(seconds)`
- `get_timeout()`
- `set_header(key, value)`
- `set_headers(headers)`
- `headers()`
- `request(method, path, options)`
- `get(path, options)`
- `post(path, options)`
- `put(path, options)`
- `delete_(path, options)`
- `options(path, options)`
- `head(path, options)`
- `patch(path, options)`

## Notes

- Only `http://` URLs are accepted.
- The library uses HTTP/1.1 over TCP sockets.
- `delete_()` uses an underscore because `delete` is a reserved keyword in C++.
- `Connection: close` is used by default to keep the implementation simple and predictable.
- Chunked responses are supported.

## Limitations

- no HTTPS
- no HTTP/2 or HTTP/3
- no automatic redirect handling
- no cookies or session store
- no JSON parser built in
- currently designed for Linux or POSIX-style socket environments

## License

See [LICENSE](LICENSE) for license details.
