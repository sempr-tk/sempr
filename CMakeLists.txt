cmake_minimum_required(VERSION 3.5)

project(sempr VERSION 0.3.1)

set(CMAKE_CXX_FLAGS "-std=c++11 -fPIC")
add_definitions(-Wall -Wno-unknown-pragmas -Wextra -pedantic -Wnon-virtual-dtor)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-undefined")

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

# everything shall be connected through a rule based forward reasoner, based on the RETE algorithm
pkg_check_modules(RETE REQUIRED rete)
include_directories(${RETE_INCLUDE_DIRS})
link_directories(${RETE_LIBRARY_DIRS})
add_definitions(${RETE_CXX_FLAGS_OTHER})

include_directories(include)

add_subdirectory(src)


enable_testing()
add_subdirectory(test)


# configure cpack
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "modular semantic environment representation based on a rete reasoner")
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Nils Niemann <nils.niemann@dfki.de>")
set(CPACK_COMPONENTS_ALL "common;pkgconfig-deb") # excludes the sempr-core-dev.pc
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL_IN_ONE_PACKAGE ON)

# when run in a CI environment, the deps should have been installed through
# debian packages and SHLIBDEBS should work to find the dependencies.
# Locally this lead to errors since I have a lot of stuff compiled from source
# (soprano with qt5 ...)
if (DEFINED ENV{CI})
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
    # somehow SHLIBDEPS does not add rete as a dependency (but does not complain
    # about not finding it, either), so add it manually.
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "rete (>= 0.2.0)")
else()
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF)
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "rete (>= 0.2.0), libodb-dev, libodb-sqlite-2.4, libodb-boost-2.4, libsqlite3-dev, libsoprano-dev, libboost-all-dev, libgeos-dev, libgeos++-dev, libgeographic-dev, libeigen3-dev")
endif()

include(CPack)
