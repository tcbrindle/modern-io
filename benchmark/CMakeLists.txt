
add_executable(read-benchmark read_benchmark.cpp)

add_dependencies(read-benchmark range-v3)
target_include_directories(read-benchmark PRIVATE ${RANGE_INCLUDE_DIR})
target_link_libraries(read-benchmark ${MODERN_IO_FILESYSTEM_LIBRARY})
