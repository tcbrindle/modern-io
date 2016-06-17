
cmake_minimum_required(VERSION 3.1)

project(modern.io CXX)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror")

# g++ (currently?) requires us to link to the filesystem library separately
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lstdc++fs")
endif()

include_directories(include)

add_subdirectory(external)
add_subdirectory(test)



