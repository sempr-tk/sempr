pkg_check_modules(Soprano REQUIRED soprano)
include_directories(${Soprano_INCLUDE_DIRS})
# message("SOPRANO: ${Soprano_INCLUDE_DIRS}")

pkg_check_modules(GDAL REQUIRED gdal)
include_directories(${GDAL_INCLUDE_DIRS})

pkg_check_modules(EIGEN REQUIRED eigen3)
include_directories(${EIGEN_INCLUDE_DIRS})

find_package(OpenMP)
if(OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()


file(GLOB_RECURSE SRC_FILES core/* data/* entity/* processing/* query/* storage/*)
file(GLOB_RECURSE ODB_FILES
    ../include/sempr/storage/DBObject.hpp
    ../include/sempr/storage/History.hpp
    ../include/sempr/storage/Dummy.hpp
    ../include/sempr/data/*
    ../include/sempr/entity/*
)

# message( "found files: ${SRC_FILES}")
# message( "found files: ${ODB_FILES}")

set(DATABASE sqlite)
if (${DATABASE} MATCHES sqlite)
    set(CONFIG_DATABASE DATABASE_SQLITE)
# elseif()
endif()


odb_compile(SRC_FILES
    FILES ${ODB_FILES}
    INCLUDE ../include ${Soprano_INCLUDE_DIRS} ${GDAL_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
    # PROFILE boost/uuid
    PROJECT_INCLUDE ${PROJECT_SOURCE_DIR}/include
    HEADER_PROLOGUE ${PROJECT_SOURCE_DIR}/include/sempr/storage/traits.hxx
    DB ${DATABASE} GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA STANDARD c++11)

include_directories(${ODB_COMPILE_OUTPUT_DIR})
# message("SOURCE FILES: ${SRC_FILES}")
add_library(sempr_core SHARED ${SRC_FILES})
target_link_libraries(sempr_core ${ODB_LIBRARIES} ${Soprano_LIBRARIES} ${GDAL_LIBRARIES})
target_compile_definitions(sempr_core PUBLIC -D${CONFIG_DATABASE})
# target_include_directories(sempr_core
    # PRIVATE ${ODB_COMPILE_OUTPUT_DIR} )


## install stuff
install(
    TARGETS sempr_core
    LIBRARY DESTINATION lib/sempr
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
    DIRECTORY ../include/sempr
    DESTINATION include
)

# odb-generated headers
# well... where to put them? how to include them?
# includes are currently used directly: #include <Point_odb.h>
# TODO: should we change that?
# just move them to a common folder: .../include/sempr/odb_gen
# and add the path to the include dirs in the config file
install(
    DIRECTORY ${ODB_COMPILE_OUTPUT_DIR}
    DESTINATION include/sempr
    FILES_MATCHING PATTERN "*.h"
)

# pkg-config-file
configure_file("sempr-core.pc.in" "sempr-core.pc" @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/sempr-core.pc"
    DESTINATION lib/pkgconfig
)
