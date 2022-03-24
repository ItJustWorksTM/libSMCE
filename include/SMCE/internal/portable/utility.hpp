/*
 *  portable/utility.hpp
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

#ifndef LIBSMCE_UTILITY_HPP
#define LIBSMCE_UTILITY_HPP

#ifdef __cpp_lib_to_underlying
#    include <utility>
#else
#    include <type_traits>
#endif

namespace smce::portable {

#ifdef __cpp_lib_to_underlying

using std::to_underlying;

#else

template <class E>
constexpr std::underlying_type_t<E> to_underlying(E e) noexcept {
    return static_cast<std::underlying_type_t<E>>(e);
}

#endif

} // namespace smce::portable

#endif // LIBSMCE_UTILITY_HPP
