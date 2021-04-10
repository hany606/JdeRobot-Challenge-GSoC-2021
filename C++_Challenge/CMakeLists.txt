cmake_minimum_required (VERSION 3.5.1)

project(JDE_cpp_Challenge)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

include_directories(includes)
include_directories(src)

add_executable(main
  src/main.cpp
  includes/Labyrinth.cpp
)