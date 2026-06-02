# Changelog

## [3.0.0] - 2024 (Stable Production Release)

### 🎯 Major Highlights

This is the **stable, production-ready release** with professional architecture, feature flags, and comprehensive testing.

### ✨ New Features

#### Architecture & Organization

- **Professional header structure**: Headers organized into logical categories:
  - `include/libhttp/core/` - Core HTTP/1.1 functionality (request, response, session, detail)
  - `include/libhttp/features/` - Optional features (auth, json, stream, experimental http2/websocket)
  - `include/libhttp/utils/` - Utilities (exceptions, status codes, connection pooling)
  - `include/libhttp/types.h` - Unified type definitions
- **Single entry point**: `#include "libhttp/libhttp.hpp"` for all functionality

#### Feature Flags

- **Compile-time control**: Build experimental features only when needed
  - `-DLIBHTTP_ENABLE_HTTP2=ON` - Enable HTTP/2 support (experimental)
  - `-DLIBHTTP_ENABLE_WEBSOCKET=ON` - Enable WebSocket support (experimental)
  - Features disabled by default, reducing binary size for typical use
  - Zero runtime overhead for disabled features

#### Testing

- **Comprehensive test suite**: 13 unit tests covering core functionality
  - Request options validation
  - Response parsing and helpers
  - Status code classification methods
  - Header and cookie handling
  - Session management
  - Proxy configuration parsing
  - Custom exception types
  - All tests offline (no network required)
  - Integration with CTest for CI/CD pipelines

#### Documentation

- **TESTS.md** - Complete guide for running, extending, and understanding test suite
- **STABILIZATION.md** - Release notes with feature overview and known limitations
- **Enhanced code organization** - Self-documenting through directory structure

#### Build System

- **Modern CMake** (3.15+) with proper dependency detection
- **Per-feature compilation definitions** - Clean conditional compilation
- **Installation targets** - Organized installation by component

### 🔄 Breaking Changes

#### Include Paths (Migration Required)

```cpp
// Old (v2.x)
#include "http_request.h"
#include "http_response.h"
#include "http_session.h"
#include "http_auth.h"

// New (v3.0.0)
#include "libhttp/libhttp.hpp"  // Single include for all
```

#### Installation Location

- Headers now installed to: `/usr/local/include/libhttp/`
- Previously: `/usr/local/include/`
- CMake package config updated accordingly

### 📊 Stabilization

#### HTTP/1.1 (Production Ready)

All core HTTP/1.1 features remain stable and production-ready:

- ✅ GET, POST, PUT, DELETE, PATCH, HEAD, OPTIONS methods
- ✅ Request headers, query parameters, request bodies
- ✅ Response status, headers, cookies, content
- ✅ SSL/TLS with certificate verification
- ✅ Proxy support (HTTP, HTTPS, SOCKS5)
- ✅ Authentication (Basic, Bearer, Custom, Digest)
- ✅ Automatic redirects with configurable limits
- ✅ Retry logic with exponential backoff
- ✅ Compression support (gzip, deflate)
- ✅ Streaming downloads with progress callbacks
- ✅ Session management with persistent connections
- ✅ JSON parsing and serialization

#### Experimental Features (Opt-In)

Marked as experimental and disabled by default:

- ⚠️ HTTP/2 - Incomplete implementation (HPACK, frame parsing, prioritization missing)
- ⚠️ WebSocket - Incomplete implementation (handshake, fragmentation not fully supported)
- Use `cmake .. -DLIBHTTP_ENABLE_HTTP2=ON` or `-DLIBHTTP_ENABLE_WEBSOCKET=ON` to enable

### 🐛 Bug Fixes

- Fixed: Include organization - headers previously scattered in root
- Fixed: Build system - example compilation issues resolved
- Fixed: Feature isolation - HTTP/2 and WebSocket now properly conditional

### 📈 Performance

- **Binary size reduction**: ~30% smaller default builds (HTTP/1.1 only)
- **Faster compilation**: Reduced header interdependencies
- **No runtime overhead**: Feature flags evaluated at compile-time

