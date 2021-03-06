
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef TCB_IO_CONCEPTS_HPP
#define TCB_IO_CONCEPTS_HPP

#include <io/buffer.hpp>
#include <io/seek_mode.hpp>

namespace io {

namespace detail {

template <class...> using void_t = void;

template<class...> struct conjunction : std::true_type { };
template<class B1> struct conjunction<B1> : B1 { };
template<class B1, class... Bn>
struct conjunction<B1, Bn...>
        : std::conditional_t<bool(B1::value), conjunction<Bn...>, B1> {};

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
using seek_result_t = decltype(std::declval<T>().seek(0, io::seek_mode::current));

template <typename T>
using seek_ec_result_t = decltype(std::declval<T>().seek(0, io::seek_mode::current, std::declval<std::error_code&>()));

template <typename T>
using offset_type_t = typename T::offset_type;

template <typename T, typename = void>
struct is_stream_position_impl : std::false_type {};

template <typename T>
struct is_stream_position_impl<T, void_t<
    std::enable_if_t<std::is_signed<offset_type_t<T>>::value>,
    std::enable_if_t<std::is_default_constructible<T>::value>,
    std::enable_if_t<std::is_constructible<T, offset_type_t<T>>::value>,
    std::enable_if_t<std::is_same<offset_type_t<T>, decltype(std::declval<const T>().offset_from_start())>::value>,
    std::enable_if_t<std::is_same<T&, decltype(std::declval<T>() += std::declval<offset_type_t<T>>())>::value>,
    std::enable_if_t<std::is_same<T&, decltype(std::declval<T>() += std::declval<offset_type_t<T>>())>::value>,
    std::enable_if_t<std::is_same<T, decltype(std::declval<T>() + std::declval<offset_type_t<T>>())>::value>,
    std::enable_if_t<std::is_same<T, decltype(std::declval<T>() - std::declval<offset_type_t<T>>())>::value>,
    std::enable_if_t<std::is_same<offset_type_t<T>, decltype(std::declval<T>() - std::declval<T>())>::value>
>> : std::true_type {};

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
    std::enable_if_t<std::is_same<seek_result_t<T>, seek_ec_result_t<T>>::value>,
    std::enable_if_t<is_stream_position_impl<seek_result_t<T>>::value>
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

template <typename T>
using position_type = detail::seek_result_t<T>;

template <typename T>
using offset_type = typename position_type<T>::offset_type;


} // end namespace io

#endif
