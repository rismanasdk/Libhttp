# Minimal CMakeLists for tests
cmake_minimum_required(VERSION 3.15)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

# Set include directories
set(libhttp_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include)

# Core tests
add_executable(test_core test_core.cpp)
target_include_directories(test_core PRIVATE ${libhttp_INCLUDE_DIRS})
target_link_libraries(test_core PRIVATE OpenSSL::SSL OpenSSL::Crypto ZLIB::ZLIB)

# Register tests with CTest
add_test(NAME core_tests COMMAND test_core)

