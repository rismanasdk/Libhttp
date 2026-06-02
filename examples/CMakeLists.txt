cmake_minimum_required(VERSION 3.15)
project(libhttp_examples)

# Find required packages
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Use local libhttp instead of system installed
set(libhttp_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Simple GET example
add_executable(01_simple_get 01_simple_get.cpp)
target_include_directories(01_simple_get PRIVATE ${libhttp_INCLUDE_DIRS})
target_link_libraries(01_simple_get PRIVATE OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)

# Session example
add_executable(02_session 02_session.cpp)
target_include_directories(02_session PRIVATE ${libhttp_INCLUDE_DIRS})
target_link_libraries(02_session PRIVATE OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)

# JSON API example
add_executable(03_json_api 03_json_api.cpp)
target_include_directories(03_json_api PRIVATE ${libhttp_INCLUDE_DIRS})
target_link_libraries(03_json_api PRIVATE OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)

# Authentication example
add_executable(04_authentication 04_authentication.cpp)
target_include_directories(04_authentication PRIVATE ${libhttp_INCLUDE_DIRS})
target_link_libraries(04_authentication PRIVATE OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)

# Streaming example
add_executable(05_streaming 05_streaming.cpp)
target_include_directories(05_streaming PRIVATE ${libhttp_INCLUDE_DIRS})
target_link_libraries(05_streaming PRIVATE OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)

# HTTP/2 example (optional - may not compile if HTTP/2 not fully implemented)
add_executable(06_http2 06_http2.cpp)
target_include_directories(06_http2 PRIVATE ${libhttp_INCLUDE_DIRS})
target_link_libraries(06_http2 PRIVATE OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)

# WebSocket example (optional - may not compile if WebSocket not fully implemented)
add_executable(07_websocket 07_websocket.cpp)
target_include_directories(07_websocket PRIVATE ${libhttp_INCLUDE_DIRS})
target_link_libraries(07_websocket PRIVATE OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)
