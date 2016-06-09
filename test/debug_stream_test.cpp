
#include "catch.hpp"

#include <io/debug/debug_stream.hpp>
#include <io/stream_reader.hpp>

#include <range/v3/algorithm.hpp>

#include <array>
#include <string>
#include <vector>

namespace rng = ranges::v3;

const std::string test_string = "The quick brown fox jumped over the lazy dog";

static_assert(io::SyncReadStream<io::debug_stream>(),
              "debug_stream does not meet the SyncReadStream requirements");
static_assert(io::SyncWriteStream<io::debug_stream>(),
              "debug_stream does not meet the SyncWriteStream requirements");
static_assert(io::SeekableStream<io::debug_stream>(),
              "debug_stream does not meet the SeekableStream requirements");

TEST_CASE("Debug streams can be default constructed", "[debug_stream]")
{
    const io::debug_stream d{};
    REQUIRE(d.str() == "");
    REQUIRE(d.get_position() == 0);
}

TEST_CASE("Debug streams can be value constructed", "[debug_stream]")
{
    const io::debug_stream d{test_string};
    REQUIRE(d.str() == test_string);
    REQUIRE(d.get_position() == 0);
}

TEST_CASE("Debug streams support seeking", "[debug_stream]")
{
    io::debug_stream d{test_string};

    SECTION("...from start") {
        REQUIRE_NOTHROW(d.seek(10, io::seek_mode::start));
        REQUIRE(d.get_position() == 10);
    }

    SECTION("...from end") {
        REQUIRE_NOTHROW(d.seek(-10, io::seek_mode::end));
        REQUIRE(d.get_position() == test_string.size() - 10);
    }

    SECTION("...from current") {
        REQUIRE_NOTHROW(d.seek(10, io::seek_mode::start));
        REQUIRE_NOTHROW(d.seek(10, io::seek_mode::current));
        REQUIRE(d.get_position() == 20);
    }

    SECTION("...and throw for negative seeks") {
        REQUIRE_THROWS_AS(d.seek(-10, io::seek_mode::start), std::system_error);
    }
}

TEST_CASE("Debug streams can be read from", "[debug_stream]")
{
    io::debug_stream d{test_string};

    SECTION("...using a short static buffer") {
        constexpr int buf_size = 10;
        std::array<char, buf_size> buf;
        std::error_code ec{};
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = d.read_some(io::buffer(buf), ec));
        REQUIRE_FALSE(ec);
        REQUIRE(bytes_read == buf_size);
        REQUIRE(rng::equal(std::cbegin(buf),
                           std::cend(buf),
                           std::cbegin(test_string),
                           std::cbegin(test_string) + buf_size));
    }

    SECTION("...using a short static buffer (throwing)") {
        constexpr int buf_size = 10;
        std::array<char, buf_size> buf;
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = d.read_some(io::buffer(buf)));
        REQUIRE(bytes_read == buf_size);
        REQUIRE(rng::equal(std::cbegin(buf),
                           std::cend(buf),
                           std::cbegin(test_string),
                           std::cbegin(test_string) + buf_size));
    }

    SECTION("...using a long static buffer") {
        std::array<char, 100> buf;
        std::error_code ec{};
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = d.read_some(io::buffer(buf), ec));
        REQUIRE(ec.value() == static_cast<int>(io::stream_errc::eof));
        REQUIRE(ec.category() == io::stream_category());
        REQUIRE(bytes_read == test_string.size());
        REQUIRE(rng::equal(std::begin(buf),
                           std::begin(buf) + test_string.size(),
                           std::cbegin(test_string),
                           std::cend(test_string)));
    }

    SECTION("...using a long static buffer (throwing)") {
        std::array<char, 100> buf;
        std::size_t bytes_read = 0;
        REQUIRE_THROWS_AS(bytes_read = d.read_some(io::buffer(buf)), std::system_error);
        REQUIRE(bytes_read == 0); // Because above call doesn't return
        REQUIRE(rng::equal(std::begin(buf),
                           std::begin(buf) + test_string.size(),
                           std::cbegin(test_string),
                           std::cend(test_string)));
    }

    SECTION("...using a dynamic buffer") {
        std::vector<char> buf;
        std::error_code ec;
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = io::read(d, io::dynamic_buffer(buf),
                                 io::transfer_exactly{test_string.size()},
                                 ec));
        REQUIRE(bytes_read == test_string.size());
        REQUIRE(rng::equal(buf, test_string));
    }

    SECTION("...using a dynamic buffer (throwing)") {
        std::vector<char> buf;
        std::size_t bytes_read = 0;
        REQUIRE_THROWS_AS(bytes_read = io::read(d, io::dynamic_buffer(buf),
                                   io::transfer_exactly{test_string.size()}),
                          std::system_error);
        REQUIRE(rng::equal(buf, test_string));
    }
}