cmake_minimum_required(VERSION 2.6)
project(leptjson_test)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi -pedar -Wall")
endif()

add_library(leptjson leptjson.cpp)
add_executable(leptjson_test test.cpp)
target_link_libraries(leptjson_test leptjson)