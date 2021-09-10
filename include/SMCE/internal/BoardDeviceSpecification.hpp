/*
 *  internal/BoardDeviceView.hpp
 *  Copyright 2021 ItJustWorksTM
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

// USERS SHALL NOT INCLUDE THIS FILE

#ifndef LIBSMCE_INTERNAL_BOARDDEVICESPECIFICATION_HPP
#define LIBSMCE_INTERNAL_BOARDDEVICESPECIFICATION_HPP

#include <string_view>

namespace smce {

class BoardDeviceSpecification {
  public:
    std::string_view full_string;
    std::string_view name;
    std::size_t r8_count;
    std::size_t r16_count;
    std::size_t r32_count;
    std::size_t r64_count;
    std::size_t a8_count;
    std::size_t a16_count;
    std::size_t a32_count;
    std::size_t a64_count;
    std::size_t mtx_count;
};

} // namespace smce

#endif // LIBSMCE_INTERNAL_BOARDDEVICESPECIFICATION_HPP
