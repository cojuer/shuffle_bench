cmake_minimum_required(VERSION 3.10)
project(bench_rng)
include(CheckCXXCompilerFlag)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
find_package(spdlog REQUIRED)

include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        release-1.12.1
)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(googletest googlebenchmark)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()

enable_cxx_compiler_flag_if_supported("-std=c++20")
enable_cxx_compiler_flag_if_supported("-O2")
enable_cxx_compiler_flag_if_supported("-g")
enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-Wshadow")
enable_cxx_compiler_flag_if_supported("-Wnon-virtual-dtor")

file(GLOB SOURCES src/*.hpp src/*.cpp third-party/*.hpp)

SET(CMAKE_INCLUDE_PATH ${CMAKE_LIBRARY_PATH} "/usr/local/include")

add_executable(brng main.cpp ${SOURCES})

target_include_directories(brng PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(brng PRIVATE benchmark::benchmark)
target_link_libraries(brng PRIVATE spdlog::spdlog)