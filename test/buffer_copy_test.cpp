
#include "catch.hpp"

#include <io/buffer.hpp>

TEST_CASE("Single buffers can be copied into long buffers", "[net]")
{
    const std::string source = "1234567890";
    std::string dest(2 * source.size(), '\0');
    std::size_t bytes_copied = 0;

    REQUIRE_NOTHROW(bytes_copied = io::buffer_copy(io::buffer(dest), io::buffer(source)));
    REQUIRE(bytes_copied == source.size());
    REQUIRE(source == dest.substr(0, source.size()));
}

TEST_CASE("Single buffers can be copied into short buffers", "[net]")
{
    const std::string source = "1234567890";
    std::string dest(source.size()/2, '\0');
    std::size_t bytes_copied = 0;

    REQUIRE_NOTHROW(bytes_copied = io::buffer_copy(io::buffer(dest), io::buffer(source)));
    REQUIRE(bytes_copied == dest.size());
    REQUIRE(source.substr(0, dest.size()) == dest);
}

TEST_CASE("Single buffers can be copied with a maximum size", "[net]")
{
    const std::string source = "1234567890";
    std::string dest(source.size(), '\0');
    std::size_t bytes_copied = 0;
    std::size_t transfer_size = source.size()/2;

    REQUIRE_NOTHROW(bytes_copied = io::buffer_copy(io::buffer(dest), io::buffer(source),
                                                    transfer_size));
    REQUIRE(bytes_copied == source.size()/2);
    REQUIRE(dest.substr(0, transfer_size) == source.substr(0, transfer_size));
    REQUIRE(dest.substr(transfer_size) == std::string(source.size() - transfer_size, '\0'));
}

TEST_CASE("Multiple buffers can be copied into multiple buffers", "[net]")
{
    const std::string src1 = "1234567890";
    const std::string src2 = "abcdefghij";

    std::string dst1(src1.size()/2, '\0');
    std::string dst2(src1.size()/2, '\0');
    std::string dst3(src2.size()/2, '\0');
    std::string dst4(src2.size()/2, '\0');

    const std::vector<io::const_buffer> source{io::buffer(src1),
                                               io::buffer(src2)};
    std::vector<io::mutable_buffer> dest{io::buffer(dst1),
                                         io::buffer(dst2),
                                         io::buffer(dst3),
                                         io::buffer(dst4)};
    std::size_t bytes_copied = 0;

    REQUIRE_NOTHROW(bytes_copied = io::buffer_copy(dest, source));
    REQUIRE(bytes_copied == src1.size() + src2.size());
    REQUIRE(src1 + src2 == dst1 + dst2 + dst3 + dst4);
}

TEST_CASE("Multiple buffers can be copied into multiple buffers with max size", "[net]")
{
    const std::string src1 = "1234567890";
    const std::string src2 = "abcdefghij";

    std::string dst1(src1.size()/2, '\0');
    std::string dst2(src1.size()/2, '\0');
    std::string dst3(src2.size()/2, '\0');
    std::string dst4(src2.size()/2, '\0');

    const std::vector<io::const_buffer> source{io::buffer(src1),
                                               io::buffer(src2)};
    std::vector<io::mutable_buffer> dest{io::buffer(dst1),
                                         io::buffer(dst2),
                                         io::buffer(dst3),
                                         io::buffer(dst4)};
    std::size_t transfer_size = 12;
    std::size_t bytes_copied = 0;

    REQUIRE_NOTHROW(bytes_copied = io::buffer_copy(dest, source, transfer_size));
    REQUIRE(bytes_copied == transfer_size);
    REQUIRE(src1 == dst1 + dst2);
    REQUIRE(dst3.substr(0, 2) == "ab");
    REQUIRE(dst4 == std::string(src1.size()/2, '\0'));
}