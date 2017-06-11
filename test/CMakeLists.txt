
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(test-modern-io
    basic_test.cpp
    buffer_copy_test.cpp
    buffered_stream_test.cpp
    byte_range_test.cpp
    catch_main.cpp
    copy_test.cpp
    file_test.cpp
    read_only_test.cpp
    read_until_test.cpp
    string_stream_test.cpp
    string_view_stream_test.cpp
    )

target_include_directories(test-modern-io PRIVATE ${RANGE_INCLUDE_DIR})
target_link_libraries(test-modern-io Threads::Threads ${MODERN_IO_FILESYSTEM_LIBRARY})
