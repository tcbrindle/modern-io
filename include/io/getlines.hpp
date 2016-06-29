
// Copyright (c) 2016 Tristan Brindle (tcbrindle at gmail dot com)
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef IO_GETLINES_HPP
#define IO_GETLINES_HPP

#include <range/v3/view/split.hpp>

namespace io {

template <typename InputIt, typename Sentinel, typename Delims,
          CONCEPT_REQUIRES_(rng::SinglePass<InputIt>())>
bool getline(InputIt first, Sentinel last, std::string& str, Delims delim)
{
    str.erase();

    while (first != last) {
        auto c = *first;
        ++first;
        //if (rng::find(std::forward<Delims>(delims), c) != rng::end(delims)) {
        if (c == delim) {
            return true;
        }
        str += c;
    }

    return false;
};

constexpr char default_delimiter[] = "\n";

template <typename Rng,
          CONCEPT_REQUIRES_(rng::InputRange<Rng>() && !rng::ForwardRange<Rng>())>
bool getline(Rng& range, std::basic_string<rng::range_value_t<Rng>>& str,
             rng::range_value_t<Rng> delimiter = '\n')
{
    return getline(rng::begin(range),
                   rng::end(range),
                   str, delimiter);
}

template <typename Rng,
          CONCEPT_REQUIRES_(rng::InputRange<Rng>() && !rng::ForwardRange<Rng>())>
class getlines_view : public rng::view_facade<getlines_view<Rng>, rng::unknown>
{
public:
    getlines_view() = default;

    getlines_view(Rng& range, char delim = '\n')
        : range_(std::addressof(range)),
          delim_(delim),
          done_{false}
    {
        this->next();
    }

private:
    friend rng::range_access;

    struct cursor {
        cursor() = default;

        explicit cursor(getlines_view& gv) : gv_{&gv} {}

        const std::string& get() const
        {
            return gv_->str_;
        }

        void next()
        {
            gv_->next();
        }

        bool done() const
        {
            return gv_->done_;
        }

        getlines_view* gv_ = nullptr;
    };

    cursor begin_cursor() { return cursor{*this}; }

    void next()
    {
        done_ = !getline(*range_, str_, delim_);
    }

    Rng* range_ = nullptr;
    std::string str_;
    char delim_ = '\n';
    bool done_ = true;
};

template <typename Rng,
          CONCEPT_REQUIRES_(rng::InputRange<Rng>() && !rng::ForwardRange<Rng>())>
getlines_view<Rng> getlines(Rng& range, char delim = '\n')
{
    return getlines_view<Rng>(range, delim);
}

template <typename Rng,
          CONCEPT_REQUIRES_(rng::ForwardRange<Rng>())>
auto getlines(Rng&& range, char delim = '\n')
    -> decltype(std::forward<Rng>(range) | rng::view::split(delim))
{
    return std::forward<Rng>(range) | rng::view::split(delim);
}

};

#endif // IO_GETLINES_HPP
