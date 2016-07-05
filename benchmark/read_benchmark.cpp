
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <cerrno>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>

#include <io/file.hpp>
#include <io/read.hpp>
#include <io/stream_reader.hpp>

#ifdef _POSIX_VERSION
#include <io/posix/mmap_file.hpp>
#endif

#include <range/v3/algorithm/copy.hpp>

#include "black_box.hpp"

namespace {

struct timer {
    using clock_type = std::chrono::high_resolution_clock;

    timer() = default;

    template <typename DurationType = std::chrono::milliseconds>
    DurationType elapsed() const
    {
        return std::chrono::duration_cast<DurationType>(clock_type::now() - start_);
    }

private:
    clock_type::time_point start_ = clock_type::now();
};

/*
 * stdio tests
 */

// Incrementally reads from a FILE*, with (hopefully) good error checking
std::vector<std::uint8_t> read_stdio_inc(const char* file_name)
{
    FILE* file = std::fopen(file_name, "rb");
    if (!file) {
        throw std::system_error{errno, std::system_category()};
    }
    constexpr std::size_t read_size = 65'536;
    std::vector<uint8_t> output{};
    std::size_t total_read_size = 0;
    std::error_code ec;

    while (true) {
        auto current_size = output.size();
        output.resize(current_size + read_size);
        auto bytes_read = std::fread(output.data() + current_size, 1, read_size, file);
        total_read_size += bytes_read;
        if (bytes_read < read_size) {
            if (!feof(file)) {
                ec.assign(errno, std::system_category());
            }
            break;
        }
    }

    output.resize(total_read_size);
    std::fclose(file);

    if (ec) {
        throw std::system_error{ec};
    }

    return output;
}

// Read from a FILE* into a preallocated vector with good error checking
std::vector<std::uint8_t> read_stdio_prealloc(const char* file_name)
{
    FILE* file = std::fopen(file_name, "rb");
    if (!file) {
        throw std::system_error{errno, std::system_category()};
    }
    // Seek to the end of the stream to find the size
    if (std::fseek(file, 0, SEEK_END) != 0) {
        std::fclose(file);
        throw std::system_error{errno, std::system_category()};
    }
    std::size_t file_size = std::ftell(file);
    std::rewind(file);

    std::vector<std::uint8_t> output(file_size);
    std::size_t bytes_read = std::fread(output.data(), 1, file_size, file);

    if (bytes_read != file_size) {
        std::fclose(file);
        throw std::system_error{errno, std::system_category()};
    }

    std::fclose(file);

    return output;
}

/*
 * iostreams tests
 */

// Read from an ifstream into a preallocated vector with exceptions enabled
std::vector<std::uint8_t> read_iostream_prealloc(const char* file_name)
{
    std::ifstream file{};
    file.exceptions(std::ios::badbit);
    file.open(file_name, std::ios_base::binary);

    file.seekg(0, std::ios_base::end);
    auto file_size = file.tellg();
    file.seekg(0);
    std::vector<std::uint8_t> output(file_size);

    file.read(reinterpret_cast<char*>(output.data()), file_size);

    return output;
}

// Read from an ifstream into a vector using istreambuf_iterator
std::vector<std::uint8_t> read_iostream_range(const char* file_name)
{
    std::ifstream file{};
    file.exceptions(std::ios::badbit);
    file.open(file_name, std::ios_base::binary);

    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file),
                                std::istreambuf_iterator<char>());
}

// Read from an ifstream into a preallocated vector using istreambuf iterator
std::vector<std::uint8_t> read_iostream_range_prealloc(const char* file_name)
{
    std::ifstream file{};
    file.exceptions(std::ios::badbit);
    file.open(file_name, std::ios_base::binary);

    file.seekg(0, std::ios_base::end);
    auto file_size = file.tellg();
    file.seekg(0);

    std::vector<std::uint8_t> output;
    output.reserve(file_size);

    std::copy(std::istreambuf_iterator<char>(file),
              std::istreambuf_iterator<char>(),
              std::back_inserter(output));

    return output;
}


/*
 * modern::io file tests
 */

// Incrementally reads from a file, with good error checking behind the scenes
std::vector<std::uint8_t> read_modern_inc(const char* file_name)
{
    auto file = io::open_file(file_name, io::open_mode::read_only);
    std::vector<uint8_t> output;
    io::read_all(file, io::dynamic_buffer(output));
    return output;
}


// Read a file into a preallocated vector with exceptions
std::vector<std::uint8_t> read_modern_prealloc(const char* file_name)
{
    auto file = io::open_file(file_name, io::open_mode::read_only);
    auto file_size = io::seek(file, 0, io::seek_mode::end);
    io::seek(file, 0, io::seek_mode::start);
    std::vector<std::uint8_t> output(file_size);
    io::read(file, io::buffer(output));
    return output;
}

// Read a file into a vector using a stream reader (in two lines!)
std::vector<std::uint8_t> read_modern_range(const char* file_name)
{
    auto file = io::open_file(file_name, io::open_mode::read_only);
    return io::read(std::move(file));
}

