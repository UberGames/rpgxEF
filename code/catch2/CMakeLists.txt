cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

add_library(catch2_main STATIC runner.cpp)
#set_property(TARGET catch2_main PROPERTY POSITION_INDEPENDENT_CODE ON)

add_library(catch2_symbols STATIC UnitTest.cpp)
set_property(TARGET catch2_symbols PROPERTY POSITION_INDEPENDENT_CODE ON)