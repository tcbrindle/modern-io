
#ifndef IO_BUFFER_IMPL_HPP
#define IO_BUFFER_IMPL_HPP

namespace io {
namespace net {

class stream_error_category : public std::error_category
{
    virtual const char* name() const noexcept override
    {
        return "stream";
    }

    virtual std::string message(int condition) const override
    {
        auto se = static_cast<stream_errc>(condition);
        switch (se) {
        case stream_errc::eof:
            return "end of file";
        case stream_errc::not_found:
            return "delimiter not found";
        }
    }
};

// 16.3 Error codes [buffer.err]

inline const std::error_category& stream_category() noexcept
{
    static stream_error_category cat{};
    return cat;
}

inline std::error_code make_error_code(stream_errc e) noexcept
{
    return std::error_code{static_cast<int>(e), stream_category()};
}

inline std::error_condition make_error_condition(stream_errc e) noexcept
{
    return std::error_condition{static_cast<int>(e), stream_category()};
}

// 16.4 Class mutable_buffer [buffer.mutable]

class mutable_buffer {
public:
    mutable_buffer() noexcept = default;

    mutable_buffer(void* p, std::size_t n) noexcept
        : data_{p}, size_{n}
    {}

    void* data() const noexcept { return data_; }

    std::size_t size() const noexcept { return size_; }

private:
    void* data_ = nullptr;
    std::size_t size_ = 0;
};

// 16.5 Class const_buffer [buffer.const]

class const_buffer
{
public:
    const_buffer() noexcept = default;

    const_buffer(const void* p, size_t n) noexcept
        : data_{p}, size_{n}
    {}

    const_buffer(const mutable_buffer& b) noexcept
        : data_{b.data()}, size_{b.size()}
    {}

    const void* data() const noexcept { return data_; }

