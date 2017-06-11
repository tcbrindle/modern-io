
#include "catch.hpp"

#include <io/buffered_stream.hpp>
#include <io/string_stream.hpp>

const std::string test_string = "The quick brown fox jumped over the lazy dog";

TEST_CASE("Basic buffered_read_stream test")
{
    io::buffered_read_stream<io::string_stream> stream{io::string_stream{test_string}, 20};
    static_assert(io::is_sync_read_stream_v<decltype(stream)>, "");
    static_assert(io::is_sync_write_stream_v<decltype(stream)>, "");

    std::string buf(10, '\0');
    std::error_code ec;
    std::size_t bytes_read = 0;

    REQUIRE_NOTHROW(bytes_read = io::read(stream, io::buffer(buf), ec));
    REQUIRE(bytes_read == 10);
    REQUIRE(buf == test_string.substr(0, 10));

    REQUIRE_NOTHROW(bytes_read = io::read(stream, io::buffer(buf), ec));
    REQUIRE(bytes_read == 10);
    REQUIRE(buf == test_string.substr(10, 10));

    REQUIRE_NOTHROW(bytes_read = io::read(stream, io::buffer(buf), ec));
    REQUIRE(bytes_read == 10);
    REQUIRE(buf == test_string.substr(20, 10));
}

TEST_CASE("Basic buffered_write_stream test")
{
    io::buffered_write_stream<io::string_stream> stream{io::string_stream{test_string}, 20};
    static_assert(io::is_sync_read_stream_v<decltype(stream)>, "");
    static_assert(io::is_sync_write_stream_v<decltype(stream)>, "");
}

TEST_CASE("Basic buffered_stream test")
{
    io::buffered_stream<io::string_stream> stream{io::string_stream{test_string}, 20, 20};
    static_assert(io::is_sync_read_stream_v<decltype(stream)>, "");
    static_assert(io::is_sync_write_stream_v<decltype(stream)>, "");

    std::string buf(10, '\0');
    std::error_code ec;
    std::size_t bytes_read = 0;

    REQUIRE_NOTHROW(bytes_read = io::read(stream, io::buffer(buf), ec));
    REQUIRE(bytes_read == 10);
    REQUIRE(buf == test_string.substr(0, 10));

    REQUIRE_NOTHROW(bytes_read = io::read(stream, io::buffer(buf), ec));
    REQUIRE(bytes_read == 10);
    REQUIRE(buf == test_string.substr(10, 10));

    REQUIRE_NOTHROW(bytes_read = io::read(stream, io::buffer(buf), ec));
    REQUIRE(bytes_read == 10);
    REQUIRE(buf == test_string.substr(20, 10));
}