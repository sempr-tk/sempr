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
    # ./component/GeosGeometry.cpp
    ./component/TextComponent.cpp
    ./component/TripleContainer.cpp
    ./component/TripleDocument.cpp
    ./component/TriplePropertyMap.cpp
    ./component/TripleVector.cpp
    ./nodes/AffineTransformNodes.cpp
    ./nodes/AffineTransformBuilders.cpp
    ./nodes/ConstructRulesNode.cpp
    ./nodes/ConstructRulesBuilder.cpp
    ./nodes/ECNode.cpp
    ./nodes/ExtractTriples.cpp
    ./nodes/ExtractTriplesBuilder.cpp
    ./nodes/FileMonitorNode.cpp
    ./nodes/FileMonitorNodeBuilder.cpp
    # ./nodes/GeoConversionNodes.cpp
    # ./nodes/GeoConversionBuilders.cpp
    # ./nodes/GeoDistanceNode.cpp
    # ./nodes/GeoDistanceBuilder.cpp
    ./nodes/InferECNode.cpp
    ./nodes/LoadTriplesFromFileNode.cpp
    ./nodes/LoadTriplesFromFileBuilder.cpp
    ./nodes/TextComponentTextNode.cpp
    ./nodes/TextComponentTextBuilder.cpp
    ./nodes/TripleDocumentFilenameNode.cpp
    ./nodes/TripleDocumentFilenameBuilder.cpp
    ./nodes/SopranoModule.cpp
    ./nodes/SopranoNodeBuilder.cpp
    ./util/FileWatcher.cpp
    ./util/GenericCoordinateFilter.cpp
)

# create the core library
add_library(sempr_core SHARED ${SRC_FILES})
set_target_properties(sempr_core PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
#target_link_libraries(sempr_core ${Soprano_LIBRARIES} ${GEOS_LIBRARY} ${GeographicLib_LIBRARIES} ${RETE_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)
target_link_libraries(sempr_core
    ${Soprano_LIBRARIES} ${RETE_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)


# create the geos library part
add_library(sempr_geos SHARED
    ./component/GeosGeometry.cpp
    ./nodes/GeoConversionNodes.cpp
    ./nodes/GeoConversionBuilders.cpp
    ./nodes/GeoDistanceNode.cpp
    ./nodes/GeoDistanceBuilder.cpp
)
set_target_properties(
    sempr_geos
    PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_link_libraries(
    sempr_geos sempr_core ${GEOS_LIBRARY} ${GeographicLib_LIBRARIES}
)

# create the geos plugin
add_library(sempr_plugin_geos SHARED
    ./plugins/GeosPlugin.cpp
)
set_target_properties(
    sempr_plugin_geos
    PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_link_libraries(
    sempr_plugin_geos sempr_core sempr_geos ${RETE_LIBRARIES}
    Qt5::Core
)


## install stuff
install(
    TARGETS sempr_core sempr_geos
    LIBRARY DESTINATION lib COMPONENT common
)

install(
    TARGETS sempr_plugin_geos
    LIBRARY DESTINATION lib/sempr_plugins COMPONENT common
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


