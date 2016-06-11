
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>

#include <io/file.hpp>
#include <io/read.hpp>
#include <io/stream_reader.hpp>


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

std::vector<std::uint8_t> read_stdio(const char* file_name)
{
    FILE* file = std::fopen(file_name, "rb");
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

std::vector<std::uint8_t> read_iostream(const char* file_name)
{
    std::ifstream file{};
    file.exceptions(std::ios::badbit);
    file.open(file_name, std::ios_base::binary);

    return std::vector<uint8_t>(std::istreambuf_iterator<char>(file),
                                std::istreambuf_iterator<char>());
}

std::vector<std::uint8_t> read_modern(const char* file_name)
{
    auto file = io::open_file(file_name);
    std::vector<uint8_t> output;
    io::read_all(file, io::dynamic_buffer(output));
    return output;
}

}

int main(int argc, char** argv)
{
    if (argc < 2) {
        std::cout << "Pass a filename to read\n";
        return 1;
    }

    auto file_name = argv[1];

    {
        auto t = timer{};
        auto v = read_stdio(file_name);
        auto e = t.elapsed();
        std::cout << "stdio read took " << e.count() << "ms\n";
    }

    {
        auto t = timer{};
        auto v = read_iostream(file_name);
        auto e = t.elapsed();
        std::cout << "ifstream read took " << e.count() << "ms\n";
    }

    {
        auto t = timer{};
        auto v = read_modern(file_name);
        auto e = t.elapsed();
        std::cout << "modern::io read took " << e.count() << "ms\n";
    }
}
