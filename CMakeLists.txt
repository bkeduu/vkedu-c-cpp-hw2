cmake_minimum_required(VERSION 3.0.0)
project(numbers)

add_subdirectory(num_lib)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=gnu11 -g -fprofile-arcs -ftest-coverage -fPIC -O0 -Wall -Werror -Wpedantic")

add_executable(${PROJECT_NAME}_shared numbers.c)
add_executable(${PROJECT_NAME}_static numbers.c)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm -coverage -lgcov -pthread")

target_include_directories(${PROJECT_NAME}_shared PUBLIC ${NUM_LIB_COMMON_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_shared PRIVATE ${NUM_LIB_LIBRARIES_COMMON})
target_link_libraries(${PROJECT_NAME}_shared PRIVATE ${NUM_LIB_LIBRARIES_SHARED})

target_include_directories(${PROJECT_NAME}_static PUBLIC ${NUM_LIB_COMMON_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME}_static PRIVATE ${NUM_LIB_LIBRARIES_COMMON})
target_link_libraries(${PROJECT_NAME}_static PRIVATE ${NUM_LIB_LIBRARIES_STATIC})

add_subdirectory(tests)