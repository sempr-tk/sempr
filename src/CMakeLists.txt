pkg_check_modules(Soprano REQUIRED soprano)
include_directories(${Soprano_INCLUDE_DIRS})
# message("SOPRANO: ${Soprano_INCLUDE_DIRS}")

pkg_check_modules(GDAL REQUIRED gdal)
include_directories(${GDAL_INCLUDE_DIRS})
message("GDAL: ${GDAL_INCLUDE_DIRS}")


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


odb_compile(SRC_FILES
    FILES ${ODB_FILES}
    INCLUDE ../include ${Soprano_INCLUDE_DIRS} ${GDAL_INCLUDE_DIRS}
    # PROFILE boost/uuid
    PROJECT_INCLUDE ${PROJECT_SOURCE_DIR}/include
    HEADER_PROLOGUE ${PROJECT_SOURCE_DIR}/include/sempr/storage/traits.hxx
    DB sqlite GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA STANDARD c++11)

include_directories(${ODB_COMPILE_OUTPUT_DIR})
# message("SOURCE FILES: ${SRC_FILES}")
add_library(sempr_core SHARED ${SRC_FILES})
target_link_libraries(sempr_core ${ODB_LIBRARIES} ${Soprano_LIBRARIES} ${GDAL_LIBRARIES})
target_compile_definitions(sempr_core PUBLIC -DDATABASE_SQLITE)
# target_include_directories(sempr_core
    # PRIVATE ${ODB_COMPILE_OUTPUT_DIR} )
