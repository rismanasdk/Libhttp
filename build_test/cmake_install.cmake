# Install script for directory: /home/risman-hadinata/Documents/github_repo/lib-http

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libhttp" TYPE FILE FILES
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/libhttp.hpp"
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/types.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libhttp/core" TYPE FILE FILES
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/core/request.h"
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/core/response.h"
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/core/detail.h"
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/core/session.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libhttp/features" TYPE FILE FILES
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/features/auth.h"
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/features/json.h"
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/features/stream.h"
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/features/http2.h"
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/features/websocket.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/libhttp/utils" TYPE FILE FILES
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/utils/exceptions.h"
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/utils/status.h"
    "/home/risman-hadinata/Documents/github_repo/lib-http/include/libhttp/utils/connection_pool.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libhttp/libhttp-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libhttp/libhttp-targets.cmake"
         "/home/risman-hadinata/Documents/github_repo/lib-http/build_test/CMakeFiles/Export/7453c3cde7eddf495ba21d74baf54a30/libhttp-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libhttp/libhttp-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/libhttp/libhttp-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/libhttp" TYPE FILE FILES "/home/risman-hadinata/Documents/github_repo/lib-http/build_test/CMakeFiles/Export/7453c3cde7eddf495ba21d74baf54a30/libhttp-targets.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/libhttp" TYPE FILE FILES
    "/home/risman-hadinata/Documents/github_repo/lib-http/build_test/libhttp-config.cmake"
    "/home/risman-hadinata/Documents/github_repo/lib-http/build_test/libhttp-config-version.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/risman-hadinata/Documents/github_repo/lib-http/build_test/examples/cmake_install.cmake")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for the subdirectory.
  include("/home/risman-hadinata/Documents/github_repo/lib-http/build_test/tests/cmake_install.cmake")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "/home/risman-hadinata/Documents/github_repo/lib-http/build_test/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
