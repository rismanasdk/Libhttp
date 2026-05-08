# Development Guide for LibHTTP

This guide explains how to build, test, and package LibHTTP for distribution.

## Local Development Setup

### Build LibHTTP Locally

```bash
# Create build directory
mkdir build && cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .

# Install locally (optional)
cmake --install . --prefix ./install
```

### Test with CMake

Create a test project:

```bash
# Create test directory
mkdir test-project && cd test-project

# Create CMakeLists.txt
cat > CMakeLists.txt << 'EOF'
cmake_minimum_required(VERSION 3.15)
project(libhttp_test)

# Point to local libhttp
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../build/install")

find_package(libhttp REQUIRED)

add_executable(test_http test.cpp)
target_link_libraries(test_http PRIVATE libhttp::libhttp)
EOF

# Create test program
cat > test.cpp << 'EOF'
#include <iostream>
#include "http.h"

int main() {
    http::Response response = http::get("https://httpbin.org/get");
    std::cout << "Status: " << response.status_code << std::endl;
    return 0;
}
EOF

# Build and run
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH=../../build/install
cmake --build .
./test_http
```

## Publishing to vcpkg

### Prerequisites

1. Fork or create a repo with LibHTTP
2. Tag a release: `git tag v2.0.0`
3. Push to GitHub

### Steps to Add to Official vcpkg

1. **Clone vcpkg repository**:

   ```bash
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   ```

2. **Create port directory**:

   ```bash
   mkdir ports/libhttp
   ```

3. **Copy port files**:

   ```bash
   cp ../lib-http/portfile.cmake ports/libhttp/
   cp ../lib-http/vcpkg.json ports/libhttp/vcpkg.json
   ```

4. **Update portfile.cmake** with correct GitHub URL and SHA512

5. **Test the port**:

   ```bash
   ./vcpkg install libhttp
   ```

6. **Submit PR** to Microsoft/vcpkg repository

### For Private/Local vcpkg Registry

1. **Create a registry**:

   ```bash
   mkdir my-ports
   mkdir my-ports/libhttp
   cp portfile.cmake vcpkg.json my-ports/libhttp/
   ```

2. **Configure vcpkg.json in your project**:

   ```json
   {
     "registries": [
       {
         "kind": "filesystem",
         "path": "../my-ports"
       }
     ],
     "dependencies": ["libhttp"]
   }
   ```

3. **Install**:
   ```bash
   vcpkg install
   ```

## Cross-Platform Considerations

LibHTTP uses standard dependencies (OpenSSL, zlib) that are available on:

- ✅ Linux (apt, yum, pacman)
- ✅ macOS (Homebrew)
- ✅ Windows (vcpkg)

The CMake configuration handles platform-specific details automatically.

## Version Management

Update version in three places:

1. `CMakeLists.txt` - `project(libhttp VERSION 2.0.0 ...)`
2. `vcpkg.json` - `"version": "2.0.0"`
3. Git tags - `git tag v2.0.0`

Keep versions synchronized across files.

## Publishing to Conan

### Create the package locally

```bash
conan create . --build=missing
```

### Use it from another project

Example consumer `conanfile.txt`:

```ini
[requires]
libhttp/2.0.0

[generators]
CMakeDeps
CMakeToolchain
```

Then configure the consumer project:

```bash
conan install . --build=missing
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=build/conan_toolchain.cmake
cmake --build build
```

### Before publishing remotely

Update these fields first:

1. `conanfile.py` -> `url`
2. `conanfile.py` -> `homepage`
3. package version if a new release is made

If you publish to ConanCenter or a private remote, keep the package version aligned with Git tags and `CMakeLists.txt`.
