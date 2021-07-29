# CMakeList.txt: test1 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
set(CMAKE_CXX_STANDARD 17)
cmake_minimum_required (VERSION 3.8)

project ("test2")

# 将源代码添加到此项目的可执行文件。
add_executable (test1 "test2.cpp" "test2.h" )

# TODO: 如有需要，请添加测试并安装目标。
include_directories ($ENV{CPP_EXTRA_INCLUDE})