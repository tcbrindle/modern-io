
#include "catch.hpp"

#include <io/read.hpp>
#include <io/read_only.hpp>
#include <io/string_stream.hpp>
#include <io/write_only.hpp>

TEST_CASE("read_only works as expected", "[read]")
{
    auto stream = io::read_only<io::string_stream>{"Hello world"};
    static_assert(io::is_sync_read_stream_v<decltype(stream)>,
                  "read_only stream should be readable!");
    static_assert(!io::is_sync_write_stream_v<decltype(stream)>,
                  "read_only stream should not be writable");
    static_assert(io::is_seekable_stream_v<decltype(stream)>,
                  "read_only<string_stream> should be seekable");
    std::string str;
    REQUIRE_NOTHROW(io::read_all(stream, io::dynamic_buffer(str)));
    REQUIRE(str == stream.next_layer().str());
}

TEST_CASE("write_only works as expected", "[write]")
{
    auto stream = io::write_only<io::string_stream>{};
    static_assert(!io::is_sync_read_stream_v<decltype(stream)>,
                  "read_only stream not should be readable");
    static_assert(io::is_sync_write_stream_v<decltype(stream)>,
                  "write_only stream should not be writable!");
    static_assert(io::is_seekable_stream_v<decltype(stream)>,
                  "write_only<string_stream> should be seekable");
    const io_std::string_view str = "Hello world";
    REQUIRE_NOTHROW(io::write(stream, io::buffer(str)));
    REQUIRE(str == stream.next_layer().str());
}
