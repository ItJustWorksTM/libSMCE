/*
 *  ostream_joiner.hpp
 *  Copyright 2021-2022 ItJustWorksTM
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef LIBSMCE_OSTREAMJOINER_HPP
#define LIBSMCE_OSTREAMJOINER_HPP

#if __has_include(<experimental/iterator>)
#    include <experimental/iterator>
#    if __cpp_lib_experimental_ostream_joiner >= 201411
#        define SMCE_PORTABLE_HAVE_EXPERIMENTAL_OSTREAM_JOINER
#    endif
#endif

#include <iterator>
#include <ostream>
#include <string>
#include <type_traits>

namespace smce::portable {

#ifdef SMCE_PORTABLE_HAVE_EXPERIMENTAL_OSTREAM_JOINER

using std::experimental::ostream_joiner;

using std::experimental::make_ostream_joiner;

#else

template <class DelimT, class CharT = char, class Traits = std::char_traits<CharT>>
class ostream_joiner {
  public:
    using char_type = CharT;
    using traits_type = Traits;
    using ostream_type = std::basic_ostream<CharT, Traits>;
    using iterator_category = std::output_iterator_tag;
    using value_type = void;
    using difference_type = void;
    using pointer = void;
    using reference = void;

  private:
    ostream_type* m_os;
    DelimT m_delim;
    bool m_is_first = true;

  public:
    ostream_joiner(ostream_type& os, const DelimT& delimiter) noexcept(std::is_nothrow_copy_constructible_v<DelimT>)
        : m_os(std::addressof(os)), m_delim(delimiter) {}

    ostream_joiner(ostream_type& os, DelimT&& delimiter) noexcept(std::is_nothrow_move_constructible_v<DelimT>)
        : m_os(std::addressof(os)), m_delim(std::move(delimiter)) {}

    template <typename T>
    ostream_joiner& operator=(const T& value) {
        if (!m_is_first) {
            *m_os << m_delim;
        }
        m_is_first = false;
        *m_os << value;
        return *this;
    }

    ostream_joiner& operator*() noexcept { return *this; }

    ostream_joiner& operator++() noexcept { return *this; }

    ostream_joiner& operator++(int) noexcept { return *this; }
};

template <typename CharT, typename Traits, typename DelimT>
inline ostream_joiner<std::remove_cvref_t<DelimT>, CharT, Traits>
make_ostream_joiner(std::basic_ostream<CharT, Traits>& os, DelimT&& delimiter) {
    return {os, std::forward<DelimT>(delimiter)};
}

#endif

} // namespace smce::portable

#endif // LIBSMCE_OSTREAMJOINER_HPP
