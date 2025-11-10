# CMakeLists.txt
cmake_minimum_required(VERSION 3.23)
project(MyProject LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-fcolor-diagnostics)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

add_subdirectory(dsalgo)

add_executable(MyProgram main.cpp)
target_link_libraries(MyProgram PRIVATE DSAlgo)

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