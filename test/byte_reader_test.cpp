
#include "catch.hpp"

#include <io/string_stream.hpp>
#include <io/byte_reader.hpp>

TEST_CASE("Basic byte_range test")
{
    const std::string string = "The quick brown fox jumps over the lazy dog";

    io::string_stream stream(string);

    io::byte_reader<io::string_stream> range(stream);

    REQUIRE(std::equal(std::begin(range), std::end(range),
                       std::begin(string), std::end(string)));
}