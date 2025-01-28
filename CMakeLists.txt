cmake_minimum_required(VERSION 3.22)
project(QLeverTest C CXX)


# C/C++ Versions
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Add colored output for Ninja
if ("${CMAKE_GENERATOR}" STREQUAL "Ninja")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        add_compile_options(-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        add_compile_options(-fcolor-diagnostics)
    endif ()
endif ()

include(FetchContent)


FetchContent_Declare(
        range-v3
        GIT_REPOSITORY https://github.com/joka921/range-v3
        GIT_TAG 1dc0b09abab1bdc7d085a78754abd5c6e37a5d0c # 0.12.0
)

FetchContent_MakeAvailable(range-v3)

include_directories(${range-v3_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Wall -Wextra")
# Enable the specification of additional compiler flags manually from the commandline
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ADDITIONAL_COMPILER_FLAGS}")

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -save-temps=obj")

set(USE_CPP_17_BACKPORTS OFF CACHE BOOL "Use the C++17 backports (range-v3 and enable_if_t instead of std::ranges and concepts)")
if (${USE_CPP_17_BACKPORTS})
    add_definitions("-DQLEVER_CPP_17 -DCPP_CXX_CONCEPTS=0")
endif()


add_executable(QLeverTest main.cpp)

