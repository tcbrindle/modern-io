
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "catch.hpp"

#include <io/string_stream.hpp>
#include <io/byte_reader.hpp>

#include <array>
#include <string>
#include <vector>

const std::string test_string = "The quick brown fox jumped over the lazy dog";

static_assert(io::is_sync_read_stream_v<io::string_stream>,
              "string_stream does not meet the SyncReadStream requirements");
static_assert(io::is_sync_write_stream_v<io::string_stream>,
              "string_stream does not meet the SyncWriteStream requirements");
static_assert(io::is_seekable_stream_v<io::string_stream>,
              "string_stream does not meet the SeekableStream requirements");

TEST_CASE("String streams can be default constructed", "[string_stream]")
{
    const io::string_stream d{};
    REQUIRE(d.str() == "");
    REQUIRE(d.get_position().offset_from_start() == 0);
}

TEST_CASE("String streams can be value constructed", "[string_stream]")
{
    const io::string_stream d{test_string};
    REQUIRE(d.str() == test_string);
    REQUIRE(d.get_position().offset_from_start() == 0);
}

TEST_CASE("String streams support seeking", "[string_stream]")
{
    io::string_stream d{test_string};

    SECTION("...from start") {
        REQUIRE_NOTHROW(d.seek(10, io::seek_mode::start));
        REQUIRE(d.get_position().offset_from_start() == 10);
    }

    SECTION("...from end") {
        REQUIRE_NOTHROW(d.seek(-10, io::seek_mode::end));
        REQUIRE(d.get_position().offset_from_start() == test_string.size() - 10);
    }

    SECTION("...from current") {
        REQUIRE_NOTHROW(d.seek(10, io::seek_mode::start));
        REQUIRE_NOTHROW(d.seek(10, io::seek_mode::current));
        REQUIRE(d.get_position().offset_from_start() == 20);
    }

    SECTION("...and throw for negative seeks") {
        REQUIRE_THROWS_AS(d.seek(-10, io::seek_mode::start), std::system_error);
    }
}

TEST_CASE("String streams can be read from", "[string_stream]")
{
    io::string_stream d{test_string};

    SECTION("...using a short static buffer") {
        constexpr int buf_size = 10;
        std::array<char, buf_size> buf;
        std::error_code ec{};
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = d.read_some(io::buffer(buf), ec));
        REQUIRE_FALSE(ec);
        REQUIRE(bytes_read == buf_size);
        REQUIRE(std::equal(std::cbegin(buf),
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
        REQUIRE(std::equal(std::cbegin(buf),
                           std::cend(buf),
                           std::cbegin(test_string),
                           std::cbegin(test_string) + buf_size));
    }

    SECTION("...using a long static buffer") {
        std::array<char, 100> buf;
        std::error_code ec{};
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = d.read_some(io::buffer(buf), ec));
        REQUIRE(bytes_read == test_string.size());
        REQUIRE(std::equal(std::begin(buf),
                           std::begin(buf) + test_string.size(),
                           std::cbegin(test_string),
                           std::cend(test_string)));
    }

    SECTION("...using a long static buffer (throwing)") {
        std::array<char, 100> buf;
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = d.read_some(io::buffer(buf)));
        REQUIRE(bytes_read == test_string.size());
        REQUIRE(std::equal(std::begin(buf),
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
        REQUIRE(std::equal(std::cbegin(buf), std::cend(buf),
                           std::cbegin(test_string), std::cend(test_string)));
    }

    SECTION("...using a dynamic buffer (throwing)") {
        std::vector<char> buf;
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = io::read(d, io::dynamic_buffer(buf),
                                   io::transfer_exactly{test_string.size()}));
        REQUIRE(bytes_read == test_string.size());
        REQUIRE(std::equal(std::cbegin(buf), std::cend(buf),
                        std::cbegin(test_string), std::cend(test_string)));
    }
}

TEST_CASE("String streams can be written to", "[string_stream]")
{
    io::string_stream d{};

    SECTION("Using a static buffer") {
        std::error_code ec{};
        std::size_t bytes_written = 0;
        REQUIRE_NOTHROW(bytes_written = d.write_some(io::buffer(test_string), ec));
        REQUIRE_FALSE(ec);
        REQUIRE(bytes_written == test_string.size());
        REQUIRE(d.str() == test_string);
    }

    SECTION("Using a static buffer (throwing)") {
        std::size_t bytes_written = 0;
        REQUIRE_NOTHROW(bytes_written = d.write_some(io::buffer(test_string)));
        REQUIRE(bytes_written == test_string.size());
        REQUIRE(d.str() == test_string);
    }

    SECTION("Using a dynamic buffer") {
        std::string test_string_copy(test_string);
        std::error_code ec{};
        std::size_t bytes_written = 0;
        REQUIRE_NOTHROW(bytes_written = io::write(d, io::dynamic_buffer(test_string_copy), ec));
        REQUIRE_FALSE(ec);
        REQUIRE(bytes_written == test_string.size());
        REQUIRE(test_string_copy == test_string);
        REQUIRE(d.str() == test_string);
    }
}