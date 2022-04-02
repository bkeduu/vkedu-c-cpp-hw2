cmake_minimum_required(VERSION 3.0.0)
project(num_lib_common)

set(CMAKE_C_COMPILER /usr/bin/gcc)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=gnu11 -g -O0 -fprofile-arcs -ftest-coverage -fPIC -Wall -Werror -Wpedantic")

file(GLOB SOURCES *.c)
file(GLOB INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE m)

set(NUM_LIB_LIBRARIES_COMMON ${PROJECT_NAME} PARENT_SCOPE)
set(NUM_LIB_COMMON_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

