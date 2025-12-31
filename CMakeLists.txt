# CMakeLists.txt
cmake_minimum_required(VERSION 3.23)
project(MyProject LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(APPLE)
  execute_process(
    COMMAND xcrun --sdk macosx --show-sdk-path
    OUTPUT_VARIABLE MACOS_SDK
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
  if(NOT EXISTS "${MACOS_SDK}")
    message(FATAL_ERROR "xcrun returned an invalid macOS SDK path: '${MACOS_SDK}'")
  endif()
  set(CMAKE_OSX_SYSROOT "${MACOS_SDK}" CACHE PATH "macOS SDK" FORCE)
  set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architecture" FORCE)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-fcolor-diagnostics)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fdiagnostics-color=always)
endif()

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
  set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

add_library(project_warnings INTERFACE)

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
  -Wformat=2
)

target_compile_options(project_warnings INTERFACE ${STRICT_WARNINGS})

include(FetchContent)
set(FETCHCONTENT_BASE_DIR "${CMAKE_BINARY_DIR}/_deps")
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
set(FETCHCONTENT_QUIET TRUE)

FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG        1.0.2
  GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(glm)

FetchContent_GetProperties(glm)
if(glm_SOURCE_DIR)
  set_property(DIRECTORY "${glm_SOURCE_DIR}" PROPERTY CMAKE_SUPPRESS_DEVELOPER_WARNINGS TRUE)
  set_property(DIRECTORY "${glm_SOURCE_DIR}" PROPERTY CMAKE_WARN_DEPRECATED OFF)
endif()

add_subdirectory(dsalgo)

add_executable(MyProgram main.cpp)
target_link_libraries(MyProgram PRIVATE DSAlgo glm::glm project_warnings)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()

add_subdirectory(examples)