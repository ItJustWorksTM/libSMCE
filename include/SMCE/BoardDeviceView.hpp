/*
 *  BoardDeviceView.hpp
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

#ifndef LIBSMCE_BOARDDEVICEVIEW_HPP
#define LIBSMCE_BOARDDEVICEVIEW_HPP

#include <cstddef>
#include <iterator>
#include <optional>
#include <string_view>
#include <type_traits>
#include <SMCE/BoardDeviceFieldType.hpp>
#include <SMCE/SMCE_iface.h>
#include <SMCE/fwd.hpp>
#include <SMCE_rt/SMCE_proxies.hpp>

namespace smce {

class SMCE_API VirtualDeviceField {
    BoardData* m_bdat;
    std::size_t m_base;
    BoardDeviceFieldType m_type = BoardDeviceFieldType::void_;

    VirtualDeviceField(BoardData* bdat, std::size_t map_index, std::size_t base_index, std::string_view name) noexcept;

  public:
    friend class VirtualDevice;

    [[nodiscard]] bool exists() noexcept;
    [[nodiscard]] BoardDeviceFieldType type() noexcept { return m_type; }

    [[nodiscard]] auto as_u8() -> std::uint8_t&;
    [[nodiscard]] auto as_u16() -> std::uint16_t&;
    [[nodiscard]] auto as_u32() -> std::uint32_t&;
    [[nodiscard]] auto as_u64() -> std::uint64_t&;
    [[nodiscard]] auto as_s8() -> std::int8_t&;
    [[nodiscard]] auto as_s16() -> std::int16_t&;
    [[nodiscard]] auto as_s32() -> std::int32_t&;
    [[nodiscard]] auto as_s64() -> std::int64_t&;
    [[nodiscard]] auto as_f32() -> float&;
    [[nodiscard]] auto as_f64() -> double&;

    [[nodiscard]] auto as_au8() -> smce_rt::AtomicU8;
    [[nodiscard]] auto as_au16() -> smce_rt::AtomicU16;
    [[nodiscard]] auto as_au32() -> smce_rt::AtomicU32;
    [[nodiscard]] auto as_au64() -> smce_rt::AtomicU64;
    [[nodiscard]] auto as_as8() -> smce_rt::AtomicS8;
    [[nodiscard]] auto as_as16() -> smce_rt::AtomicS16;
    [[nodiscard]] auto as_as32() -> smce_rt::AtomicS32;
    [[nodiscard]] auto as_as64() -> smce_rt::AtomicS64;
    [[nodiscard]] auto as_af32() -> smce_rt::AtomicF32;
    [[nodiscard]] auto as_af64() -> smce_rt::AtomicF64;
    [[nodiscard]] auto as_mutex() -> smce_rt::Mutex;

    [[nodiscard]] auto as_opt_u8() -> std::uint8_t*;
    [[nodiscard]] auto as_opt_u16() -> std::uint16_t*;
    [[nodiscard]] auto as_opt_u32() -> std::uint32_t*;
    [[nodiscard]] auto as_opt_u64() -> std::uint64_t*;
    [[nodiscard]] auto as_opt_s8() -> std::int8_t*;
    [[nodiscard]] auto as_opt_s16() -> std::int16_t*;
    [[nodiscard]] auto as_opt_s32() -> std::int32_t*;
    [[nodiscard]] auto as_opt_s64() -> std::int64_t*;
    [[nodiscard]] auto as_opt_f32() -> float*;
    [[nodiscard]] auto as_opt_f64() -> double*;

    [[nodiscard]] auto as_opt_au8() -> std::optional<smce_rt::AtomicU8>;
    [[nodiscard]] auto as_opt_au16() -> std::optional<smce_rt::AtomicU16>;
    [[nodiscard]] auto as_opt_au32() -> std::optional<smce_rt::AtomicU32>;
    [[nodiscard]] auto as_opt_au64() -> std::optional<smce_rt::AtomicU64>;
    [[nodiscard]] auto as_opt_as8() -> std::optional<smce_rt::AtomicS8>;
    [[nodiscard]] auto as_opt_as16() -> std::optional<smce_rt::AtomicS16>;
    [[nodiscard]] auto as_opt_as32() -> std::optional<smce_rt::AtomicS32>;
    [[nodiscard]] auto as_opt_as64() -> std::optional<smce_rt::AtomicS64>;
    [[nodiscard]] auto as_opt_af32() -> std::optional<smce_rt::AtomicF32>;
    [[nodiscard]] auto as_opt_af64() -> std::optional<smce_rt::AtomicF64>;
    [[nodiscard]] auto as_opt_mutex() -> std::optional<smce_rt::Mutex>;
};

class SMCE_API VirtualDevice {
    BoardData* m_bdat{};
    std::size_t m_map_index;
    std::size_t m_base_index;

    VirtualDevice(BoardData* bdat, std::size_t map_index, std::size_t base_index) noexcept
        : m_bdat{bdat}, m_map_index{map_index}, m_base_index{base_index} {}

  public:
    friend class VirtualDevices;

    [[nodiscard]] VirtualDeviceField operator[](std::string_view field_name) noexcept {
        return {m_bdat, m_map_index, m_base_index, field_name};
    }
};

class SMCE_API VirtualDevices {
    BoardData* m_bdat{};
    std::size_t m_map_index;

    VirtualDevices(BoardData* bdat, std::string_view name) noexcept;

  public:
    friend class BoardDeviceView;

    class SMCE_API Iterator {
        BoardData* m_bdat{};
        std::size_t m_map_index;
        std::size_t m_base_index;

        Iterator(BoardData* bdat, std::size_t map_index, std::size_t base_index) noexcept
            : m_bdat{bdat}, m_map_index{map_index}, m_base_index{base_index} {}

      public:
        friend VirtualDevices;

        using iterator_category = std::random_access_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<std::string_view, VirtualDevice>;
        using pointer = void;
        using reference = void;

        [[nodiscard]] value_type operator*() const noexcept;
        Iterator& operator++() noexcept;
        Iterator operator++(int) noexcept;
        Iterator& operator+=(std::ptrdiff_t offset) noexcept;
        Iterator& operator-=(std::ptrdiff_t offset) noexcept;
        Iterator operator+(std::ptrdiff_t offset) const noexcept;
        Iterator operator-(std::ptrdiff_t offset) const noexcept;
    };
    using iterator = Iterator;

    [[nodiscard]] bool exists() noexcept { return m_bdat; }
    [[nodiscard]] VirtualDevice operator[](std::size_t index) noexcept { return {m_bdat, m_map_index, index}; }
    [[nodiscard]] Iterator begin() noexcept { return Iterator{m_bdat, m_map_index, 0}; }
    [[nodiscard]] Iterator end() noexcept { return Iterator{m_bdat, m_map_index, size()}; }
    [[nodiscard]] std::size_t size() noexcept;
};

/// \internal
class SMCE_API BoardDeviceView {
    BoardData* m_bdat{};

  public:
    explicit BoardDeviceView(BoardView& bv) noexcept;

    [[nodiscard]] VirtualDevices operator[](std::string_view spec_name) noexcept {
        return VirtualDevices{m_bdat, spec_name};
    }

    /// Object validity check
    [[nodiscard]] bool valid() noexcept { return m_bdat; }
};

} // namespace smce

#endif // LIBSMCE_BOARDDEVICEVIEW_HPP
