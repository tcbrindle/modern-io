
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "catch.hpp"

#include <io/file.hpp>
#include <io/read.hpp>
//#include <io/posix/mmap_file.hpp>
#include <io/io_std/string_view.hpp>

#include <cstdio>

// FIXME: This entire file needs to be much, much better

const io_std::string_view test_file_contents =
    "The quick brown fox jumps over the lazy dog";

constexpr char test_file_name[] = "io_test_file.txt";

template <typename File>
void test_read(File&& file)
{
    std::string contents;
    REQUIRE_NOTHROW(io::read_all(file, io::dynamic_buffer(contents)));

    REQUIRE((contents == test_file_contents));
}

template <typename File>
void test_write(File&& file)
{
    REQUIRE_NOTHROW(io::write(file, io::buffer(test_file_contents)));

    FILE* in_file = std::fopen(test_file_name, "rb");
    std::string read_buf(test_file_contents.size(), '\0');
    std::fread(&*read_buf.begin(), 1, test_file_contents.size(), in_file);

    REQUIRE(test_file_contents == read_buf);

    std::fclose(in_file);
}

TEST_CASE("Files reads work as expected", "[file]")
{
    // Prepare a test file.
    FILE* out_file = std::fopen(test_file_name, "wb");
    if (!out_file) {
        throw std::system_error{errno, std::generic_category()};
    }

    std::size_t bytes_written = std::fwrite(test_file_contents.data(), sizeof(char),
                                            test_file_contents.size(), out_file);
    if (bytes_written != test_file_contents.size()) {
        fclose(out_file);
        throw std::system_error{errno, std::generic_category()};
    }
    std::fclose(out_file);

    SECTION("io::file can read properly") {
        io::file file{test_file_name, io::open_mode::read_only};
        test_read(file);
    }

   // SECTION("io::posix::mmap_file can read properly") {
   //     io::posix::mmap_file file{test_file_name, io::open_mode::read_only};
   //     test_read(file);
   // }

    std::remove(test_file_name);
}

TEST_CASE("File writes work as expected", "[file]")
{
    SECTION("io::file can write properly") {
        io::file file{test_file_name, io::open_mode::write_only |
                                      io::open_mode::always_create};
        test_write(file);
        std::remove(test_file_name);
    }
}