cmake_minimum_required(VERSION 3.14) 
project(leptjson_test)

set(CMAKE_CXX_STANDARD 17)

set(headers
	third-party/double-conversion/bignum.h
	third-party/double-conversion/bignum-dtoa.h
	third-party/double-conversion/cached-powers.h
	third-party/double-conversion/diy-fp.h
	third-party/double-conversion/double-conversion.h
	third-party/double-conversion/fast-dtoa.h
	third-party/double-conversion/double-to-string.h
	third-party/double-conversion/fixed-dtoa.h
	third-party/double-conversion/ieee.h
	third-party/double-conversion/string-to-double.h
	third-party/double-conversion/strtod.h
	third-party/double-conversion/utils.h

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

target_include_directories(leptjson PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(leptjson PRIVATE double-conversion) 
add_executable(leptjson_test test.cpp) 
target_link_libraries(leptjson_test PRIVATE leptjson) 
target_include_directories(leptjson_test PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}
)

include(GoogleTest)
gtest_discover_tests(g-test)