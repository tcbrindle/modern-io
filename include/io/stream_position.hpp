
// Copyright (c) 2017 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_STREAM_POSITION_HPP_INCLUDED
#define IO_STREAM_POSITION_HPP_INCLUDED

#include <io/traits.hpp>

#include <cassert>
#include <type_traits>

namespace io {

template <typename OffsetType>
struct stream_position {
    static_assert(std::is_integral<OffsetType>::value &&
                  std::is_signed<OffsetType>::value,
                  "stream_position::offset_type must be a signed integral type");

    using offset_type = OffsetType;

    constexpr stream_position() = default;

    explicit constexpr stream_position(offset_type from_start) noexcept
        : pos_(from_start)
    {
        assert(from_start >= 0);
    }

    constexpr offset_type offset_from_start() const noexcept
    {
        return pos_;
    }

    constexpr stream_position& operator+=(offset_type offset) noexcept
    {
        pos_ += offset;
        return *this;
    }

    constexpr stream_position& operator-=(offset_type offset) noexcept
    {
        assert(pos_ >= offset);
        pos_ -= offset;
        return *this;
    }

private:
    offset_type pos_{};
};

// Arithmetic operations

template <typename OffsetType>
constexpr stream_position<OffsetType>
operator+(stream_position<OffsetType> pos, OffsetType off) noexcept
{
    return stream_position<OffsetType>{pos.offset_from_start() + off};
}

template <typename OffsetType>
constexpr stream_position<OffsetType>
operator-(stream_position<OffsetType> pos, OffsetType off) noexcept
{
    assert(pos.offset_from_start() >= off);
    return stream_position<OffsetType>{pos.offset_from_start() - off};
}

template <typename OffsetType>
constexpr OffsetType
operator-(stream_position<OffsetType> pos1, stream_position<OffsetType> pos2) noexcept
{
    return pos1.offset_from_start() - pos2.offset_from_start();
}

// Comparisons

template <typename OffT>
constexpr bool operator==(stream_position<OffT> lhs, stream_position<OffT> rhs) noexcept
{
    return lhs.offset_from_start() == rhs.offset_from_start();
}

template <typename OffT>
constexpr bool operator!=(stream_position<OffT> lhs, stream_position<OffT> rhs) noexcept
{
    return !(lhs == rhs);
}

template <typename OffT>
constexpr bool operator<(stream_position<OffT> lhs, stream_position<OffT> rhs) noexcept
{
    return lhs.offset_from_start() < rhs.offset_from_start();
}

template <typename OffT>
constexpr bool operator>(stream_position<OffT> lhs, stream_position<OffT> rhs) noexcept
{
    return rhs < lhs;
}

template <typename OffT>
constexpr bool operator<=(stream_position<OffT> lhs, stream_position<OffT> rhs) noexcept
{
    return !(lhs > rhs);
}

template <typename OffT>
constexpr bool operator>=(stream_position<OffT> lhs, stream_position<OffT> rhs) noexcept
{
    return !(lhs < rhs);
}


}

#endif
