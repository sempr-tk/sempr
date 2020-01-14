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
    set(DEPENDENCIES sempr_core ${Boost_LIBRARIES} ${GEOS_LIBRARY} ${GeographicLib_LIBRARIES})


    add_executable(CoreTest CoreTest.cpp)
    target_link_libraries(CoreTest ${DEPENDENCIES})
    add_test(NAME CoreTest COMMAND CoreTest)

    add_executable(EntityTest EntityTest.cpp)
    target_link_libraries(EntityTest ${DEPENDENCIES})
    add_test(NAME EntityTest COMMAND EntityTest)

    add_executable(TripleContainerTest TripleContainerTest.cpp)
    target_link_libraries(TripleContainerTest ${DEPENDENCIES})
    add_test(NAME TripleContainerTest COMMAND TripleContainerTest)

    add_executable(ECNodeTest ECNodeTest.cpp)
    target_link_libraries(ECNodeTest ${DEPENDENCIES})
    add_test(NAME ECNodeTest COMMAND ECNodeTest)
endif()


