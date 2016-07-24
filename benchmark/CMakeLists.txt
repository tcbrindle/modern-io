
add_executable(read-benchmark black_box.cpp read_benchmark.cpp)

target_include_directories(read-benchmark PRIVATE ${RANGE_INCLUDE_DIR})
target_link_libraries(read-benchmark ${MODERN_IO_FILESYSTEM_LIBRARY})

add_executable(adam-benchmark adam_benchmark.cpp)

target_include_directories(adam-benchmark PRIVATE ${RANGE_INCLUDE_DIR})
target_link_libraries(adam-benchmark ${MODERN_IO_FILESYSTEM_LIBRARY})