cmake_minimum_required(VERSION 3.5)

project(sempr VERSION 0.8.2)

set(CMAKE_CXX_STANDARD 11)

add_definitions(-Wall -Wno-unknown-pragmas -Wextra -pedantic -Wnon-virtual-dtor -Werror=unused-result)
add_definitions(-fPIC)

# make serialization thread safe (i.e., allow serialization into multiple
# different archives at the same time -- not access to a single archive!)
add_definitions(-DCEREAL_THREAD_SAFE=1)

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

find_package(Cereal REQUIRED)

# everything shall be connected through a rule based forward reasoner, based on the RETE algorithm
pkg_check_modules(RETE REQUIRED rete>=0.2.4)
include_directories(${RETE_INCLUDE_DIRS})
link_directories(${RETE_LIBRARY_DIRS})
add_definitions(${RETE_CXX_FLAGS_OTHER})

# use boost filesystem instead of std filesystem and step back from using
# c++17,... for now.
find_package(Boost REQUIRED COMPONENTS filesystem)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# QtCore for its plugin system

find_package(Qt5 COMPONENTS Core)
if (NOT Qt5_FOUND)
    message("Couldn't find Qt5. Looking for Qt4 instead...")
    find_package(Qt4 COMPONENTS QtCore REQUIRED)
    set(QT_CORE Qt4::QtCore)
    add_definitions(-DQT4)
else()
    set(QT_CORE Qt5::Core)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

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
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS ON)
set(CPACK_DEBIAN_PACKAGE_GENERATE_SHLIBS_POLICY ">=")

include(CPack)
