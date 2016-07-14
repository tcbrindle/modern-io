
#include "catch.hpp"

#include <io/copy.hpp>
#include <io/string_stream.hpp>

TEST_CASE("io::copy works as expected", "[copy]")
{
    io::string_stream i{"Hello world"};
    io::string_stream o;
    std::error_code ec;
    std::size_t bytes = 0;

    REQUIRE_NOTHROW(bytes = io::copy(i, o, ec));
    REQUIRE(bytes == i.str().size());
    REQUIRE_FALSE(ec);
    REQUIRE(i.str() == o.str());
}

TEST_CASE("io::copy works as expected (throwing)", "[copy]")
{
    io::string_stream i{"Hello world"};
    io::string_stream o;
    std::size_t bytes = 0;

    REQUIRE_NOTHROW(bytes = io::copy(i, o));
    REQUIRE(bytes == i.str().size());
    REQUIRE(i.str() == o.str());
}
