cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

add_library(catch2_main OBJECT runner.cpp)
add_library(catch2_symbols OBJECT UnitTest.cpp)