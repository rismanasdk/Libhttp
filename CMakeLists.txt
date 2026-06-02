cmake_minimum_required(VERSION 3.15)
project(libhttp VERSION 3.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Feature flags
option(LIBHTTP_ENABLE_HTTP2 "Enable experimental HTTP/2 support" OFF)
option(LIBHTTP_ENABLE_WEBSOCKET "Enable experimental WebSocket support" OFF)
option(LIBHTTP_BUILD_TESTS "Build test suite" ON)
option(LIBHTTP_BUILD_EXAMPLES "Build examples" OFF)

# Header-only library
add_library(libhttp INTERFACE)
add_library(libhttp::libhttp ALIAS libhttp)

target_include_directories(libhttp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(libhttp INTERFACE cxx_std_11)

# Add compile definitions for feature flags
if(LIBHTTP_ENABLE_HTTP2)
    target_compile_definitions(libhttp INTERFACE LIBHTTP_HTTP2_ENABLED)
endif()

if(LIBHTTP_ENABLE_WEBSOCKET)
    target_compile_definitions(libhttp INTERFACE LIBHTTP_WEBSOCKET_ENABLED)
endif()

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(libhttp INTERFACE
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
)

# Install main header
install(FILES
    include/libhttp/libhttp.hpp
    include/libhttp/types.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libhttp
)

# Install core headers
install(FILES
    include/libhttp/core/request.h
    include/libhttp/core/response.h
    include/libhttp/core/detail.h
    include/libhttp/core/session.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libhttp/core
)

# Install feature headers
install(FILES
    include/libhttp/features/auth.h
    include/libhttp/features/json.h
    include/libhttp/features/stream.h
    include/libhttp/features/http2.h
    include/libhttp/features/websocket.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libhttp/features
)

# Install utility headers
install(FILES
    include/libhttp/utils/exceptions.h
    include/libhttp/utils/status.h
    include/libhttp/utils/connection_pool.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libhttp/utils
)

install(TARGETS libhttp
    EXPORT libhttp-targets
)

install(EXPORT libhttp-targets
    FILE libhttp-targets.cmake
    NAMESPACE libhttp::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libhttp
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/libhttp-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/libhttp-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/libhttp-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libhttp
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libhttp-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/libhttp-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libhttp
)

# Enable testing
enable_testing()

# Add examples as optional subdirectory
option(LIBHTTP_BUILD_EXAMPLES "Build libhttp examples" OFF)
if(LIBHTTP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Add tests
add_subdirectory(tests)
