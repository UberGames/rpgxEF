cmake_minimum_required(VERSION 3.10)
project(rpgxEF LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(BUILD_STATIC false CACHE BOOL "whether to create static binaries")
if(WIN32)
    set(BUILD_STATIC true CACHE BOOL "whether to create static binaries")
endif(WIN32)

add_subdirectory(code)
