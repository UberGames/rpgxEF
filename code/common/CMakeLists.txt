cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

file(GLOB COMMON_SRC "*.h" "*.c" "*.cpp")

add_library(common OBJECT ${COMMON_SRC})
set_property(TARGET common PROPERTY POSITION_INDEPENDENT_CODE ON)
