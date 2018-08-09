cmake_minimum_required(VERSION 2.8)

project(sempr)
set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC")
add_definitions(-Wall -Wno-unknown-pragmas)


# compile-output-dir!
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")

# allow includes of current dir.
set(CMAKE_INCLUDE_CURRENT_DIR ON)

##
# find packages that are used in src and test:
##

# add pkg-config support
find_package(PkgConfig REQUIRED)

# find package soprano
pkg_check_modules(Soprano REQUIRED soprano)
include_directories(${Soprano_INCLUDE_DIRS})
link_directories(${Soprano_LIBRARY_DIRS})

# find package odb
find_package(ODB REQUIRED COMPONENTS sqlite boost)
include_directories(${ODB_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)
include(${ODB_USE_FILE})
include_directories(${ODB_COMPILE_OUTPUT_DIR})

# libodb-sqlite-dev does not specify a dependency to libsqlite3-dev,
# so lets check that manually. We just need the include dir set to
# find 'sqlite3.h'. See issue #48
pkg_check_modules(SQLITE REQUIRED sqlite3)
include_directories(${SQLITE_LIBRARY_DIRS})

# find package eigen3
pkg_check_modules(EIGEN REQUIRED eigen3)
include_directories(${EIGEN_INCLUDE_DIRS})

#find package geos
find_package(GEOS)
include_directories(${GEOS_INCLUDE_DIR})
#link_directories(${GEOS_LIBRARY})  

#find pacakge GeographicLib
set(GeographicLib_USE_STATIC_LIBS ON) 
find_package (GeographicLib)
include_directories (${GeographicLib_INCLUDE_DIRS})
add_definitions (${GeographicLib_DEFINITIONS})

include_directories(include)

add_subdirectory(src)


enable_testing()
add_subdirectory(test)
