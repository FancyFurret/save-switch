cmake_minimum_required(VERSION 3.19)
project(save_switch)

set(CMAKE_CXX_STANDARD 20)
include_directories(.)

# Re-enable exceptions since switch-cmake disables them
string(REPLACE "-fno-exceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
string(REPLACE "-fno-rtti" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DSS_DEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSS_DEBUG")

if (SWITCH)
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/lib/switch-cmake/cmake)
    find_package(LIBNX REQUIRED)
    include_directories(${LIBNX_INCLUDE_DIRS})
    include_directories(${DEVKITPRO}/portlibs/switch/include)
endif ()

include_directories(lib/json/include)
add_subdirectory(lib/json)

include_directories(lib/cereal/include)
add_subdirectory(lib/cereal)

include_directories(lib/fmt/include)
add_subdirectory(lib/fmt)

add_subdirectory(save_switch_common)

if (SWITCH)
    add_subdirectory(save_switch_nx)
endif ()
