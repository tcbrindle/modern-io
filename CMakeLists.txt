
cmake_minimum_required(VERSION 3.1)

project(modern.io CXX)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)

add_subdirectory(external)
add_subdirectory(test)
