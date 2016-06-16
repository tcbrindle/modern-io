
#include "catch.hpp"

#include <io/locking.hpp>
#include <io/string_stream.hpp>

#include <thread>

TEST_CASE("Basic lockable test", "[lockable]")
{
    auto str = io::lockable<io::string_stream>{};

    const auto thread_func = [&str] (int id) {
        const std::string s(2, '0' + id);
        auto stream = io::lock(str);
        stream.write_some(io::buffer(s, 1));
        std::this_thread::yield();
        stream.write_some(io::buffer(s) + 1);
    };

    std::thread t1{thread_func, 1};
    std::thread t2{thread_func, 2};

    t2.join();
    t1.join();

    const auto output = str.lock().base().str();

    const auto output_ok = [&output] {
        return output == "1122" || output == "2211";
    };

    REQUIRE(output_ok());
}
