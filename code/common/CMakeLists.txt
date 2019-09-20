cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

file(GLOB COMMON_SRC "*.h" "*.c" "*.cpp")

add_library(common OBJECT ${COMMON_SRC})
target_include_directories(common PUBLIC ..)
set_property(TARGET common PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(unittest_common ${COMMON_SRC} $<TARGET_OBJECTS:catch2_main>)
target_include_directories(unittest_common PUBLIC ..)
target_compile_definitions(unittest_common PUBLIC UNIT_TEST)
set_property(TARGET unittest_common PROPERTY POSITION_INDEPENDENT_CODE ON)
