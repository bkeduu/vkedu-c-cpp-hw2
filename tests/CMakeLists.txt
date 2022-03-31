cmake_minimum_required(VERSION 3.14)
project(num_test)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fprofile-arcs -ftest-coverage -g -O0")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -coverage -lgcov -pthread")

file(GLOB SOURCES *.cpp)

find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${NUM_LIB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} ${NUM_LIB_LIBRARIES} GTest::GTest gtest_main)
