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

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})

  set(DEPENDENCIES sempr_core ${Boost_LIBRARIES} ${PROJ4_LIBRARIES} ${GEOS_LIBRARY})

  add_executable(sempr_storage_test SEMPR_tests.cpp storage_tests.cpp)
  target_link_libraries(sempr_storage_test ${DEPENDENCIES})
  add_test(NAME sempr_storage_test COMMAND sempr_storage_test)

  add_executable(sempr_RDFPropertymap_test SEMPR_tests.cpp RDFPropertymap_tests.cpp)
  target_link_libraries(sempr_RDFPropertymap_test ${DEPENDENCIES})
  add_test(NAME sempr_RDFPropertymap_test COMMAND sempr_RDFPropertymap_test)

  add_executable(sempr_queries_test SEMPR_tests.cpp queries_tests.cpp)
  target_link_libraries(sempr_queries_test ${DEPENDENCIES})
  add_test(NAME sempr_queries_test COMMAND sempr_queries_test)

  add_executable(sempr_geometries_tests SEMPR_tests.cpp geometries_tests.cpp)
  target_link_libraries(sempr_geometries_tests ${DEPENDENCIES})
  add_test(NAME sempr_geometries_tests COMMAND sempr_geometries_tests)

  add_executable(sempr_spatial_index_tests SEMPR_tests.cpp spatial_index_tests.cpp)
  target_link_libraries(sempr_spatial_index_tests ${DEPENDENCIES})
  add_test(NAME sempr_spatial_index_tests COMMAND sempr_spatial_index_tests)

  #todo: add a macro to reduce the line of command here!

endif()


