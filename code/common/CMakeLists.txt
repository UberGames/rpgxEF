cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

# check for swig
find_package(SWIG 4.0)
if (NOT ${SWIG_FOUND})
    message(STATUS test)
endif (NOT ${SWIG_FOUND})

# setup swig generation
file(GLOB_RECURSE SWIG_INTERFACES "${CMAKE_CURRENT_SOURCE_DIR}/*.i")
foreach (SWIG_INTERFACE ${SWIG_INTERFACES})
    execute_process(
            COMMAND ${SWIG_EXECUTABLE} -c++ -lua ${SWIG_INTERFACE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            RESULT_VARIABLE SWIG_RESULT
            ERROR_VARIABLE SWIG_ERROR
    )
    if (NOT ${SWIG_RESULT} EQUAL "0")
        message(FATAL_ERROR ${SWIG_ERROR})
    endif (NOT ${SWIG_RESULT} EQUAL "0")
    get_filename_component(SWIG_DIR ${SWIG_INTERFACE} DIRECTORY)
    get_filename_component(SWIG_BASENAME ${SWIG_INTERFACE} NAME_WE)
    add_custom_command(
            OUTPUT ${SWIG_DIR}/${SWIG_BASENAME}_wrap.cxx
            COMMAND ${SWIG_EXECUTABLE} -c++ -lua ${SWIG_INTERFACE}
            DEPENDS ${SWIG_INTERFACE}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set(SWIG_IMPL_SOURCES ${SWIG_IMPL_SOURCES} ${SWIG_DIR}/${SWIG_BASENAME}.h ${SWIG_DIR}/${SWIG_BASENAME}.cpp)
endforeach (SWIG_INTERFACE ${SWIG_INTERFACES})
file(GLOB_RECURSE SWIG_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*_wrap.cxx")

file(GLOB COMMON_SRC "*.h" "*.c" "*.cpp")

add_library(common STATIC ${COMMON_SRC} ${SWIG_SOURCES})
target_include_directories(common PUBLIC ..)
set_property(TARGET common PROPERTY POSITION_INDEPENDENT_CODE ON)

add_executable(unittest_common ${COMMON_SRC})
target_link_libraries(unittest_common PUBLIC catch2_main)
target_include_directories(unittest_common PUBLIC ..)
target_compile_definitions(unittest_common PUBLIC UNIT_TEST)
set_property(TARGET unittest_common PROPERTY POSITION_INDEPENDENT_CODE ON)
