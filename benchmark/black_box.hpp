
#ifndef MODERN_IO_BLACK_BOX_HPP
#define MODERN_IO_BLACK_BOX_HPP

#include <memory>

namespace io {

namespace detail {
void black_box_impl(...);
}

/// A function which does nothing, but is opaque to the compiler.
/// The compiler cannot see the definition of the inner function, so while it
/// will inline this wrapper, it cannot optimise away the thing that is passed.
template <typename T>
void black_box(T&& t) {
    detail::black_box_impl(std::addressof(t));
}

}

#endif // MODERN_IO_BLACK_BOX_HPP
