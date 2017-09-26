
file(GLOB_RECURSE SRC_FILES core/* data/* entity/* processing/* query/* storage/*)
file(GLOB_RECURSE ODB_FILES 
    ../include/sempr/storage/DBObject.h
    ../include/sempr/storage/History.h
    ../include/sempr/storage/Dummy.h
    ../include/sempr/data/*
    ../include/sempr/entity/*
)

message( "found files: ${SRC_FILES}")
message( "found files: ${ODB_FILES}")

# find odb compiler & stuff
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}/cmake")
find_package(ODB REQUIRED COMPONENTS sqlite boost)
include_directories(${ODB_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)
include(${ODB_USE_FILE})

# find_package(Boost REQUIRED COMPONENTS uuid)

odb_compile(SRC_FILES 
    FILES ${ODB_FILES} 
    INCLUDE ../include
    PROFILE boost/uuid
    PROJECT_INCLUDE ${PROJECT_SOURCE_DIR}/include
    HEADER_PROLOGUE ${PROJECT_SOURCE_DIR}/include/sempr/storage/traits.hxx
    DB sqlite GENERATE_QUERY GENERATE_SESSION GENERATE_SCHEMA STANDARD c++11)

include_directories(${ODB_COMPILE_OUTPUT_DIR})
message("SOURCE FILES: ${SRC_FILES}")
add_library(sempr_core SHARED ${SRC_FILES})
target_link_libraries(sempr_core ${ODB_LIBRARIES})
target_compile_definitions(sempr_core PUBLIC -DDATABASE_SQLITE)
# target_include_directories(sempr_core 
    # PRIVATE ${ODB_COMPILE_OUTPUT_DIR} )

add_subdirectory(test)
