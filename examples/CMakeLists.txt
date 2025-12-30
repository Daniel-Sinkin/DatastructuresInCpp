# examples/CMakeLists.txt
add_executable(example_binary_tree binary_tree.cpp)
target_link_libraries(example_binary_tree PRIVATE DSAlgo)

add_executable(example_tree tree.cpp)
target_link_libraries(example_tree PRIVATE DSAlgo)