    size_t size() const noexcept { return size_;}

private:
    const void* data_ = nullptr;
    size_t size_ = 0;
};

// 16.6 Buffer type traits [buffer.traits]
// FIXME: Implement these using standard C++ metaprogramming, and redefine the concepts to use them

namespace detail {

// Metaprogramming facilities from library fundamentals v2

template <class...> using void_t = void;

struct nonesuch {
    nonesuch() = delete;
    ~nonesuch() = delete;
    nonesuch(nonesuch const&) = delete;
    void operator=(nonesuch const&) = delete;
};

template <class Default, class AlwaysVoid,
        template<class...> class Op, class... Args>
struct detector {
    using value_t = std::false_type;
    using type = Default;
};

template <class Default, template<class...> class Op, class... Args>
struct detector<Default, void_t<Op<Args...>>, Op, Args...> {
    using value_t = std::true_type;
    using type = Op<Args...>;
};

template <template<class...> class Op, class... Args>
using is_detected = typename detector<nonesuch, void, Op, Args...>::value_t;

template <template<class...> class Op, class... Args>
using detected_t = typename detector<nonesuch, void, Op, Args...>::type;

template< template<class...> class Op, class... Args >
constexpr bool is_detected_v = is_detected<Op, Args...>::value;

template <class Expected, template<class...> class Op, class... Args>
using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

template <class Expected, template<class...> class Op, class... Args>
constexpr bool is_detected_exact_v = is_detected_exact<Expected, Op, Args...>::value;

template <class To, template<class...> class Op, class... Args>
using is_detected_convertible = std::is_convertible<detected_t<Op, Args...>, To>;

template <class To, template<class...> class Op, class... Args>
constexpr bool is_detected_convertible_v = is_detected_convertible<To, Op, Args...>::value;


// is_mutable_buffer_sequence implementation

template <class T>
using buffer_sequence_iter_value_t = decltype(*net::buffer_sequence_begin(std::declval<T>()));

// FIXME: Need to test whether the return of net::buffer_sequence{begin|end} is a bidir iter.

template <class T>
using is_mutable_buffer_sequence_impl = is_detected_convertible<net::mutable_buffer, buffer_sequence_iter_value_t, T>;

// is_const_buffer_sequence implementation

template <class T>
//using is_const_buffer_sequence_impl = is_detected_convertible<net::const_buffer, buffer_sequence_iter_value_t, const T>;
using is_const_buffer_sequence_impl = std::true_type;

// is_dynamic_buffer implementation

template <class T>
using const_buffers_t = typename T::const_buffers_type;

template <class T>
using mutable_buffers_t = typename T::mutable_buffers_type;

template <class T>
using has_size_t = std::result_of_t<decltype(&T::size)(T)>;

template <class T>
using has_max_size_t = decltype(std::declval<T>().max_size());

template <class T>
using has_data_t = decltype(std::declval<T>().data());

template <class T>
using has_prepare_t = std::result_of_t<decltype(&T::prepare)(T, std::size_t)>;

template <class T>
using has_commit_t = std::result_of_t<decltype(&T::commit)(T, std::size_t)>;

template <class T>
using has_consume_t = std::result_of_t<decltype(&T::consume)(T, std::size_t)>;

template <class T>
using is_dynamic_buffer_impl
    = std::integral_constant<bool,
        is_detected_v<const_buffers_t, T> &&
                is_detected_v<mutable_buffers_t, T> &&
                is_detected_exact_v<std::size_t, has_size_t, T> &&
                is_detected_exact_v<std::size_t, has_max_size_t, T> &&
                is_detected_v<has_data_t, T> && // FIXME: Check returns T::const_buffers_type
                is_detected_v<has_prepare_t, T> && // FIXME: Check returns T::mutable_buffers_type
                is_detected_v<has_commit_t, T> &&
                is_detected_v<has_consume_t, T>>;

}

template <class T>
struct is_mutable_buffer_sequence : detail::is_mutable_buffer_sequence_impl<T> {};

template <class T>
struct is_const_buffer_sequence : detail::is_const_buffer_sequence_impl<T> {};

template <class T>
struct is_dynamic_buffer : detail::is_dynamic_buffer_impl<T> {};

// 16.7 Buffer sequence access [buffer.seq.access]

inline const mutable_buffer* buffer_sequence_begin(const mutable_buffer& b)
{
    return std::addressof(b);
}

inline const const_buffer* buffer_sequence_begin(const const_buffer& b)
{
    return std::addressof(b);
}

inline const mutable_buffer* buffer_sequence_end(const mutable_buffer& b)
{
    return std::addressof(b) + 1;
}

inline const const_buffer* buffer_sequence_end(const const_buffer& b)
{
    return std::addressof(b) + 1;
}

template<class C>
auto buffer_sequence_begin(C& c) -> decltype(c.begin())
{
    return c.begin();
}

template<class C>
auto buffer_sequence_begin(const C& c) -> decltype(c.begin())
{
    return c.begin();
}

template<class C>
auto buffer_sequence_end(C& c) -> decltype(c.end())
{
    return c.end();
}

template<class C> auto buffer_sequence_end(const C& c) -> decltype(c.end())
{
    return c.end();
}

static_assert(is_mutable_buffer_sequence<mutable_buffer>::value,
              "net::mutable_buffer does not meet the MutableBufferSequence requirements!");

static_assert(is_const_buffer_sequence<const_buffer>::value,
              "net::const_buffer does not meet the ConstBufferSequence requirements!");

// 16.8 Function buffer_size [buffer.size]

template<class ConstBufferSequence>
size_t buffer_size(const ConstBufferSequence& buffers) noexcept
{
    std::size_t total_size = 0;
    auto i = net::buffer_sequence_begin(buffers);
    auto end = net::buffer_sequence_end(buffers);

    for (; i != end; ++i) {
        const_buffer b(*i);
        total_size += b.size();
    }
    return total_size;
}

// Extension: these two functions are not in the TS
inline size_t buffer_size(const mutable_buffer& b) noexcept
{
    return b.size();
}

inline size_t buffer_size(const const_buffer& b) noexcept
{
    return b.size();
}

// 16.9 Function buffer_copy [buffer.copy]

template<class MutableBufferSequence, class ConstBufferSequence>
inline size_t buffer_copy(const MutableBufferSequence& dest,
                   const ConstBufferSequence& source) noexcept
{
    throw std::runtime_error("function net::buffer_copy() is not yet implemented");
};

// 16.10 Buffer arithmetic [buffer.arithmetic]

inline mutable_buffer operator+(const mutable_buffer& b, size_t n) noexcept
{
    return mutable_buffer{static_cast<char*>(b.data()) + std::min(n, b.size()),
                          b.size() - std::min(n, b.size())};
}

inline mutable_buffer operator+(size_t n, const mutable_buffer& b) noexcept
{
    return b + n;
}

inline const_buffer operator+(const const_buffer& b, size_t n) noexcept
{
    return const_buffer{
            static_cast<const char*>(b.data()) + std::min(n, b.size()),
            b.size() - std::min(n, b.size())};
}

inline const_buffer operator+(size_t n, const const_buffer& b) noexcept
{
    return b + n;
}

// 16.11 Buffer creation functions [buffer.creation]

inline mutable_buffer buffer(void* p, size_t n) noexcept
{
    return mutable_buffer{p, n};
}

inline const_buffer buffer(const void* p, size_t n) noexcept
{
    return const_buffer{p, n};
}

inline mutable_buffer buffer(const mutable_buffer& b) noexcept
{
    return b;
}

inline mutable_buffer buffer(const mutable_buffer& b, size_t n) noexcept
{
    return mutable_buffer{b.data(), std::min(b.size(), n)};
}

inline const_buffer buffer(const const_buffer& b) noexcept
{
    return b;
}

inline const_buffer buffer(const const_buffer& b, size_t n) noexcept
{
    return const_buffer{b.data(), std::min(b.size(), n)};
}

template<class T, std::size_t N>
mutable_buffer buffer(T (&data)[N]) noexcept
{
    return buffer(std::begin(data) != std::end(data) ? std::addressof(*std::begin(data)) : nullptr,
                  (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
};

template<class T, std::size_t N>
const_buffer buffer(const T (&data)[N]) noexcept
{
    return buffer(std::begin(data) != std::end(data) ? std::addressof(*std::begin(data)) : nullptr,
                  (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
};

template<class T, std::size_t N>
mutable_buffer buffer(std::array<T, N>& data) noexcept
{
    return buffer(std::begin(data) != std::end(data) ? std::addressof(*std::begin(data)) : nullptr,
                  (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
};

template<class T, std::size_t N>
const_buffer buffer(std::array<const T, N>& data) noexcept
{
    return buffer(std::begin(data) != std::end(data) ? std::addressof(*std::begin(data)) : nullptr,
                  (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
};

template<class T, std::size_t N>
const_buffer buffer(const std::array<T, N>& data) noexcept
{
    return buffer(std::begin(data) != std::end(data) ? std::addressof(*std::begin(data)) : nullptr,
                  (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
};

template<class T, class Allocator>
mutable_buffer buffer(std::vector<T, Allocator>& data) noexcept
{
    return buffer(std::begin(data) != std::end(data) ? std::addressof(*std::begin(data)) : nullptr,
                  (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
};

template<class T, class Allocator>
const_buffer buffer(const std::vector<T, Allocator>& data) noexcept
{
    return buffer(std::begin(data) != std::end(data) ? std::addressof(*std::begin(data)) : nullptr,
                  (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
};

template<class CharT, class Traits, class Allocator>
mutable_buffer buffer(std::basic_string<CharT, Traits, Allocator>& data) noexcept
{
    return buffer(std::begin(data) != std::end(data) ? std::addressof(*std::begin(data)) : nullptr,
                  (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
};

template<class CharT, class Traits, class Allocator>
const_buffer buffer(const std::basic_string<CharT, Traits, Allocator>& data) noexcept
{
    return buffer(std::begin(data) != std::end(data) ? std::addressof(*std::begin(data)) : nullptr,
                  (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
};

#ifdef IO_HAVE_STRING_VIEW

template<class CharT, class Traits>
const_buffer buffer(io_std::basic_string_view<CharT, Traits> data) noexcept
{
    return buffer(std::begin(data) != std::end(data) ? std::addressof(*std::begin(data)) : nullptr,
                  (std::end(data) - std::begin(data)) * sizeof(*std::begin(data)));
};

#endif // IO_HAVE_STRING_VIEW

template<class T, std::size_t N>
mutable_buffer buffer(T (&data)[N], std::size_t n) noexcept
{
    return buffer(buffer(data), n);
};

template<class T, std::size_t N>
const_buffer buffer(const T (&data)[N], std::size_t n) noexcept
{
    return buffer(buffer(data), n);
};

template<class T, std::size_t N>
mutable_buffer buffer(std::array<T, N>& data, std::size_t n) noexcept
{
    return buffer(buffer(data), n);
};

template<class T, std::size_t N>
const_buffer buffer(std::array<const T, N>& data, std::size_t n) noexcept
{
    return buffer(buffer(data), n);
};

template<class T, std::size_t N>
const_buffer buffer(const std::array<T, N>& data, std::size_t n) noexcept
{
    return buffer(buffer(data), n);
};

template<class T, class Allocator>
mutable_buffer buffer(std::vector<T, Allocator>& data, std::size_t n) noexcept
{
    return buffer(buffer(data), n);
};

template<class T, class Allocator>
const_buffer buffer(const std::vector<T, Allocator>& data, std::size_t n) noexcept
{
    return buffer(buffer(data), n);
};

template<class CharT, class Traits, class Allocator>
mutable_buffer buffer(std::basic_string<CharT, Traits, Allocator>& data,
                      std::size_t n) noexcept
{
    return buffer(buffer(data), n);
};

template<class CharT, class Traits, class Allocator>
const_buffer buffer(const std::basic_string<CharT, Traits, Allocator>& data,
                    std::size_t n) noexcept
{
    return buffer(buffer(data), n);
};

#ifdef IO_HAVE_STRING_VIEW

template<class CharT, class Traits>
const_buffer buffer(io_std::basic_string_view<CharT, Traits> data,
                    size_t n) noexcept
{
    return buffer(buffer(data), n);
};

#endif // IO_HAVE_STRING_VIEW

// 16.12 Class template dynamic_vector_buffer [buffer.dynamic.vector]

template<class T, class Allocator>
class dynamic_vector_buffer
{
public:
    static_assert(std::is_trivially_copyable<T>::value ||
                  std::is_standard_layout<T>::value,
    "dynamic_vector_buffer<T> requires T to be trivially copyable or standard layout"
    );
    static_assert(sizeof(T) == 1, "dynamic_vector_buffer<T> requires that sizeof(T) == 1");

    using const_buffers_type = const_buffer;
    using mutable_buffers_type = mutable_buffer;

    explicit dynamic_vector_buffer(std::vector<T, Allocator>& vec) noexcept
        : vec_(vec), size_{vec.size()}, max_size_{vec.max_size()}
    {}

    dynamic_vector_buffer(std::vector<T, Allocator>& vec,
                          size_t maximum_size) noexcept
        : vec_(vec), size_{vec.size()}, max_size_{maximum_size}
    {
        assert(vec.size() <= maximum_size);
    }

    dynamic_vector_buffer(dynamic_vector_buffer&&) = default;

    std::size_t size() const noexcept { return size_; }

    std::size_t max_size() const noexcept { return max_size_; }

    std::size_t capacity() const noexcept { return vec_.capacity(); }

    const_buffers_type data() const noexcept { return buffer(vec_, size_); }

    mutable_buffers_type prepare(std::size_t n)
    {
        if (size_ + n > max_size_) {
            throw std::length_error{"dynamic_vector_buffer::prepare(): size() + n > max_size()"};
        }

        vec_.resize(size_ + n);
        return buffer(buffer(vec_) + size_, n);
    }

    void commit(std::size_t n)
    {
        size_ += std::min(n, vec_.size() - size_);
        vec_.resize(size_);
    }

    void consume(std::size_t n)
    {
        std::size_t m = std::min(n, size_);
        vec_.erase(vec_.begin(), vec_.begin() + m);
        size_ -= m;
    }

private:
    std::vector<T, Allocator>& vec_;
    std::size_t size_;
    const std::size_t max_size_;
};

static_assert(is_dynamic_buffer<dynamic_vector_buffer<char, std::allocator<char>>>::value,
              "net::dynamic_vector_buffer does not meet the DynamicBuffer requirements!");

// 16.13 Class template dynamic_string_buffer [buffer.dynamic.string]

template<class CharT, class Traits, class Allocator>
class dynamic_string_buffer
{
public:
    static_assert(sizeof(CharT) == 1,
                  "dynamic_string_buffer requires that sizeof(CharT) == 1");

    using const_buffers_type = const_buffer;
    using mutable_buffers_type = mutable_buffer;

    explicit dynamic_string_buffer(std::basic_string<CharT, Traits, Allocator>& str) noexcept
        : str_(str), size_(str.size()), max_size_(str.max_size())
    {}

    dynamic_string_buffer(std::basic_string<CharT, Traits, Allocator>& str,
                          size_t maximum_size) noexcept
        : str_(str), size_(str.size()), max_size_(maximum_size)
    {
        assert(str.size() < maximum_size);
    }

    dynamic_string_buffer(dynamic_string_buffer&&) = default;

    std::size_t size() const noexcept { return size_; }

    std::size_t max_size() const noexcept { return max_size_; }

    std::size_t capacity() const noexcept { return str_.capacity(); }

    const_buffers_type data() const noexcept { return buffer(str_, size_); }

    mutable_buffers_type prepare(std::size_t n)
    {
        if (size() + n > max_size()) {
            throw std::length_error{"dynamic_vector_buffer::prepare(): size() + n > max_size()"};
        }
        str_.resize(size_ + n);
        return buffer(buffer(str_) + size_, n);
    }

    void commit(std::size_t n) noexcept
    {
        size_ += std::min(n, str_.size() - size_);
        str_.resize(size_);
    }

    void consume(std::size_t n)
    {
        std::size_t m = std::min(n, size_);
        str_.erase(m);
        size_ -= m;
    }

private:
    std::basic_string<CharT, Traits, Allocator>& str_;
    std::size_t size_;
    const std::size_t max_size_;
};

static_assert(is_dynamic_buffer<dynamic_string_buffer<char, std::char_traits<char>, std::allocator<char>>>::value,
              "net::dynamic_string_buffer does not meet the DynamicBuffer requirements!");

// 16.14 Dynamic buffer creation functions [buffer.dynamic.creation]

template<class T, class Allocator>
dynamic_vector_buffer<T, Allocator>
dynamic_buffer(std::vector<T, Allocator>& vec) noexcept
{
    return dynamic_vector_buffer<T, Allocator>{vec};
}

template<class T, class Allocator>
dynamic_vector_buffer<T, Allocator>
dynamic_buffer(std::vector<T, Allocator>& vec, std::size_t n) noexcept
{
    return dynamic_vector_buffer<T, Allocator>{vec, n};
}

template<class CharT, class Traits, class Allocator>
dynamic_string_buffer<CharT, Traits, Allocator>
dynamic_buffer(std::basic_string<CharT, Traits, Allocator>& str) noexcept
{
    return dynamic_string_buffer<CharT, Traits, Allocator>{str};
}

template<class CharT, class Traits, class Allocator>
dynamic_string_buffer<CharT, Traits, Allocator>
dynamic_buffer(std::basic_string<CharT, Traits, Allocator>& str, std::size_t n) noexcept
{
    return dynamic_string_buffer<CharT, Traits, Allocator>{str, n};
}

//
// 17 Buffer-oriented streams [buffer.stream]
//

// Value taken from Asio
constexpr std::size_t max_single_transfer_size = 65536;

// 17.2 Class transfer_all [buffer.stream.transfer.all]

class transfer_all
{
public:
    std::size_t operator()(const std::error_code& ec, std::size_t) const
    {
        return ec ? 0 : max_single_transfer_size;
    }
};

// 17.3 Class transfer_at_least [buffer.stream.transfer.at.least]


class transfer_at_least
{
public:
    explicit transfer_at_least(std::size_t m)
        : minimum_{m}
    {}

    std::size_t operator()(const std::error_code& ec, std::size_t s) const
    {
        if (!ec && s < minimum_) {
            return max_single_transfer_size;
        }
        return 0;
    }

private:
    std::size_t minimum_;
};

// 17.4 Class transfer_exactly [buffer.stream.transfer.exactly

class transfer_exactly
{
public:
    explicit transfer_exactly(std::size_t e)
        : exact_{e}
    {}

    std::size_t operator()(const std::error_code& ec, std::size_t s) const
    {
        if (!ec && s < exact_) {
            return std::min(exact_ - s, max_single_transfer_size);
        }
        return 0;
    }

private:
    std::size_t exact_;
};

// 17.5 Synchronous read operations [buffer.read]

template<class SyncReadStream, class MutableBufferSequence>
std::size_t read(SyncReadStream& stream,
                 const MutableBufferSequence& buffers)
{
    return read(stream, buffers, transfer_all{});
}

template<class SyncReadStream, class MutableBufferSequence>
std::size_t read(SyncReadStream& stream,
                 const MutableBufferSequence& buffers,
                 std::error_code& ec)
{
    return read(stream, buffers, transfer_all{}, ec);
}

template<class SyncReadStream, class MutableBufferSequence,
        class CompletionCondition>
std::size_t read(SyncReadStream& stream,
                 const MutableBufferSequence& buffers,
                 CompletionCondition completion_condition)
{
    std::error_code ec;
    std::size_t bytes_read = read(stream, buffers, completion_condition, ec);
    if (ec) throw std::system_error(ec, __func__);
    return bytes_read;
}

template<class SyncReadStream, class MutableBufferSequence,
        class CompletionCondition>
std::size_t read(SyncReadStream& stream,
                 const MutableBufferSequence& buffers,
                 CompletionCondition completion_condition,
                 std::error_code& ec)
{
    ec.clear();

    std::size_t total_bytes_read = 0;
    std::size_t next_read_size = max_single_transfer_size;
    std::size_t buf_size = buffer_size(buffers);

    while (total_bytes_read < buf_size && next_read_size != 0) {
        total_bytes_read += stream.read_some(buffers, ec);
        next_read_size = completion_condition(ec, total_bytes_read);
    }

    return total_bytes_read;
}

template<class SyncReadStream, class DynamicBuffer, class>
std::size_t read(SyncReadStream& stream, DynamicBuffer&& b)
{
    return read(stream, std::forward<DynamicBuffer>(b), transfer_all{});
}

template<class SyncReadStream, class DynamicBuffer, class>
std::size_t read(SyncReadStream& stream, DynamicBuffer&& b, std::error_code& ec)
{
    return read(stream, std::forward<DynamicBuffer>(b), transfer_all{}, ec);
}

template<class SyncReadStream, class DynamicBuffer,
        class CompletionCondition, class>
std::size_t read(SyncReadStream& stream, DynamicBuffer&& b,
                 CompletionCondition completion_condition)
{
    std::error_code ec;
    std::size_t bytes_read = read(stream, std::forward<DynamicBuffer>(b),
                                  completion_condition, ec);
    if (ec) throw std::system_error(ec, __func__);
    return bytes_read;
}

template<class SyncReadStream, class DynamicBuffer,
        class CompletionCondition, class>
std::size_t read(SyncReadStream& stream, DynamicBuffer&& b,
                 CompletionCondition completion_condition,
                 std::error_code& ec)
{
    ec.clear();

    std::size_t total_bytes_read = 0;
    std::size_t next_read_size = b.capacity();

    while (b.size() < b.max_size()) {
        auto buf = b.prepare(next_read_size);
        std::size_t bytes_read = stream.read_some(buf, ec);
        total_bytes_read += bytes_read;
        next_read_size = completion_condition(ec, total_bytes_read);
        b.commit(bytes_read);
        if (next_read_size == 0) {
            break;
        }
    }

    return total_bytes_read;
}

// 17.7 Synchronous write operations [buffer.write]

template<class SyncWriteStream, class ConstBufferSequence, class>
std::size_t write(SyncWriteStream& stream,
                  const ConstBufferSequence& buffers)
{
    return write(stream, buffers, transfer_all{});
}

template<class SyncWriteStream, class ConstBufferSequence, class>
std::size_t write(SyncWriteStream& stream,
                  const ConstBufferSequence& buffers,
                  std::error_code& ec)
{
    return write(stream, buffers, transfer_all{}, ec);
}

template<class SyncWriteStream, class ConstBufferSequence,
        class CompletionCondition, class>
std::size_t write(SyncWriteStream& stream,
             const ConstBufferSequence& buffers,
             CompletionCondition completion_condition)
{
    std::error_code ec;
    std::size_t bytes_written = write(stream, buffers,
                                      completion_condition, ec);
    if (ec) throw std::system_error(ec, __func__);
    return bytes_written;
}

template<class SyncWriteStream, class ConstBufferSequence,
        class CompletionCondition, class>
std::size_t write(SyncWriteStream& stream,
                  const ConstBufferSequence& buffers,
                  CompletionCondition completion_condition,
                  std::error_code& ec)
{
    ec.clear();

    std::size_t total_bytes_written = 0;
    std::size_t next_write_size = max_single_transfer_size;
    std::size_t buf_size = buffer_size(buffers);

    while (total_bytes_written < buf_size && next_write_size != 0) {
        total_bytes_written += stream.write_some(buffers, ec);
        next_write_size = completion_condition(ec, total_bytes_written);
    }

    return total_bytes_written;
}

template <class SyncWriteStream, class DynamicBuffer, class>
std::size_t write(SyncWriteStream& stream, DynamicBuffer&& b)
{
    return write(stream, std::forward<DynamicBuffer>(b), transfer_all{});
}

template <class SyncWriteStream, class DynamicBuffer, class>
std::size_t write(SyncWriteStream& stream, DynamicBuffer&& b, std::error_code& ec)
{
    return write(stream, std::forward<DynamicBuffer>(b), transfer_all{}, ec);
}

template <class SyncWriteStream, class DynamicBuffer,
        class CompletionCondition, class>
std::size_t write(SyncWriteStream& stream, DynamicBuffer&& b,
                  CompletionCondition completion_condition)
{
    std::error_code ec;
    std::size_t bytes_written = write(stream, std::forward<DynamicBuffer>(b),
                                      completion_condition, ec);
    if (ec) throw std::system_error(ec, __func__);
    return bytes_written;
}

template <class SyncWriteStream, class DynamicBuffer,
        class CompletionCondition, class>
std::size_t write(SyncWriteStream& stream, DynamicBuffer&& b,
                  CompletionCondition completion_condition,
                  std::error_code& ec)
{
    ec.clear();

    std::size_t total_bytes_written = 0;
    std::size_t next_write_size = b.capacity();

    while (b.size() != 0 && next_write_size != 0) {
        std::size_t bytes_written = stream.write_some(b.data(), ec);
        total_bytes_written += bytes_written;
        next_write_size = completion_condition(ec, total_bytes_written);
        b.consume(bytes_written);
    }

    return total_bytes_written;
}


} // end namespace net
} // end namespace io

#endif // IO_BUFFER_IMPL_HPP
