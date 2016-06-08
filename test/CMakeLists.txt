
add_executable(test-modern-io
    basic_test.cpp
    catch_main.cpp
    debug_stream_test.cpp
    )

add_dependencies(test-modern-io range-v3)
target_include_directories(test-modern-io PRIVATE ${RANGE_INCLUDE_DIR})

