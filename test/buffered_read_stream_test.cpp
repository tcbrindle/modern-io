
#include "catch.hpp"

#include <io/buffered_read_stream.hpp>
#include <io/debug/debug_stream.hpp>

const std::string test_string = "The quick brown fox jumped over the lazy dog";

TEST_CASE("Basic buffered_read_stream test")
{
    io::buffered_read_stream<io::debug_stream> stream{20, test_string};

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