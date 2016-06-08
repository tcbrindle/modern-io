
#ifndef IO_NET_BUFFER_HPP
#define IO_NET_BUFFER_HPP

#include <array>
#include <io/io_std/string_view.hpp>
#include <string>
#include <system_error>
#include <vector>

// Networking TS 16.1 Header <experimental/buffer>

namespace io {
namespace net {

// N.B. std::is_error_code_enum specialisation at end of file
enum class stream_errc {
    eof = 1,
    not_found
};

const std::error_category& stream_category() noexcept;

std::error_code make_error_code(stream_errc e) noexcept;
std::error_condition make_error_condition(stream_errc e) noexcept;

class mutable_buffer;

class const_buffer;

// Buffer type traits

template <class T>
struct is_mutable_buffer_sequence;
template <class T>
struct is_const_buffer_sequence;
template <class T>
struct is_dynamic_buffer;

// Buffer sequence access

const mutable_buffer* buffer_sequence_begin(const mutable_buffer& b);

const const_buffer* buffer_sequence_begin(const const_buffer& b);

const mutable_buffer* buffer_sequence_end(const mutable_buffer& b);

const const_buffer* buffer_sequence_end(const const_buffer& b);

template <class C>
auto buffer_sequence_begin(C& c) -> decltype(c.begin());

template <class C>
auto buffer_sequence_begin(const C& c) -> decltype(c.begin());

template <class C>
auto buffer_sequence_end(C& c) -> decltype(c.end());

template <class C>
auto buffer_sequence_end(const C& c) -> decltype(c.end());

// Buffer size

template <class ConstBufferSequence_>
std::size_t buffer_size(const ConstBufferSequence_& buffers) noexcept;

// Buffer copy

template <class MutableBufferSequence_, class ConstBufferSequence_>
std::size_t buffer_copy(const MutableBufferSequence_& dest,
                        const ConstBufferSequence_& source) noexcept;

template <class MutableBufferSequence_, class ConstBufferSequence_>
std::size_t buffer_copy(const MutableBufferSequence_& dest,
                        const ConstBufferSequence_& source,
                        std::size_t max_size) noexcept;

// Buffer arithmetic

mutable_buffer operator+(const mutable_buffer& b, std::size_t n) noexcept;

mutable_buffer operator+(std::size_t n, const mutable_buffer& b) noexcept;

const_buffer operator+(const const_buffer&, std::size_t n) noexcept;

const_buffer operator+(std::size_t, const const_buffer&) noexcept;

// Buffer creation

mutable_buffer buffer(void* p, std::size_t n) noexcept;

const_buffer buffer(const void* p, std::size_t n) noexcept;

mutable_buffer buffer(const mutable_buffer& b) noexcept;

mutable_buffer buffer(const mutable_buffer& b, std::size_t n) noexcept;

const_buffer buffer(const const_buffer& b) noexcept;

const_buffer buffer(const const_buffer& b, std::size_t n) noexcept;

template <class T, std::size_t N>
mutable_buffer buffer(T (& data)[N]) noexcept;

template <class T, std::size_t N>
const_buffer buffer(const T (& data)[N]) noexcept;

template <class T, std::size_t N>
mutable_buffer buffer(std::array<T, N>& data) noexcept;

template <class T, std::size_t N>
const_buffer buffer(std::array<const T, N>& data) noexcept;

template <class T, std::size_t N>
const_buffer buffer(const std::array<T, N>& data) noexcept;

template <class T, class Allocator>
mutable_buffer buffer(std::vector<T, Allocator>& data) noexcept;

template <class T, class Allocator>
const_buffer buffer(const std::vector<T, Allocator>& data) noexcept;

template <class CharT, class Traits, class Allocator>
mutable_buffer
buffer(std::basic_string<CharT, Traits, Allocator>& data) noexcept;

template <class CharT, class Traits, class Allocator>
const_buffer
buffer(const std::basic_string<CharT, Traits, Allocator>& data) noexcept;

#ifdef IO_HAVE_STRING_VIEW
template<class CharT, class Traits>
const_buffer buffer(io_std::basic_string_view<CharT, Traits> data) noexcept;
#endif // IO_HAVE_STRING_VIEW

template <class T, std::size_t N>
mutable_buffer buffer(T (& data)[N], std::size_t n) noexcept;

template <class T, std::size_t N>
const_buffer buffer(const T (& data)[N], std::size_t n) noexcept;

template <class T, std::size_t N>
mutable_buffer buffer(std::array<T, N>& data, std::size_t n) noexcept;

template <class T, std::size_t N>
const_buffer buffer(std::array<const T, N>& data, std::size_t n) noexcept;

template <class T, std::size_t N>
const_buffer buffer(const std::array<T, N>& data, std::size_t n) noexcept;

template <class T, class Allocator>
mutable_buffer buffer(std::vector<T, Allocator>& data, std::size_t n) noexcept;

template <class T, class Allocator>
const_buffer
buffer(const std::vector<T, Allocator>& data, std::size_t n) noexcept;

template <class CharT, class Traits, class Allocator>
mutable_buffer buffer(std::basic_string<CharT, Traits, Allocator>& data,
                      std::size_t n) noexcept;

template <class CharT, class Traits, class Allocator>
const_buffer buffer(const std::basic_string<CharT, Traits, Allocator>& data,
                    std::size_t n) noexcept;

#ifdef IO_HAVE_STRING_VIEW
template<class CharT, class Traits>
const_buffer buffer(io_std::basic_string_view<CharT, Traits> data,
                    std::size_t n) noexcept;
#endif // IO_HAVE_STRING_VIEW

template <class T, class Allocator>
class dynamic_vector_buffer;

template <class CharT, class Traits, class Allocator>
class dynamic_string_buffer;

// Dynamic buffer creation

template <class T, class Allocator>
dynamic_vector_buffer<T, Allocator>
dynamic_buffer(std::vector<T, Allocator>& vec) noexcept;

template <class T, class Allocator>
dynamic_vector_buffer<T, Allocator>
dynamic_buffer(std::vector<T, Allocator>& vec, std::size_t n) noexcept;

template <class CharT, class Traits, class Allocator>
dynamic_string_buffer<CharT, Traits, Allocator>
dynamic_buffer(std::basic_string<CharT, Traits, Allocator>& str) noexcept;

template <class CharT, class Traits, class Allocator>
dynamic_string_buffer<CharT, Traits, Allocator>
dynamic_buffer(std::basic_string<CharT, Traits, Allocator>& str,
               std::size_t n) noexcept;

class transfer_all;

class transfer_at_least;

class transfer_exactly;

// Synchronous read operations

template <class SyncReadStream, class MutableBufferSequence>
std::size_t read(SyncReadStream& stream,
                 const MutableBufferSequence& buffers);

template <class SyncReadStream, class MutableBufferSequence>
std::size_t read(SyncReadStream& stream,
                 const MutableBufferSequence& buffers, std::error_code& ec);

template <class SyncReadStream, class MutableBufferSequence,
        class CompletionCondition>
std::size_t read(SyncReadStream& stream,
                 const MutableBufferSequence& buffers,
                 CompletionCondition completion_condition);

template <class SyncReadStream, class MutableBufferSequence,
          class CompletionCondition>
std::size_t read(SyncReadStream& stream,
                 const MutableBufferSequence& buffers,
                 CompletionCondition completion_condition,
                 std::error_code& ec);

template <class SyncReadStream, class DynamicBuffer,
          class = std::enable_if_t<is_dynamic_buffer<std::decay_t<DynamicBuffer>>::value>>
std::size_t read(SyncReadStream& stream, DynamicBuffer&& b);

template <class SyncReadStream, class DynamicBuffer,
          class = std::enable_if_t<is_dynamic_buffer<std::decay_t<DynamicBuffer>>::value>>
std::size_t read(SyncReadStream& stream, DynamicBuffer&& b, std::error_code& ec);

template <class SyncReadStream, class DynamicBuffer, class CompletionCondition,
          class = std::enable_if_t<is_dynamic_buffer<std::decay_t<DynamicBuffer>>::value>>
std::size_t read(SyncReadStream& stream, DynamicBuffer&& b,
                 CompletionCondition completion_condition);

template <class SyncReadStream, class DynamicBuffer, class CompletionCondition,
          class = std::enable_if_t<is_dynamic_buffer<std::decay_t<DynamicBuffer>>::value>>
std::size_t read(SyncReadStream& stream, DynamicBuffer&& b,
                 CompletionCondition completion_condition, std::error_code& ec);

// Asynchronous read operations -- NOT IMPLEMENTED

// Synchronous write operations

template <class SyncWriteStream, class ConstBufferSequence,
          class = std::enable_if_t<is_const_buffer_sequence<ConstBufferSequence>::value>>
std::size_t write(SyncWriteStream& stream,
                  const ConstBufferSequence& buffers);

template <class SyncWriteStream, class ConstBufferSequence,
          class = std::enable_if_t<is_const_buffer_sequence<ConstBufferSequence>::value>>
std::size_t write(SyncWriteStream& stream,
                  const ConstBufferSequence& buffers, std::error_code& ec);

template <class SyncWriteStream, class ConstBufferSequence,
        class CompletionCondition,
                class = std::enable_if_t<is_const_buffer_sequence<ConstBufferSequence>::value>>
std::size_t write(SyncWriteStream& stream,
                  const ConstBufferSequence& buffers,
                  CompletionCondition completion_condition);

template <class SyncWriteStream, class ConstBufferSequence,
        class CompletionCondition,
                class = std::enable_if_t<is_const_buffer_sequence<ConstBufferSequence>::value>>
std::size_t write(SyncWriteStream& stream,
                  const ConstBufferSequence& buffers,
                  CompletionCondition completion_condition,
                  std::error_code& ec);

template <class SyncWriteStream, class DynamicBuffer,
          class = std::enable_if_t<is_dynamic_buffer<DynamicBuffer>::value>>
std::size_t write(SyncWriteStream& stream, DynamicBuffer&& b);

template <class SyncWriteStream, class DynamicBuffer,
          class = std::enable_if_t<is_dynamic_buffer<DynamicBuffer>::value>>
std::size_t write(SyncWriteStream& stream, DynamicBuffer&& b, std::error_code& ec);

template <class SyncWriteStream, class DynamicBuffer, class CompletionCondition,
          class = std::enable_if_t<is_dynamic_buffer<DynamicBuffer>::value>>
std::size_t write(SyncWriteStream& stream, DynamicBuffer&& b,
                  CompletionCondition completion_condition);

template <class SyncWriteStream, class DynamicBuffer, class CompletionCondition,
          class = std::enable_if_t<is_dynamic_buffer<DynamicBuffer>::value>>
std::size_t write(SyncWriteStream& stream, DynamicBuffer&& b,
                  CompletionCondition completion_condition,
                  std::error_code& ec);

// Asynchronous write operations -- NOT IMPLEMENTED

// Synchronous delimited read operations

template <class SyncReadStream, class DynamicBuffer>
std::size_t read_until(SyncReadStream& s, DynamicBuffer&& b, char delim);

template <class SyncReadStream, class DynamicBuffer>
std::size_t read_until(SyncReadStream& s, DynamicBuffer&& b,
                       char delim, std::error_code& ec);

#ifdef IO_HAVE_STRING_VIEW
template<class SyncReadStream, class DynamicBuffer>
std::size_t read_until(SyncReadStream& s, DynamicBuffer&& b, io_std::string_view delim);
template<class SyncReadStream, class DynamicBuffer>
std::size_t read_until(SyncReadStream& s, DynamicBuffer&& b,
                       io_std::string_view delim, std::error_code& ec);
#endif // IO_HAVE_STRING_VIEW

// Asynchronous delimited read operations -- NOT IMPLEMENTED

} // end namespace net
} // end namespace io

namespace std {

template <>
struct is_error_code_enum<io::net::stream_errc> : std::true_type {};

} // end namespace std

#include "impl/buffer.impl.hpp"

#endif // IO_NET_BUFFER_HPP
