cmake_minimum_required(VERSION 2.8)

project(sempr)
set(CMAKE_CXX_FLAGS "-std=c++11")
include_directories(include)

add_subdirectory(src)
add_subdirectory(test)

enable_testing()
add_test(NAME test1 COMMAND unittest)
