cmake_minimum_required(VERSION 3.10)
project(libgraph)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 收集所有源文件
file(GLOB SOURCES "./src/*.cpp")

# 创建测试可执行文件
add_executable(geojson_converter_test ${SOURCES})

# 包含头文件目录
target_include_directories(geojson_converter_test PRIVATE "./include" )

# 链接数学库(Haversine公式需要)
if(UNIX)
    target_link_libraries(geojson_converter_test m)
endif()

# 对于Windows平台，确保正确链接
if(WIN32)
    # 添加Windows特定编译选项
    target_compile_options(geojson_converter_test PRIVATE /EHsc)
    # 添加宏定义
    add_definitions(-DROUTING_KIT_NO_POSIX)
endif()