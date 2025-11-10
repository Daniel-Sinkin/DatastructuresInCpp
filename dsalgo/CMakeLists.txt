# DSAlgo/CMakeLists
add_library(DSAlgo STATIC
    src/list.cpp
    src/linked_list_single.cpp
    src/linked_list_double.cpp
)

target_sources(DSAlgo PUBLIC
    FILE_SET  HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/src/list.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/linked_list_single.hpp
      ${CMAKE_CURRENT_SOURCE_DIR}/src/linked_list_double.hpp
)

target_include_directories(DSAlgo PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>
)

target_compile_options(DSAlgo PRIVATE -Wall -Wextra -Wpedantic)