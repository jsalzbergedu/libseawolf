cmake_minimum_required(VERSION 3.1.3)
project(libseawolf)
set(CMAKE_CXX_STANDARD 99)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -pedantic -Wmissing-prototypes -D_GNU_SOURCE")
set(Python_ADDITIONAL_VERSIONS 2.7 2.6 2.5)
find_package(PythonInterp REQUIRED)

set(LIBSEAWOLF_TOPLEVEL "${CMAKE_CURRENT_SOURCE_DIR}" CACHE INTERNAL "")

# Find which system this is being run on
set(OS_LINUX_P 0)
set(OS_DARWIN_P 0)
set(OS_FREEBSD_P 0)
set(OS_OPENBSD_P 0)
set(OS_NETBSD_P 0)
set(OS_NONE_P 0)
message("System name is " ${CMAKE_SYSTEM_NAME})
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  message("Detected OS: Linux")
  set(OS_LINUX_P 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  message("Detected OS: Darwin")
  set(OS_DARWIN_P 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
  message("Detected OS: FreeBSD")
  set(OS_FREEBSD_P 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "OpenBSD")
  message("Detected OS: OpenBSD")
  set(OS_OPENBSD_P 1)
elseif(CMAKE_SYSTEM_NAME STREQUAL "NetBSD")
  message("Detected OS: NetBSD")
  set(OS_NETBSD_P 1)
else()
  message("No OS detected")
  set(OS_NONE_P 1)
endif()

set(DEFAULT_LIBRARIES "" CACHE INTERNAL "")

if(OS_LINUX_P)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__SW_Linux__")
elseif(OS_DARWIN_P)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__SW_DARWIN__")
  set(DEFAULT_LIBRARIES "${DEFAULT_LIBRARIES} rt")
elseif(OS_FREEBSD_P)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__SW_FreeBSD__")
  set(DEFAULT_LIBRARIES "${DEFAULT_LIBRARIES} rt")
elseif(OS_OPENBSD_P)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D__SW_OpenBSD__")
elseif(OS_NETBSD_P)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-char-subscripts -D__SW_NetBSD__")
  set(DEFAULT_LIBRARIES "${DEFAULT_LIBRARIES} rt")
endif()

## Uninstall
# Add uninstall target, taken from the cmake wiki:
# https://cmake.org/Wiki/CMake_FAQ#Can_I_do_.22make_uninstall.22_with_CMake.3F
if(NOT TARGET uninstall)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()


# Header files 
include_directories("include")
include_directories("include/seawolf")
add_subdirectory(src)
