cmake_minimum_required(VERSION 3.15)
project(libhttp VERSION 2.0.0 LANGUAGES CXX)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# Header-only library
add_library(libhttp INTERFACE)
add_library(libhttp::libhttp ALIAS libhttp)

target_include_directories(libhttp INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

target_compile_features(libhttp INTERFACE cxx_std_11)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

target_link_libraries(libhttp INTERFACE
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
)

install(FILES
    http.h
    http_auth.h
    http_detail.h
    http_exception.h
    http_json.h
    http_request.h
    http_response.h
    http_session.h
    http_status.h
    http_stream.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
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
