
#include "catch.hpp"

#include <io/string_stream.hpp>
#include <io/read.hpp>

const std::string test_string = "abcdefghijklmnopqrstuvwxyz";

TEST_CASE("io::read_until finds a character in a string", "[read][read_until]")
{
    std::vector<char> buf;
    io::string_stream d{test_string};
    std::error_code ec;
    std::size_t bytes_read;

    REQUIRE_NOTHROW(bytes_read = io::read_until(d, io::dynamic_buffer(buf), 'e', ec));
    REQUIRE(bytes_read == 5);
    REQUIRE_FALSE(ec);
}

TEST_CASE("io::read_until finds a character in a string (throwing)", "[read][read_until]")
{
    std::vector<char> buf;
    io::string_stream d{test_string};
    std::size_t bytes_read = 0;

    REQUIRE_NOTHROW(bytes_read = io::read_until(d, io::dynamic_buffer(buf), 'e'));
    REQUIRE(bytes_read == 5);
}

TEST_CASE("io::read_until correctly reports a missing character", "[read][read_until]")
{
    std::vector<char> buf;
    io::string_stream d{test_string};
    std::error_code ec;
    std::size_t bytes_read = 0;

    REQUIRE_NOTHROW(bytes_read = io::read_until(d, io::dynamic_buffer(buf), '1', ec));
    REQUIRE(bytes_read == test_string.size());
    REQUIRE(ec.category() == io::stream_category());
    REQUIRE(ec.value() == static_cast<int>(io::stream_errc::not_found));
}

TEST_CASE("io::read_until finds a substring in a string", "[read][read_until]")
{
    std::vector<char> buf;
    io::string_stream d{test_string};
    std::error_code ec;
    std::size_t bytes_read;

    REQUIRE_NOTHROW(bytes_read = io::read_until(d, io::dynamic_buffer(buf), "ef", ec));
    REQUIRE(bytes_read == 5);
    REQUIRE_FALSE(ec);
}

TEST_CASE("io::read_until finds a substring in a string (throwing)", "[read][read_until]")
{
    std::vector<char> buf;
    io::string_stream d{test_string};
    std::size_t bytes_read = 0;

    REQUIRE_NOTHROW(bytes_read = io::read_until(d, io::dynamic_buffer(buf), "ef"));
    REQUIRE(bytes_read == 5);
}

TEST_CASE("io::read_until correctly reports a missing substring", "[read][read_until]")
{
    std::vector<char> buf;
    io::string_stream d{test_string};
    std::error_code ec;
    std::size_t bytes_read = 0;

    REQUIRE_NOTHROW(bytes_read = io::read_until(d, io::dynamic_buffer(buf), "12", ec));
    REQUIRE(bytes_read == test_string.size());
    REQUIRE(ec.category() == io::stream_category());
    REQUIRE(ec.value() == static_cast<int>(io::stream_errc::not_found));
}