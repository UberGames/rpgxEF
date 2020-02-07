cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

file(GLOB BASE_GAME_SRC "*.h" "*.c" "*.cpp")

add_library(base_game STATIC ${BASE_GAME_SRC})
target_compile_options(base_game PUBLIC -Wno-write-strings)
target_include_directories(base_game PUBLIC ..)
set_property(TARGET base_game PROPERTY POSITION_INDEPENDENT_CODE ON)