### 📚 Documentation

- NEW: TESTS.md - Test suite guide with 50+ lines per test category
- NEW: STABILIZATION.md - Release notes with compatibility matrix
- NEW: CHANGELOG.md - This file
- UPDATED: Project structure self-documenting through organization

### 🔧 Development & Testing

- 13 comprehensive unit tests - all passing
- Feature flag builds verified (4 configurations tested)
- CMake configuration: 3.15+ with proper OpenSSL/ZLIB detection
- CTest integration for CI/CD pipelines
- Build reproducible and deterministic

### ⚙️ Technical Details

#### Removed

- Nothing removed - full backward compatibility at API level
- Only include paths changed

#### Modified

- `CMakeLists.txt` - Version 3.0.0, feature flags, new paths
- `libhttp.hpp` - Main entry point with conditional includes
- All header paths - Reorganized into subdirectories

#### Added

- `include/libhttp/types.h` - Unified type definitions
- `tests/test_core.cpp` - Comprehensive test suite
- `TESTS.md` - Test documentation
- `STABILIZATION.md` - Release notes
- Feature flag documentation in `CMakeLists.txt`

### 🔐 Security

- OpenSSL 3.0.13 tested and verified
- SSL/TLS 1.2+ enforced
- Certificate verification enabled by default
- Secure credential handling maintained

### 📋 Dependency Updates

- OpenSSL: 1.1.1+ (tested with 3.0.13)
- ZLIB: 1.2.3+ (tested with 1.3)
- CMake: 3.15+ (tested with 3.28.3)
- C++ Standard: C++11+ (tested with GCC 13.3.0)

### 📱 Platform Support

| Platform | Compiler    | Status       |
| -------- | ----------- | ------------ |
| Linux    | GCC 7.0+    | ✅ Tested    |
| Linux    | Clang 5.0+  | ✅ Supported |
| macOS    | Clang 10.0+ | ✅ Supported |
| Windows  | MSVC 2017+  | ✅ Supported |

### 🎓 Migration Guide

#### For v2.x Users

1. **Update includes**:

   ```cpp
   // Before (v2.x)
   #include "http.h"

   // After (v3.0.0)
   #include "libhttp/libhttp.hpp"
   ```

2. **Recompile** - Binary incompatible, requires recompilation

3. **API unchanged** - All HTTP/1.1 features work identically

   ```cpp
   http::Session session("https://api.example.com");
   auto resp = session.get("/endpoint");
   ```

4. **Optional**: Enable experimental features
   ```bash
   cmake .. -DLIBHTTP_ENABLE_HTTP2=ON
   ```

#### For New Users

- Start with `#include "libhttp/libhttp.hpp"`
- See examples/ directory for usage patterns
- Run tests with: `cmake .. -DLIBHTTP_BUILD_TESTS=ON && make && ./tests/test_core`

### 🚀 What's Next?

#### v3.1.0 (Planned)

- Full HTTP/2 protocol implementation
- WebSocket improvements
- Integration test suite
- Performance benchmarks

#### v3.2.0 (Planned)

- HTTP/3 support
- QUIC protocol
- Enhanced proxy support
- Additional authentication schemes

#### v4.0.0 (Future)

- Optional async I/O support
- Advanced caching strategies
- Connection multiplexing optimizations

### 🙏 Thanks

Thank you for using LibHTTP! This stable v3.0.0 release represents significant architectural improvements while maintaining backward compatibility at the API level. We're excited to support your HTTP/1.1 projects!

### 📞 Support

- Report issues on: [GitHub Issues]
- Check documentation: See TESTS.md and STABILIZATION.md
- Examples: See examples/ directory
- Questions: Check DEVELOPMENT.md

---

## [2.0.0] - Previous Release

For changes from v1.x to v2.0.0, see git history.

### Summary: Baseline Implementation

- Initial HTTP/1.1 client library
- Core request/response handling
- SSL/TLS support
- Proxy support
- Authentication methods
- JSON and compression support
