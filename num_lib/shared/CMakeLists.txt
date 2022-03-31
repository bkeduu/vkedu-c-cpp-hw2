cmake_minimum_required(VERSION 3.0.0)
project(num_lib_shared)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-g -O0 -fprofile-arcs -ftest-coverage -fPIC -Wall -Werror -Wpedantic")

file(GLOB SOURCES *.c)
file(GLOB INCLUDE_DIRS ${NUM_LIB_COMMON_INCLUDE_DIRS})

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

set(NUM_LIB_LIBRARIES_SHARED ${PROJECT_NAME} PARENT_SCOPE)
set(NUM_LIB_SHARED_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)

