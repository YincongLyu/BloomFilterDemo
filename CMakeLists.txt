cmake_minimum_required(VERSION 3.16)

project(bloomstore)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# gtest dependency
include(FetchContent)
FetchContent_Declare(
    gtest
    URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(gtest)
# 开启编译优化
add_compile_options(-Og -g)

# -- Library implemented here
file(GLOB SRC "src/*.cc")
add_library(bloomstore SHARED ${SRC})

target_include_directories(
    bloomstore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include> 
        $<INSTALL_INTERFACE:include>
        # CMake 生成器表达式指定 build_tree 和 install_tree包含的目录
)

# -- Benchmarking stuff implemented here
add_executable(main main.cpp)
target_link_libraries(
    main
        bloomstore
)

# -- Test stuff implemented here
add_executable(bloom_filter_test test/bloom_filter_test.cc)
target_link_libraries(
    bloom_filter_test
        bloomstore
        gtest_main
)

add_executable(bloom_buffer_test test/bloom_buffer_test.cc)
target_link_libraries(
    bloom_buffer_test
        bloomstore
        gtest_main
)