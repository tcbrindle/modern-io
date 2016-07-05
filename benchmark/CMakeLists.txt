
add_executable(read-benchmark read_benchmark.cpp)

target_include_directories(read-benchmark PRIVATE ${RANGE_INCLUDE_DIR})
target_link_libraries(read-benchmark ${MODERN_IO_FILESYSTEM_LIBRARY})
