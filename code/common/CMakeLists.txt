cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)

file(GLOB COMMON_SRC "*.h" "*.c" "*.cpp")

add_library(common OBJECT ${COMMON_SRC})
