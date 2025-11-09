cmake_minimum_required(VERSION 3.23)

project(MyProject)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(MyProgram)
target_sources(MyProgram
    PRIVATE
        main.cpp
)
