find_package(Boost REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(Soprano REQUIRED soprano)
include_directories(${Soprano_INCLUDE_DIRS})

add_executable(manual_test main.cpp)
target_link_libraries(manual_test sempr_core)

add_executable(sempr_test_bin SEMPR_tests.cpp)
target_link_libraries(sempr_test_bin sempr_core)
add_test(NAME sempr_test COMMAND sempr_test_bin)
