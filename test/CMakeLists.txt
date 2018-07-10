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


add_executable(manual_test main.cpp)
target_link_libraries(manual_test sempr_core ${PROJ4_LIBRARIES} ${GEOS_LIBRARY})

add_executable(sempr_test_bin SEMPR_tests.cpp
                              storage_tests.cpp
                              RDFPropertymap_tests.cpp
                              queries_tests.cpp
                              geometries_tests.cpp
                              spatial_index_tests.cpp)

MESSAGE("DBG GEOS_LIB ${GEOS_LIBRARY}")
target_link_libraries(sempr_test_bin sempr_core ${PROJ4_LIBRARIES} ${GEOS_LIBRARY})

if(Boost_FOUND)
  message("found boost")
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(sempr_test_bin ${Boost_LIBRARIES} ${PROJ4_LIBRARIES} ${GEOS_LIBRARY})
endif()
