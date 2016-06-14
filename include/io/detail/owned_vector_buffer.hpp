
#ifndef IO_DETAIL_OWNED_VECTOR_BUFFER_HPP
#define IO_DETAIL_OWNED_VECTOR_BUFFER_HPP

#include <io/buffer.hpp>

#include <vector>

namespace io {
namespace detail {

// This class replicates net::dynamic_vector_buffer, except that it owns the
// contained vector
template<class T, class Allocator>
class owned_vector_buffer
{
public:
    static_assert(std::is_trivially_copyable<T>::value ||
                          std::is_standard_layout<T>::value,
                  "owned_vector_buffer<T> requires T to be trivially copyable or standard layout"
    );
    static_assert(sizeof(T) == 1, "owned_vector_buffer<T> requires that sizeof(T) == 1");

    using const_buffers_type = const_buffer;
    using mutable_buffers_type = mutable_buffer;

    explicit owned_vector_buffer(std::size_t maximum_size) noexcept
            : max_size_{maximum_size}
    {}

    owned_vector_buffer(const Allocator& allocator,
                        size_t maximum_size) noexcept
            : vec_(allocator), max_size_{maximum_size}
    {}

    owned_vector_buffer(owned_vector_buffer&&) = default;

    Allocator get_allocator() const { return vec_.get_allocator(); }

    std::size_t size() const noexcept { return size_ - start_; }

    std::size_t max_size() const noexcept { return max_size_ - start_; }

    std::size_t capacity() const noexcept { return vec_.capacity(); }

    const_buffers_type data() const noexcept { return buffer(vec_, size_) + start_; }

    void clear() { start_ = 0; size_ = 0; }

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
        start_ += m;
    }

private:
    std::vector<T, Allocator> vec_;
    std::size_t start_ = 0;
    std::size_t size_ = 0;
    const std::size_t max_size_;
};


} // end namespace detail
} // end namespace io

#endif // IO_DETAIL_OWNED_VECTOR_BUFFER_HPP