// Read a file into a preallocated vector using a stream reader
std::vector<std::uint8_t> read_modern_range_prealloc(const char* file_name)
{
    auto file = io::open_file(file_name, io::open_mode::read_only);
    auto file_size = io::seek(file, 0, io::seek_mode::end);
    io::seek(file, 0, io::seek_mode::start);
    std::vector<std::uint8_t> output;
    output.reserve(file_size);

    auto reader = io::read(std::move(file));

    io::rng::copy(reader, io::rng::back_inserter(output));

    return output;
}

#ifdef _POSIX_VERSION
/*
 * modern::io mmap tests
 */

// Read an mmap'd file into a preallocated vector with exceptions
std::vector<std::uint8_t> read_modern_mmap_prealloc(const char* file_name)
{
    auto file = io::posix::mmap_file{file_name, io::open_mode::read_only};
    std::vector<uint8_t> output(file.size());
    io::read(file, io::buffer(output));
    return output;
}

// Read an mmap'd file into a vector using a stream reader
std::vector<std::uint8_t> read_modern_mmap_range(const char* file_name)
{
    auto file = io::posix::mmap_file{file_name, io::open_mode::read_only};
    return io::read(std::move(file));
}

// Read an mmap'd fine into a preallocated vector using a stream reader
std::vector<std::uint8_t> read_modern_mmap_range_prealloc(const char* file_name)
{
    auto file = io::posix::mmap_file{file_name, io::open_mode::read_only};
    std::vector<std::uint8_t> output;
    output.reserve(file.size());
    io::rng::copy(io::read(std::move(file)), io::rng::back_inserter(output));
    return output;
}

#endif // _POSIX_VERSION

} // end anonymous namespace

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Pass a filename to read\n";
        return 1;
    }

    auto file_name = argv[1];
    const bool check = argc > 2 && argv[2] == std::string("check");

    using test_entry = std::pair<std::string, std::vector<std::uint8_t>(*)(const char*)>;
#ifdef _POSIX_VERSION
    const std::vector<test_entry> tests {
            { "stdio incremental read", read_stdio_inc },
            { "modern::io incremental read", read_modern_inc },
            { "stdio preallocated read", read_stdio_prealloc },
            { "iostream preallocated read", read_iostream_prealloc },
            { "modern::io preallocated read", read_modern_prealloc },
            { "modern::io preallocated mmap read", read_modern_mmap_prealloc },
            { "iostream incremental range read", read_iostream_range },
            { "modern::io incremental range read", read_modern_range },
            { "modern::io incremental mmap range read", read_modern_mmap_range },
            { "iostream preallocated range read", read_iostream_range_prealloc },
            { "modern::io preallocated range read", read_modern_range_prealloc },
            { "modern::io preallocated mmap range read", read_modern_mmap_range_prealloc },
    };
#else // !_POSIX_VERSION
    const std::vector<test_entry> tests {
            { "stdio incremental read", read_stdio_inc },
            { "modern::io incremental read", read_modern_inc },
            { "stdio preallocated read", read_stdio_prealloc },
            { "iostream preallocated read", read_iostream_prealloc },
            { "modern::io preallocated read", read_modern_prealloc },
            { "iostream incremental range read", read_iostream_range },
            { "modern::io incremental range read", read_modern_range },
            { "iostream preallocated range read", read_iostream_range_prealloc },
            { "modern::io preallocated range read", read_modern_range_prealloc },
    };
#endif // _POSIX_VERSION

    std::chrono::microseconds reference_time;
    std::size_t file_size = 0;

    std::vector<uint8_t> reference;
    {
        std::cout << "warming cache and checking disk speed... " << std::flush;
        timer t{};
        auto v = read_stdio_inc(file_name);
        reference_time = t.elapsed<std::chrono::microseconds>();
        file_size = v.size();
        if (check) {
            reference = std::move(v);
        }
        std::cout << "done\n";
    }

    // A good heuristic for meaningful results seems to be to make the fastest
    // tests take ~2 seconds
    auto n_times = std::max<long>(std::chrono::seconds{2}/reference_time, 1);

    std::cout << "Performing " << n_times << " read(s) of "
              << file_name << " (" << file_size << " bytes, "
              << file_size * n_times << " total) per test" << std::endl;

    for (const auto& test : tests) {
        std::cout << test.first << " ";
        try {
            auto t = timer{};
            std::vector<std::uint8_t> v;
            for (int i = 0; i < n_times; i++) {
                io::black_box(v = test.second(file_name));
            }
            auto e = t.elapsed();
            if (check && v != reference) {
                throw std::runtime_error{"read does not match reference output"};
            }
            std::cout << "took " << e.count() << "ms ("
                      << n_times * file_size/(1000.0 * e.count()) << "MB/s)\n";
        } catch (const std::exception& e){
            std::cout << "-- ERROR " << e.what() << "\n";
        }
    }
}
