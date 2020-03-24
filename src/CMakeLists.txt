pkg_check_modules(Soprano REQUIRED soprano)
include_directories(${Soprano_INCLUDE_DIRS})
link_directories(${Soprano_LIBRARY_DIRS})

find_package(Threads REQUIRED)

# list of all source files to compile into the library
set(SRC_FILES
    Core.cpp
    RDF.cpp
    SimpleIncrementalIDGenerator.cpp
    Entity.cpp
    ECAccessors.cpp
    Component.cpp
    SeparateFileStorage.cpp
    ./component/AffineTransform.cpp
    ./component/GeosGeometry.cpp
    ./component/TripleContainer.cpp
    ./component/TripleDocument.cpp
    ./component/TriplePropertyMap.cpp
    ./component/TripleVector.cpp
    ./nodes/AffineTransformNodes.cpp
    ./nodes/AffineTransformBuilders.cpp
    ./nodes/ECNode.cpp
    ./nodes/ExtractTriples.cpp
    ./nodes/ExtractTriplesBuilder.cpp
    ./nodes/FileMonitorNode.cpp
    ./nodes/FileMonitorNodeBuilder.cpp
    ./nodes/InferECNode.cpp
    ./nodes/LoadTriplesFromFileNode.cpp
    ./nodes/LoadTriplesFromFileBuilder.cpp
    ./nodes/TripleDocumentFilenameNode.cpp
    ./nodes/TripleDocumentFilenameBuilder.cpp
    ./nodes/SopranoModule.cpp
    ./nodes/SopranoNodeBuilder.cpp
    ./util/FileWatcher.cpp
)

# create the library
add_library(sempr_core SHARED ${SRC_FILES})
set_target_properties(sempr_core PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
target_link_libraries(sempr_core ${Soprano_LIBRARIES} ${GEOS_LIBRARY} ${GeographicLib_LIBRARIES} ${RETE_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)

## install stuff
install(
    TARGETS sempr_core
    LIBRARY DESTINATION lib/sempr COMPONENT common
)
# install header files
install(
    DIRECTORY ../include/
    DESTINATION include/sempr COMPONENT common
)


# pkg-config-file - one to find sempr when installed through deb packaging...
set(SEMPR_INSTALL_PREFIX "/usr")
configure_file("sempr-core.pc.in" "sempr-core.pc" @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/sempr-core.pc"
    DESTINATION lib/pkgconfig
    COMPONENT pkgconfig-deb
)

# ... and one to find sempr in the CMAKE_INSTALL_PREFIX path
set(SEMPR_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
configure_file("sempr-core.pc.in" "sempr-core-dev.pc" @ONLY)
install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}/sempr-core-dev.pc"
    DESTINATION lib/pkgconfig
    COMPONENT pkgconfig-cmake
)


