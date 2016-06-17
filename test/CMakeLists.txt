
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(test-modern-io
    basic_test.cpp
    buffer_copy_test.cpp
    catch_main.cpp
    read_until_test.cpp
    string_stream_test.cpp
    string_view_stream_test.cpp
    )

add_dependencies(test-modern-io range-v3)
target_include_directories(test-modern-io PRIVATE ${RANGE_INCLUDE_DIR})
target_link_libraries(test-modern-io Threads::Threads)
