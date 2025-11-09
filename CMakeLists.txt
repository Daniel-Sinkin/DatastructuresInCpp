cmake_minimum_required(VERSION 3.23)

project(MyProject)

add_executable(MyProgram)
target_sources(MyProgram
  PRIVATE
    main.cpp
)