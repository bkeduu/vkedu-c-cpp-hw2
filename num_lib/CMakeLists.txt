cmake_minimum_required(VERSION 3.0.0)
project(num_lib)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-g -O0 -Wall -Werror -Wpedantic")
#-fprofile-arcs -ftest-coverage -fPIC

file(GLOB SOURCES src/*.c)
file(GLOB INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/inc)

message("INCLUDE_DIRS = ${INCLUDE_DIRS}")

add_library(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

set(NUM_LIB_LIBRARIES ${PROJECT_NAME} PARENT_SCOPE)
set(NUM_LIB_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)
