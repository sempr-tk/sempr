find_package(Boost REQUIRED system filesystem)
find_package(PkgConfig REQUIRED)

pkg_check_modules(Soprano REQUIRED soprano)
include_directories(${Soprano_INCLUDE_DIRS})
link_directories(${Soprano_LIBRARY_DIRS})

#find_package(GDAL)
#if(GDAL_FOUND)
#  include_directories(${GDAL_INCLUDE_DIR})
#  link_directories(${GDAL_LIBRARY})
#endif(GDAL_FOUND)


#add_executable(manual_test main.cpp)
#target_link_libraries(manual_test sempr_core ${GEOS_LIBRARY} ${GeographicLib_LIBRARIES})

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    set(DEPENDENCIES sempr_core sempr_plugin_geos ${Boost_LIBRARIES} ${GEOS_LIBRARY} ${GeographicLib_LIBRARIES})


    add_executable(CoreTest CoreTest.cpp)
    target_link_libraries(CoreTest ${DEPENDENCIES})
    add_test(NAME CoreTest COMMAND CoreTest)

    add_executable(ConstructRulesEffectTest ConstructRulesEffectTest.cpp)
    target_link_libraries(ConstructRulesEffectTest ${DEPENDENCIES})
    add_test(NAME ConstructRulesEffectTest COMMAND ConstructRulesEffectTest)

    add_executable(EntityTest EntityTest.cpp)
    target_link_libraries(EntityTest ${DEPENDENCIES})
    add_test(NAME EntityTest COMMAND EntityTest)

    add_executable(FileStorageTest FileStorageTest.cpp)
    target_link_libraries(FileStorageTest ${DEPENDENCIES})
    add_test(NAME FileStorageTest COMMAND FileStorageTest)

    add_executable(ComponentSerializationTest ComponentSerializationTest.cpp)
    target_link_libraries(ComponentSerializationTest ${DEPENDENCIES})
    add_test(NAME ComponentSerializationTest COMMAND ComponentSerializationTest)

    add_executable(FileMonitorNodeTest FileMonitorNodeTest.cpp)
    target_link_libraries(FileMonitorNodeTest ${DEPENDENCIES})
    add_test(NAME FileMonitorNodeTest COMMAND FileMonitorNodeTest)

    add_executable(TripleContainerTest TripleContainerTest.cpp)
    target_link_libraries(TripleContainerTest ${DEPENDENCIES})
    add_test(NAME TripleContainerTest COMMAND TripleContainerTest)

    add_executable(TripleDocumentTest TripleDocumentTest.cpp)
    target_link_libraries(TripleDocumentTest ${DEPENDENCIES})
    add_test(NAME TripleDocumentTest COMMAND TripleDocumentTest)

    add_executable(TriplePropertyMapTest TriplePropertyMapTest.cpp)
    target_link_libraries(TriplePropertyMapTest ${DEPENDENCIES})
    add_test(NAME TriplePropertyMapTest COMMAND TriplePropertyMapTest)

    add_executable(ECNodeTest ECNodeTest.cpp)
    target_link_libraries(ECNodeTest ${DEPENDENCIES})
    add_test(NAME ECNodeTest COMMAND ECNodeTest)

    add_executable(InferECTest InferECTest.cpp)
    target_link_libraries(InferECTest ${DEPENDENCIES})
    add_test(NAME InferECTest COMMAND InferECTest)

    add_executable(SopranoTest SopranoTest.cpp)
    target_link_libraries(SopranoTest ${DEPENDENCIES})
    add_test(NAME SopranoTest COMMAND SopranoTest)

    add_executable(GeometryTest GeometryTest.cpp)
    target_link_libraries(GeometryTest ${DEPENDENCIES})
    add_test(NAME GeometryTest COMMAND GeometryTest)

    add_executable(AffineTransformTest AffineTransformTest.cpp)
    target_link_libraries(AffineTransformTest ${DEPENDENCIES})
    add_test(NAME AffineTransformTest COMMAND AffineTransformTest)

    add_executable(CreateEntityTest CreateEntityTest.cpp)
    target_link_libraries(CreateEntityTest ${DEPENDENCIES})
    add_test(NAME CreateEntityTest COMMAND CreateEntityTest)
endif()


