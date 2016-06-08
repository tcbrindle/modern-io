
#ifndef TCB_IO_CONCEPTS_HPP
#define TCB_IO_CONCEPTS_HPP

#include <range/v3/range_concepts.hpp>

#include <io/buffer.hpp>

// Forward definitions
namespace asio {
class mutable_buffer;
class const_buffer;
}

namespace io {

namespace rng = ranges::v3;

namespace concepts {

using rng::concepts::convertible_to;
using rng::concepts::refines;
using rng::concepts::models;
using rng::concepts::valid_expr;

struct SyncReadStream
{
    template <typename T>
    auto requires_(T&& t) -> void;
};

struct SyncWriteStream
{
    template <typename T>
    auto requires_(T&& t) -> void;
};

struct SeekableStream
    : refines<SyncReadStream>
{
    template <typename T>
    using offset_t = typename T::offset_type;

    //template <typename T>
    //auto requires_(T& t, offset_t<T> offset, seek_mode mode) -> decltype(
    //    rng::concepts::has_type<offset_t<T>>(t.seek(offset, mode))
    //);
};

struct StreamReader
    : refines<rng::concepts::InputRange>
{
};

struct StreamWriter
    : refines<rng::concepts::OutputRange>
{
};

} // end namespace concepts


template <typename T>
using MutableBufferSequence = net::is_mutable_buffer_sequence<T>;

template <typename T>
using ConstBufferSequence = net::is_const_buffer_sequence<T>;

template <typename T>
using DynamicBuffer = net::is_dynamic_buffer<T>;

template <typename T>
using SyncReadStream = concepts::models<concepts::SyncReadStream, T>;

template <typename T>
using SyncWriteStream = concepts::models<concepts::SyncWriteStream, T>;

template <typename T>
using SeekableStream = concepts::models<concepts::SeekableStream, T>;

template <typename T>
using StreamReader = concepts::models<concepts::StreamReader, T>;

template <typename T>
using StreamWriter = concepts::models<concepts::StreamWriter, T>;


} // end namespace io

#endif
