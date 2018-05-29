cmake_minimum_required(VERSION 2.8)

project(sempr)
set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC")
add_definitions(-Wall -Wno-unknown-pragmas)

# since odb is used in both src and test, find it here for a consistent
# compile-output-dir!
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
find_package(ODB REQUIRED COMPONENTS sqlite boost)
include_directories(${ODB_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)
include(${ODB_USE_FILE})
include_directories(${ODB_COMPILE_OUTPUT_DIR})


include_directories(include)

add_subdirectory(src)


enable_testing()
add_subdirectory(test)
