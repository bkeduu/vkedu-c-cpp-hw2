cmake_minimum_required(VERSION 3.0.0)
project(numbers)

add_subdirectory(num_lib)
#set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-g -fprofile-arcs -ftest-coverage -fPIC -O0 -Wall -Werror -Wpedantic")
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-g -O0 -Wall -Werror -Wpedantic")

add_executable(${PROJECT_NAME} numbers.c)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lm -coverage -lgcov" )

target_include_directories(${PROJECT_NAME} PUBLIC ${NUM_LIB_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${NUM_LIB_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE m)

#add_subdirectory(tests)
