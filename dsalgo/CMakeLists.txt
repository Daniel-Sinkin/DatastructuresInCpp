# dsalgo/CMakeLists.txt
add_library(DSAlgo INTERFACE)

file(GLOB DSALGO_HEADERS
  CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)

target_sources(DSAlgo INTERFACE
  FILE_SET HEADERS
  BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
  FILES ${DSALGO_HEADERS}
)

target_include_directories(DSAlgo INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>
)

target_compile_features(DSAlgo INTERFACE cxx_std_23)

target_link_libraries(DSAlgo INTERFACE glm::glm project_warnings)

target_include_directories(DSAlgo SYSTEM INTERFACE
  $<TARGET_PROPERTY:glm::glm,INTERFACE_INCLUDE_DIRECTORIES>
)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_ok)
if(ipo_ok)
  set_property(TARGET DSAlgo PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()