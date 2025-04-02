cmake_minimum_required(VERSION 3.14) 
project(leptjson_test)

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()


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

add_executable(g-test g-test.cc)
target_link_libraries(
    g-test
    GTest::gtest_main
    leptjson
)

include(GoogleTest)
gtest_discover_tests(g-test)