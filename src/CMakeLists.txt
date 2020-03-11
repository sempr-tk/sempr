pkg_check_modules(Soprano REQUIRED soprano)
include_directories(${Soprano_INCLUDE_DIRS})
link_directories(${Soprano_LIBRARY_DIRS})
# message("SOPRANO: ${Soprano_INCLUDE_DIRS}")

#find_package(GDAL)
#if(GDAL_FOUND)
#    include_directories(${GDAL_INCLUDE_DIR})
#    link_directories(${GDAL_LIBRARY})
#endif(GDAL_FOUND)


find_package(OpenMP)
if(OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


#file(GLOB_RECURSE SRC_FILES core/* data/* entity/* processing/* query/* storage/*)
#file(GLOB_RECURSE ODB_FILES
#    ../include/sempr/storage/DBObject.hpp
#    ../include/sempr/storage/History.hpp
#    ../include/sempr/storage/Dummy.hpp
#    ../include/sempr/data/*
#    ../include/sempr/entity/*
#)

set(SRC_FILES
    Core.cpp
    RDF.cpp
    SimpleIncrementalIDGenerator.cpp
    Entity.cpp
    ECAccessors.cpp
    Component.cpp
    ./component/AffineTransform.cpp
    ./component/GeosGeometry.cpp
    ./component/TripleContainer.cpp
    ./component/TriplePropertyMap.cpp
    ./component/TripleVector.cpp
    ./nodes/AffineTransformNodes.cpp
    ./nodes/AffineTransformBuilders.cpp
    ./nodes/ECNode.cpp
    ./nodes/ExtractTriples.cpp
    ./nodes/ExtractTriplesBuilder.cpp
    ./nodes/SopranoModule.cpp
    ./nodes/SopranoNodeBuilder.cpp
)

set(ODB_FILES
    ../include/Entity.hpp
    ../include/Component.hpp
)

# message( "found files: ${SRC_FILES}")
# message( "found files: ${ODB_FILES}")

# There are two variables to be set:
# 1. the odb-compiler needs to know which database is used. e.g. "sqlite"
# 2. the traits that implement serialization of new datatypes are selected based on the
#    database, too, and need a definition of e.g. DATABASE_SQLITE.
# So, we need to set both, and also pass our choice on to extensions of sempr.
# - based on the "DATABASE" variable also set the definition of "DATABASE_*"
# - add the DATABASE-variable to the pkgconfig-file. Extensions can use this to set the parameter
#   for the odb-compiler.
# - TODO: currently we also include -DDATABASE_* in the pkgconfig, so this is a bit redundant,
#   isn't it?
set(DATABASE sqlite)
if (${DATABASE} MATCHES sqlite)
    set(CONFIG_DATABASE DATABASE_SQLITE)
# elseif()
endif()

# ODB_GENERATED = list of _odb.h files
# ODB needs the include dirs, too
# in addition to the projects include dir
# header prologue: dataype serialization definitions
# X_CPP_OPTION (passed to underlying c++ compiler): -fPIC for Qt5-support!
#odb_compile(ODB_GENERATED
#    FILES ${ODB_FILES}
#    #    HEADER_PROLOGUE ${PROJECT_SOURCE_DIR}/include/sempr/storage/traits.hxx
#    INCLUDE ../include ${Soprano_INCLUDE_DIRS} ${GDAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS} ${RETE_INCLUDE_DIRS}
#    # PROFILE boost/uuid
#    PROJECT_INCLUDE ${PROJECT_SOURCE_DIR}/include
#    X_CPP_OPTION -fPIC
#    DB ${DATABASE} GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA STANDARD c++11)

list(APPEND SRC_FILES ${ODB_GENERATED})
#message("ODB_GENERATED: ${ODB_GENERATED}")

include_directories(${ODB_COMPILE_OUTPUT_DIR})
# message("ODB_COMPILE_OUTPUT_DIR: ${ODB_COMPILE_OUTPUT_DIR}")
# message("SOURCE FILES: ${SRC_FILES}")
add_library(sempr_core SHARED ${SRC_FILES})
target_link_libraries(sempr_core ${ODB_LIBRARIES} ${Soprano_LIBRARIES} ${GEOS_LIBRARY} ${GeographicLib_LIBRARIES} ${RETE_LIBRARIES})
target_compile_definitions(sempr_core PUBLIC -D${CONFIG_DATABASE})
# target_include_directories(sempr_core
    # PRIVATE ${ODB_COMPILE_OUTPUT_DIR} )


## install stuff
install(
    TARGETS sempr_core
    LIBRARY DESTINATION lib/sempr COMPONENT common
)
# headers.
# maybe having the "include/sempr" folder wasn't such a good idea, after all. Makes things a bit
# weird. what shall the dir structure look like?
# lets make it look like this:
    # .../include/
    # .../include/sempr/
    # .../include/sempr/core
    # .../include/sempr/entity
    # .../include/sempr/...
    # .../include/sempr-agri
    # .../include/sempr-agri/...
install(
    DIRECTORY ../include/
    DESTINATION include/sempr COMPONENT common
)

# odb-generated headers
# well... where to put them? how to include them?
# includes are currently used directly: #include <Point_odb.h>
# TODO: should we change that?
# just move them to a common folder: .../include/sempr/odb_gen
# and add the path to the include dirs in the config file
install(
    DIRECTORY ${ODB_COMPILE_OUTPUT_DIR}
    DESTINATION include/sempr COMPONENT common
    FILES_MATCHING PATTERN "*.h"
)

# pkg-config-file
set(SEMPR_INSTALL_PREFIX "/usr")
configure_file("sempr-core.pc.in" "sempr-core.pc" @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/sempr-core.pc"
    DESTINATION lib/pkgconfig
    COMPONENT pkgconfig-deb
)

set(SEMPR_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
configure_file("sempr-core.pc.in" "sempr-core-dev.pc" @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/sempr-core-dev.pc"
    DESTINATION lib/pkgconfig
    COMPONENT pkgconfig-cmake
)
