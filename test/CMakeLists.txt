find_package(Boost REQUIRED system filesystem)

add_executable(manual_test main.cpp)
target_link_libraries(manual_test sempr_core)

add_executable(sempr_test_bin SEMPR_tests.cpp)
target_link_libraries(sempr_test_bin sempr_core )

if(Boost_FOUND)
  message("found boost")
  include_directories(${Boost_INCLUDE_DIRS})
  target_link_libraries(sempr_test_bin ${Boost_LIBRARIES})
endif()
