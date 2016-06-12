
#include "catch.hpp"

#include <io/debug/debug_stream.hpp>
#include <io/stream_reader.hpp>

#include <range/v3/algorithm/copy.hpp>

#include <iostream>

TEST_CASE("Debug streams can be printed", "[debug]")
{
    auto stream = io::debug_stream("Hello, world!\n");
    auto reader = io::read(std::move(stream));

    io::rng::copy(reader,
                  io::rng::ostream_iterator<char>(std::cout));
}