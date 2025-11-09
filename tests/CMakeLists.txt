# tests/CMakeLists.txt

add_executable(test_list test_list.cpp)
target_link_libraries(test_list PRIVATE DSAlgo)
add_test(NAME test_list COMMAND test_list)