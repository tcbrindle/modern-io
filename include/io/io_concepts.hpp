
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef TCB_IO_CONCEPTS_HPP
#define TCB_IO_CONCEPTS_HPP

#include <range/v3/range_concepts.hpp>

#include <io/buffer.hpp>
#include <io/seek_mode.hpp>


namespace io {

namespace detail {

template <class...> using void_t = void;

template<class...> struct disjunction : std::false_type { };
template<class B1> struct disjunction<B1> : B1 { };
template<class B1, class... Bn>
struct disjunction<B1, Bn...>
        : std::conditional_t<bool(B1::value), B1, disjunction<Bn...>>  { };

template <class... B>
constexpr bool disjunction_v = disjunction<B...>::value;


template <typename T>
using read_some_t = decltype(std::declval<T>().read_some(std::declval<io::mutable_buffer>()));

template <typename T>
using read_some_ec_t = decltype(std::declval<T>().read_some(std::declval<io::mutable_buffer>(),
                                                            std::declval<std::error_code&>()));

template <typename T>
using write_some_t = decltype(std::declval<T>().write_some(std::declval<const io::const_buffer>()));

template <typename T>
using write_some_ec_t = decltype(std::declval<T>().write_some(std::declval<const io::const_buffer>(),
                                                              std::declval<std::error_code&>()));

template <typename T>
using seek_result_t = decltype(std::declval<T>().seek(std::declval<typename T::offset_type>(),
                                                      std::declval<io::seek_mode>()));


template <typename T, typename = void_t<>>
struct is_sync_read_stream_impl : std::false_type {};

template <typename T>
struct is_sync_read_stream_impl<T, void_t<
        std::is_same<std::size_t, read_some_t<T>>,
        std::is_same<std::size_t, read_some_ec_t<T>>
>> : std::true_type {};


template <typename T, typename = void>
struct is_sync_write_stream_impl : std::false_type {};

template <typename T>
struct is_sync_write_stream_impl<T, void_t<
    std::is_same<std::size_t, write_some_t<T>>,
    std::is_same<std::size_t, write_some_ec_t<T>>
>> : std::true_type {};

template <typename T, typename = void>
struct is_seekable_stream_impl : std::false_type {};

template <typename T>
struct is_seekable_stream_impl<T, void_t<
    disjunction<is_sync_read_stream_impl<T>, is_sync_write_stream_impl<T>>,
    std::is_same<typename T::offset_type, seek_result_t<T>>
>> : std::true_type {};

}

template <typename T>
struct is_sync_read_stream : detail::is_sync_read_stream_impl<T> {};

template <typename T>
constexpr bool is_sync_read_stream_v = is_sync_read_stream<T>::value;

template <typename T>
using is_sync_write_stream = detail::is_sync_write_stream_impl<T>;

template <typename T>
constexpr bool is_sync_write_stream_v = is_sync_write_stream<T>::value;

template <typename T>
using is_seekable_stream = detail::is_seekable_stream_impl<T>;

template <typename T>
constexpr bool is_seekable_stream_v = is_seekable_stream<T>::value;

namespace rng = ranges::v3;

namespace concepts {

using rng::concepts::convertible_to;
using rng::concepts::refines;
using rng::concepts::models;
using rng::concepts::valid_expr;

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
using SyncReadStream = is_sync_read_stream<T>;

template <typename T>
using SyncWriteStream = is_sync_write_stream<T>;

template <typename T>
using SeekableStream = is_seekable_stream<T>;

template <typename T>
using StreamReader = concepts::models<concepts::StreamReader, T>;

template <typename T>
using StreamWriter = concepts::models<concepts::StreamWriter, T>;


} // end namespace io

#endif
