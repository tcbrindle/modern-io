
#include "catch.hpp"

#include <io/string_stream.hpp>
#include <io/stream_reader.hpp>

#include <range/v3/algorithm/copy.hpp>

#include <iostream>

TEST_CASE("String streams can be printed", "[string_stream]")
{
    auto stream = io::string_stream("Hello, world!\n");
    auto reader = io::read(std::move(stream));

    io::rng::copy(reader,
                  io::rng::ostream_iterator<char>(std::cout));
}