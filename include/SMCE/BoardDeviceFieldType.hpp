/*
 *  BoardDeviceSpecification.hpp
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

#ifndef LIBSMCE_BOARDDEVICEFIELDTYPE_HPP
#define LIBSMCE_BOARDDEVICEFIELDTYPE_HPP

#include <algorithm>
#include <cstdint>
#include <iterator>
#include <string_view>
#include <SMCE/SMCE_iface.h>
#include <SMCE/internal/portable/utility.hpp>

namespace smce {

// clang-format off
enum class BoardDeviceFieldType : std::uint8_t {
    u8, u16, u32, u64,
    s8, s16, s32, s64,
    f32, f64,
    au8, au16, au32, au64,
    as8, as16, as32, as64,
    af32, af64,
    mutex,
    void_ = 255
};
constexpr std::string_view board_device_field_type_names[]{
    "u8", "u16", "u32", "u64",
    "s8", "s16", "s32", "s64",
    "f32", "f64",
    "au8", "au16", "au32", "au64",
    "as8", "as16", "as32", "as64",
    "af32", "af64",
    "mutex"
};
// clang-format on

[[nodiscard]] constexpr std::string_view to_string(BoardDeviceFieldType bdfield_type) noexcept {
    if (portable::to_underlying(bdfield_type) >= std::size(board_device_field_type_names))
        return {};
    return board_device_field_type_names[portable::to_underlying(bdfield_type)];
}

[[nodiscard]] constexpr BoardDeviceFieldType to_board_device_field_type(std::string_view name) noexcept {
    const auto it = std::find(std::begin(board_device_field_type_names), std::end(board_device_field_type_names), name);
    if (it == std::end(board_device_field_type_names))
        return BoardDeviceFieldType::void_;
    return static_cast<BoardDeviceFieldType>(std::distance(std::begin(board_device_field_type_names), it));
}

} // namespace smce

#endif // LIBSMCE_BOARDDEVICEFIELDTYPE_HPP
