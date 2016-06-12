
add_executable(test-modern-io
    basic_test.cpp
    catch_main.cpp
    debug_stream_test.cpp
    read_until_test.cpp
    )

add_dependencies(test-modern-io range-v3)
target_include_directories(test-modern-io PRIVATE ${RANGE_INCLUDE_DIR})

add_executable(read-bench read_bench.cpp)

add_dependencies(read-bench range-v3)
target_include_directories(read-bench PRIVATE ${RANGE_INCLUDE_DIR})
