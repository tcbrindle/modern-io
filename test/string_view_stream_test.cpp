
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "catch.hpp"

#include <io/string_view_stream.hpp>
#include <io/stream_reader.hpp>

#include <range/v3/algorithm/equal.hpp>

#include <array>
#include <io/io_std/string_view.hpp>
#include <vector>

namespace rng = ranges::v3;

const io_std::string_view test_string_view = "The quick brown fox jumped over the lazy dog";

static_assert(io::SyncReadStream<io::string_view_stream>(),
              "string_view_stream does not meet the SyncReadStream requirements");
static_assert(io::SeekableStream<io::string_view_stream>(),
              "string_view_stream does not meet the SeekableStream requirements");

TEST_CASE("string_view streams can be default constructed", "[string_view_stream]")
{
    const io::string_view_stream d{};
    REQUIRE(d.str() == "");
    REQUIRE(d.get_position() == 0);
}

TEST_CASE("string_view streams can be value constructed", "[string_view_stream]")
{
    const io::string_view_stream d{test_string_view};
    REQUIRE(d.str() == test_string_view);
    REQUIRE(d.get_position() == 0);
}

TEST_CASE("string_view streams support seeking", "[string_view_stream]")
{
    io::string_view_stream d{test_string_view};

    SECTION("...from start") {
        REQUIRE_NOTHROW(d.seek(10, io::seek_mode::start));
        REQUIRE(d.get_position() == 10);
    }

    SECTION("...from end") {
        REQUIRE_NOTHROW(d.seek(-10, io::seek_mode::end));
        REQUIRE(d.get_position() == test_string_view.size() - 10);
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

TEST_CASE("string_view streams can be read from", "[string_view_stream]")
{
    io::string_view_stream d{test_string_view};

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
                           std::cbegin(test_string_view),
                           std::cbegin(test_string_view) + buf_size));
    }

    SECTION("...using a short static buffer (throwing)") {
        constexpr int buf_size = 10;
        std::array<char, buf_size> buf;
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = d.read_some(io::buffer(buf)));
        REQUIRE(bytes_read == buf_size);
        REQUIRE(rng::equal(std::cbegin(buf),
                           std::cend(buf),
                           std::cbegin(test_string_view),
                           std::cbegin(test_string_view) + buf_size));
    }

    SECTION("...using a long static buffer") {
        std::array<char, 100> buf;
        std::error_code ec{};
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = d.read_some(io::buffer(buf), ec));
        REQUIRE_FALSE(ec);
        REQUIRE(bytes_read == test_string_view.size());
        REQUIRE(rng::equal(std::begin(buf),
                           std::begin(buf) + test_string_view.size(),
                           std::cbegin(test_string_view),
                           std::cend(test_string_view)));

        SECTION("...and further reads result in an error") {
            REQUIRE_NOTHROW(bytes_read = d.read_some(io::buffer(buf), ec));
            REQUIRE(ec == io::stream_errc::eof);
            REQUIRE(bytes_read == 0);
            REQUIRE(rng::equal(std::begin(buf),
                               std::begin(buf) + test_string_view.size(),
                               std::cbegin(test_string_view),
                               std::cend(test_string_view)));
        }
    }

    SECTION("...using a long static buffer (throwing)") {
        std::array<char, 100> buf;
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = d.read_some(io::buffer(buf)));
        REQUIRE(bytes_read == test_string_view.size());
        REQUIRE(rng::equal(std::begin(buf),
                           std::begin(buf) + test_string_view.size(),
                           std::cbegin(test_string_view),
                           std::cend(test_string_view)));

        SECTION("...and further reads result in an error") {
            REQUIRE_THROWS_AS(bytes_read = d.read_some(io::buffer(buf)),
                              std::system_error);
            REQUIRE(bytes_read == test_string_view.size());
            REQUIRE(rng::equal(std::begin(buf),
                               std::begin(buf) + test_string_view.size(),
                               std::cbegin(test_string_view),
                               std::cend(test_string_view)));
        }
    }

    SECTION("...using a dynamic buffer") {
        std::vector<char> buf;
        std::error_code ec;
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = io::read(d, io::dynamic_buffer(buf),
                                 io::transfer_exactly{test_string_view.size()},
                                 ec));
        REQUIRE(bytes_read == test_string_view.size());
        REQUIRE(rng::equal(buf, test_string_view));
    }

    SECTION("...using a dynamic buffer (throwing)") {
        std::vector<char> buf;
        std::size_t bytes_read = 0;
        REQUIRE_NOTHROW(bytes_read = io::read(d, io::dynamic_buffer(buf),
                                   io::transfer_exactly{test_string_view.size()}));
        REQUIRE(bytes_read == test_string_view.size());
        REQUIRE(rng::equal(buf, test_string_view));
    }
}
