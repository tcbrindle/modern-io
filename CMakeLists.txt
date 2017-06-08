
cmake_minimum_required(VERSION 3.1)

project(modern.io CXX)

set(CMAKE_CXX_STANDARD 17)

if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror")
    # g++ (currently?) requires us to link to the filesystem library separately
    if(CMAKE_COMPILER_IS_GNUCXX)
        set(MODERN_IO_FILESYSTEM_LIBRARY stdc++fs)
    endif() # GNUCXX
endif() # UNIX

if (WIN32)
    if (CMAKE_COMPILER_IS_GNUCXX)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Werror")
        find_package(Boost COMPONENTS filesystem REQUIRED)
        include_directories(${Boost_INCLUDE_DIR})
        set(MODERN_IO_FILESYSTEM_LIBRARY ${Boost_LIBRARIES})
    endif()
    add_definitions("-DNOMINMAX")
endif()

include_directories(include)
set(RANGE_INCLUDE_DIR "${modern.io_SOURCE_DIR}/external/range-v3/include")

add_subdirectory(benchmark)
add_subdirectory(test)




