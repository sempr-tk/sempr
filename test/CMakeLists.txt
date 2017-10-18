find_package(Boost REQUIRED)

add_executable(semprtest main.cpp)
target_link_libraries(semprtest sempr_core)

add_executable(unittest test.cpp)
target_link_libraries(unittest sempr_core)
