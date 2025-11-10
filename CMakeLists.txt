# CMakeLists.txt
cmake_minimum_required(VERSION 3.23)
project(MyProject LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-fcolor-diagnostics)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

include(FetchContent)
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.deps")
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.2
  GIT_SHALLOW    TRUE    
)
FetchContent_MakeAvailable(glm)
FetchContent_GetProperties(glm)
if (glm_SOURCE_DIR)
  set_property(DIRECTORY ${glm_SOURCE_DIR} PROPERTY CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE)
  set_property(DIRECTORY ${glm_SOURCE_DIR} PROPERTY CMAKE_WARN_DEPRECATED OFF)
endif()
set(FETCHCONTENT_QUIET TRUE)

add_subdirectory(dsalgo)

add_executable(MyProgram main.cpp)
target_link_libraries(MyProgram PRIVATE DSAlgo glm::glm)

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
target_compile_options(MyProgram PRIVATE ${STRICT_WARNINGS})

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()