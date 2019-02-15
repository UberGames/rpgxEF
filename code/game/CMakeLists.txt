cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
    set(ARCH "x86_64")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "amd64")
    set(ARCH "x86_64")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
    if (CMAKE_CL_64)
        set(ARCH "x86_64")
    else ()
        set(ARCH "x86")
    endif ()
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86")
    set(ARCH "x86")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
    set(ARCH "x86")
elseif (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i686")
    set(ARCH "x86")
else ()
    message(FATAL_ERROR "Unknown processor:" ${CMAKE_SYSTEM_PROCESSOR})
endif ()

set(GAME_LIB_NAME "qagame${ARCH}")

file(GLOB_RECURSE GAME_SRC "*.h" "*.c" "*.cpp")


add_library(${GAME_LIB_NAME} SHARED ${GAME_SRC})
target_link_libraries(${GAME_LIB_NAME} base_game)
target_include_directories(${GAME_LIB_NAME} PUBLIC ../lua/include ../json/include)
target_compile_options(${GAME_LIB_NAME} PUBLIC -fpermissive)
set_target_properties(${GAME_LIB_NAME} PROPERTIES PREFIX "")
