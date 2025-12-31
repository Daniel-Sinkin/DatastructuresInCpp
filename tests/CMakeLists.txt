# tests/CMakeLists.txt
function(add_ds_test name)
  add_executable(${name} ${name}.cpp)
  target_link_libraries(${name} PRIVATE DSAlgo project_warnings)
  add_test(NAME ${name} COMMAND ${name})
endfunction()

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test_*.cpp")

foreach(src ${TEST_SOURCES})
  get_filename_component(name "${src}" NAME_WE) # e.g. test_array
  add_executable(${name} "${src}")
  target_link_libraries(${name} PRIVATE DSAlgo project_warnings)
  add_test(NAME ${name} COMMAND ${name})
endforeach()