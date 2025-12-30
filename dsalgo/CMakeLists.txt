add_library(DSAlgo INTERFACE)

target_sources(DSAlgo INTERFACE
  FILE_SET HEADERS
  BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
  FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/list.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/array.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/linked_list_single.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/linked_list_double.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/hashmap_chained.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/types.hpp
)

target_include_directories(DSAlgo INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
  $<INSTALL_INTERFACE:include>
)
target_include_directories(DSAlgo INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_features(DSAlgo INTERFACE cxx_std_23)

set_target_properties(DSAlgo PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(DSAlgo INTERFACE glm::glm project_warnings)

target_include_directories(DSAlgo SYSTEM INTERFACE
  $<TARGET_PROPERTY:glm::glm,INTERFACE_INCLUDE_DIRECTORIES>
)

include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_ok)
if(ipo_ok)
  set_property(TARGET DSAlgo PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()