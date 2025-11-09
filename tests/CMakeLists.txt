# tests/CMakeLists.txt

add_executable(DSTests
  main.cpp
  test_list.cpp
)
target_link_libraries(DSTests PRIVATE DSAlgo)

add_test(NAME DSTests COMMAND DSTests)