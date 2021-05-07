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
    ./nodes/InferECNode.cpp
    ./nodes/InferECBuilder.cpp
    ./nodes/LoadTriplesFromFileNode.cpp
    ./nodes/LoadTriplesFromFileBuilder.cpp
    ./nodes/TextComponentTextNode.cpp
    ./nodes/TextComponentTextBuilder.cpp
    ./nodes/TripleDocumentFilenameNode.cpp
    ./nodes/TripleDocumentFilenameBuilder.cpp
    ./nodes/SopranoModule.cpp
    ./nodes/SopranoNodeBuilder.cpp
    ./nodes/ComponentQuery.cpp
    ./nodes/ComponentQueryBuilder.cpp
    ./nodes/CreateEntityNode.cpp
    ./nodes/CreateEntityBuilder.cpp
    ./util/FileWatcher.cpp
    ./util/GenericCoordinateFilter.cpp
)

# create the core library
add_library(sempr_core SHARED ${SRC_FILES})
set_target_properties(sempr_core PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
target_link_libraries(sempr_core
    ${Soprano_LIBRARIES} ${RETE_LIBRARIES} ${Boost_LIBRARIES} Threads::Threads)


# create the geos library part
add_library(sempr_geos SHARED
    ./component/GeosGeometry.cpp
    ./nodes/GeoConversionNodes.cpp
    ./nodes/GeoConversionBuilders.cpp
    ./nodes/GeoDistanceNode.cpp
    ./nodes/GeoDistanceBuilder.cpp
    ./nodes/GeoAreaNode.cpp
    ./nodes/GeoAreaBuilder.cpp
    ./nodes/GeoIntersectionNode.cpp
    ./nodes/GeoIntersectionBuilder.cpp
    ./nodes/GeoUnionNode.cpp
    ./nodes/GeoUnionBuilder.cpp
    ./nodes/GeoUnionAllNode.cpp
    ./nodes/GeoUnionAllBuilder.cpp
    ./nodes/GeoIntersectsNode.cpp
    ./nodes/GeoIntersectsBuilder.cpp
    ./nodes/GeoBufferNode.cpp
    ./nodes/GeoBufferBuilder.cpp
    ./nodes/GeoDifferenceNode.cpp
    ./nodes/GeoDifferenceBuilder.cpp
    ./nodes/GeoFromWKTNode.cpp
    ./nodes/GeoFromWKTBuilder.cpp
    ./nodes/GeoToWKTNode.cpp
    ./nodes/GeoToWKTBuilder.cpp
)
set_target_properties(
    sempr_geos
    PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_link_libraries(
    sempr_geos sempr_core ${GEOS_LIBRARY} ${GeographicLib_LIBRARIES}
)

# create the geos plugin
#qt5_wrap_cpp(plugin_geos_src ./plugins/GeosPlugin.cpp ../include/plugins/GeosPlugin.hpp)
message("qt5_wrap... ${plugin_geos_src}")

add_library(sempr_plugin_geos SHARED
    ./plugins/GeosPlugin.cpp
    ../include/plugins/GeosPlugin.hpp
    ${plugin_geos_src}
)
set_target_properties(
    sempr_plugin_geos
    PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_link_libraries(
    sempr_plugin_geos sempr_core sempr_geos ${RETE_LIBRARIES}
    ${QT_CORE}
)

# create the AffineTransform plugin (just to register node parsers, the
# functionality is still located in sempr-core)
add_library(sempr_plugin_affine SHARED
    ./plugins/AffineTFPlugin.cpp
    ../include/plugins/AffineTFPlugin.hpp
)
set_target_properties(sempr_plugin_affine
    PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_link_libraries(sempr_plugin_affine
    sempr_core
    ${RETE_LIBRARIES}
    ${QT_CORE}
)

# create a RDFPlugin that registers things like the ExtractTriple module and
# also inserts a few default rules for extracting triples and updating soprano.
add_library(sempr_plugin_rdf SHARED
    ./plugins/RDFPlugin.cpp
    ../include/plugins/RDFPlugin.hpp
)
set_target_properties(sempr_plugin_rdf
    PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_link_libraries(sempr_plugin_rdf
    sempr_core
    ${RETE_LIBRARIES}
    ${QT_CORE}
)

# create a UtilPlugin that registers additional utility stuff, e.g.
# TextComponents, FileMonitor, ConstructRules, ...
# Also, adds a rule to infers more rules from entities with text components.
add_library(sempr_plugin_util SHARED
    ./plugins/UtilPlugin.cpp
    ../include/plugins/UtilPlugin.hpp
)
set_target_properties(sempr_plugin_util
    PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_link_libraries(sempr_plugin_util
    sempr_core
    ${RETE_LIBRARIES}
    ${QT_CORE}
)

# Date sections
add_library(sempr_date SHARED
    ./nodes/DateDayBuilder.cpp
    ./nodes/DateDayNode.cpp
    ./nodes/DateDiffBuilder.cpp
    ./nodes/DateDiffNode.cpp
    ./nodes/DateMonthBuilder.cpp
    ./nodes/DateMonthNode.cpp
    ./nodes/DateYearBuilder.cpp
    ./nodes/DateYearNode.cpp    
)
set_target_properties(
    sempr_date
    PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_link_libraries(
    sempr_date sempr_core ${RETE_LIBRARIES} ${QT_CORE}
)

#Date plugin
add_library(sempr_plugin_date SHARED
    ./plugins/DatePlugin.cpp
    ../include/plugins/DatePlugin.hpp
)
set_target_properties(sempr_plugin_date
    PROPERTIES VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)
target_link_libraries(
	sempr_plugin_date sempr_core sempr_date ${RETE_LIBRARIES} ${QT_CORE}
)


## install stuff
install(
    TARGETS sempr_core sempr_geos sempr_date
    LIBRARY DESTINATION lib COMPONENT common
)

install(
    TARGETS
        sempr_plugin_date
        sempr_plugin_geos
        sempr_plugin_affine
        sempr_plugin_rdf
        sempr_plugin_util
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


