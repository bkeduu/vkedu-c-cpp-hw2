cmake_minimum_required(VERSION 3.14)
project(num_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fprofile-arcs -ftest-coverage -g -O0")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -coverage -lgcov -pthread")

file(GLOB SOURCES_STATIC test_static.cpp)
file(GLOB SOURCES_SHARED test_shared.cpp)

find_package(GTest REQUIRED)
find_library(${NUM_LIB_LIBRARIES_COMMON} REQUIRED)
find_library(${NUM_LIB_LIBRARIES_SHARED} REQUIRED)
find_library(${NUM_LIB_LIBRARIES_STATIC} REQUIRED)

add_executable(${PROJECT_NAME}_static ${SOURCES_STATIC})
add_executable(${PROJECT_NAME}_shared ${SOURCES_SHARED})


target_include_directories(${PROJECT_NAME}_shared PUBLIC ${NUM_LIB_COMMON_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME}_shared PUBLIC ${NUM_LIB_SHARED_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_shared PRIVATE ${NUM_LIB_LIBRARIES_COMMON})
target_link_libraries(${PROJECT_NAME}_shared PRIVATE ${NUM_LIB_LIBRARIES_SHARED})
target_link_libraries(${PROJECT_NAME}_shared PRIVATE m)
target_link_libraries(${PROJECT_NAME}_shared PRIVATE ${NUM_LIB_LIBRARIES_COMMON} GTest::GTest gtest_main)

target_include_directories(${PROJECT_NAME}_static PUBLIC ${NUM_LIB_COMMON_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME}_static PUBLIC ${NUM_LIB_STATIC_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_static PRIVATE ${NUM_LIB_LIBRARIES_COMMON})
target_link_libraries(${PROJECT_NAME}_static PRIVATE ${NUM_LIB_LIBRARIES_STATIC})
target_link_libraries(${PROJECT_NAME}_static PRIVATE m)
target_link_libraries(${PROJECT_NAME}_static PRIVATE ${NUM_LIB_LIBRARIES_COMMON} GTest::GTest gtest_main)
