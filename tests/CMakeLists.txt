# tests/CMakeLists.txt
add_executable(test_list test_list.cpp)
target_link_libraries(test_list PRIVATE DSAlgo)
add_test(NAME test_list COMMAND test_list)

add_executable(test_linked_list_single test_linked_list_single.cpp)
target_link_libraries(test_linked_list_single PRIVATE DSAlgo)
add_test(NAME test_linked_list_single COMMAND test_linked_list_single)