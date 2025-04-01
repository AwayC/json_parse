cmake_minimum_required(VERSION 3.10) 
project(leptjson_test)

file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/latest/download/CPM.cmake 
     ${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CPM.cmake)

CPMAddPackage(
    NAME double-conversion
    GITHUB_REPOSITORY google/double-conversion
    VERSION 3.3.0
    OPTIONS          # 新增构建选项
        "-DBUILD_SHARED_LIBS=OFF"
        "-DBUILD_TESTING=OFF"
        "-DCMAKE_POSITION_INDEPENDENT_CODE=ON"
)

add_library(leptjson leptjson.cpp leptjson.h)
target_include_directories(leptjson PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_link_libraries(leptjson PRIVATE 
    double-conversion
)

# 测试可执行文件
add_executable(leptjson_test test.cpp)
target_link_libraries(leptjson_test PRIVATE 
    leptjson
)

