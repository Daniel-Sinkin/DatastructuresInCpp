# CMakeLists.txt
cmake_minimum_required(VERSION 3.23)

project(MyProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(dsalgo)

add_executable(MyProgram main.cpp)
target_link_libraries(MyProgram PRIVATE DSAlgo)

include(CTest)
if(BUILD_TESTING)
  add_subdirectory(tests)
endif()