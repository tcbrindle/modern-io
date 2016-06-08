
#ifndef IO_TYPES_HPP
#define IO_TYPES_HPP

#include <range/v3/range_fwd.hpp>
#include <string>

namespace io {

enum class byte : unsigned char {};

enum class char8_t : char {};

using u8string = std::basic_string<char8_t>;

namespace rng = ranges::v3;

}

#endif // IO_TYPES_HPP
