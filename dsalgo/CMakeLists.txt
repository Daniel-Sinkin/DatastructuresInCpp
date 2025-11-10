# DSAlgo/CMakeLists
add_library(DSAlgo INTERFACE)

target_sources(DSAlgo INTERFACE
  FILE_SET HEADERS
  BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
  FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/list.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/linked_list_single.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/linked_list_double.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hashmap.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/primitives.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/types.hpp
)

target_include_directories(DSAlgo INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(DSAlgo INTERFACE cxx_std_23)
set_target_properties(DSAlgo PROPERTIES POSITION_INDEPENDENT_CODE ON)

set(STRICT_WARNINGS
  -Wall
  -Wextra
  -Wpedantic
  -Werror
  -Wshadow
  -Wconversion
  -Wsign-conversion
  -Wnull-dereference
  -Wdouble-promotion
  -Wimplicit-fallthrough
  -Wold-style-cast
  -Woverloaded-virtual
  -Wcast-align
  -Wmissing-declarations
  -Wformat=2
)
target_compile_options(DSAlgo INTERFACE ${STRICT_WARNINGS})

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_ok)
if(ipo_ok)
  set_property(TARGET DSAlgo PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()