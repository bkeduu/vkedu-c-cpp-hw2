cmake_minimum_required(VERSION 3.0.0)
project(num_lib)

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-g -O0 -fprofile-arcs -ftest-coverage -fPIC -Wall -Werror -Wpedantic")

option(BUILD_STATIC OFF)
option(BUILD_SHARED OFF)

message("BUILD_STATIC: ${BUILD_STATIC}")
message("BUILD_SHARED: ${BUILD_SHARED}")

file(GLOB SOURCES src/*.c)
file(GLOB INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/inc)

message("SOURCES = ${SOURCES}")
message("INCLUDE_DIRS = ${INCLUDE_DIRS}")

if(BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
elseif(BUILD_STATIC)
    set(CMAKE_C_FLAGS "-D BUILD_STATIC ${CMAKE_C_FLAGS}")
    add_library(${PROJECT_NAME} STATIC ${SOURCES})
else()
    add_library(${PROJECT_NAME} SHARED ${SOURCES})
endif(BUILD_SHARED)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE_DIRS})

set(NUM_LIB_LIBRARIES ${PROJECT_NAME} PARENT_SCOPE)
set(NUM_LIB_INCLUDE_DIRS ${INCLUDE_DIRS} PARENT_SCOPE)
