# LibHTTP

Small header-only C++ HTTP client with a requests-like API.

`libhttp` is designed for simple `http://` use cases where you want a light client, direct control, and a familiar interface:

- free functions like `http::get(...)`
- a reusable `http::Client`
- a `http::Response` with `status_code`, `headers`, `body`, and `text()`

This project currently focuses on plain HTTP over HTTP/1.1. It is not trying to match the full feature set of Python `requests`.

## Positioning

`libhttp` is a good fit if you want:

- a tiny header-only client
- no external dependency
- requests-like ergonomics
- explicit behavior

It is not a good fit if you need:

- HTTPS
- redirects
- cookies or sessions
- automatic JSON parsing
- HTTP/2 or HTTP/3

## Features

- `GET`
- `POST`
- `PUT`
- `DELETE` via `delete_()`
- `OPTIONS`
- `HEAD`
- `PATCH`
- query parameters
- custom headers
- form data
- raw body payloads
- reusable client with default headers and timeout
- chunked response decoding
- case-insensitive response header lookup

## Files

- `http.h`: main header
- `http_request.h`: request options
- `http_response.h`: response object

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

## Examples

### Basic GET

```cpp
http::Response response = http::get("http://example.com");
```

### GET with query parameters

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

### POST JSON body

```cpp
http::RequestOptions options;
options.headers["Content-Type"] = "application/json";
options.body = "{\"name\":\"Risman\"}";

http::Response response = http::post("http://example.com/api/users", options);
```

### HEAD request

```cpp
http::Response response = http::head("http://example.com");

if (response.has_header("server")) {
    std::cout << response.header("Server") << '\n';
}
```

### Reusable client

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

## API

### `http::RequestOptions`

- `headers`: custom request headers
- `params`: query string parameters
- `data`: `application/x-www-form-urlencoded` form fields
- `body`: raw request body
- `timeout_seconds`: socket timeout in seconds

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
- `has_header(key)`
- `is_redirect()`

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

- `Client()`
- `Client(base_url)`
- `Client(base_url, headers, timeout_seconds)`
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
- The implementation uses HTTP/1.1 over TCP sockets.
- `delete_()` uses an underscore because `delete` is a reserved C++ keyword.
- `Connection: close` is used by default to keep behavior simple and predictable.
- The `Host` header includes the port when a non-default port is used.

## Limitations

- no HTTPS
- no automatic redirect following
- no cookie jar or session persistence
- no built-in JSON parser
- no streaming API
- no HTTP/2 or HTTP/3
- currently intended for Linux or POSIX-style socket environments

## Naming Recommendation

For this repository, I would keep the repo name as `libhttp`.

Why:

- it is short and easy to remember
- the scope can be clarified in the README instead of the repo name
- renaming to something like `libhttp1` or `libhttp-http1` usually makes the project feel narrower and less polished

A better compromise is:

- keep the repo name `libhttp`
- use a subtitle like `Small header-only C++ HTTP/1.1 client`
- clearly state the current limitations near the top of the README

If one day you add HTTPS, redirects, cookies, or broader protocol support, the `libhttp` name will still make sense.

## License

See [LICENSE](LICENSE) for license details.
