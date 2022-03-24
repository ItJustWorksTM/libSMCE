/*
 *  BoardDeviceView.hpp
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

#include <boost/hana/at_key.hpp>
#include <boost/hana/index_if.hpp>
#include "SMCE/BoardDeviceView.hpp"
#include "SMCE/BoardView.hpp"
#include "SMCE/internal/BoardData.hpp"
#include "SMCE/internal/portable/utility.hpp"

namespace smce_rt {
struct Impl {};
} // namespace smce_rt

namespace smce {

VirtualDeviceField::VirtualDeviceField(BoardData* bdat, std::size_t map_index, std::size_t base_index,
                                       std::string_view name) noexcept
    : m_bdat{bdat} {
    if (!m_bdat)
        return;
    auto& device = (m_bdat->device_map.begin() + map_index)->second;
    const auto it = device.fields.find({name.begin(), name.end()});
    if (it == device.fields.end())
        return;

    const auto type = it->second;
    const auto base_field_idx = device_field_type_to_bank_idx[portable::to_underlying(type)];
    const auto base_field = device.bases[base_field_idx];

    m_type = type;

    std::size_t front_elements = 0;
    for (auto& [_, dev_type] : device.fields) {
        if (device_field_type_to_bank_idx[portable::to_underlying(dev_type)] == base_field_idx)
            ++front_elements;
    }

    m_base = base_field + front_elements * base_index;
}

bool VirtualDeviceField::exists() noexcept { return m_bdat; }

template <class T>
static decltype(auto) field_as(BoardData* bdat, std::size_t base) {
    constexpr auto bank_idx = device_field_type_to_bank_idx[*boost::hana::index_if(
        device_field_types, boost::hana::equal.to(boost::hana::type_c<T>))];
    auto& underlying = bdat->banks[boost::hana::size_c<bank_idx>][base];
    if constexpr (std::is_trivial_v<T>)
        return reinterpret_cast<T&>(underlying);
    else
        return T{&underlying, smce_rt::Impl{}};
}

auto VirtualDeviceField::as_u8() -> std::uint8_t& { return field_as<std::uint8_t>(m_bdat, m_base); }
auto VirtualDeviceField::as_u16() -> std::uint16_t& { return field_as<std::uint16_t>(m_bdat, m_base); }
auto VirtualDeviceField::as_u32() -> std::uint32_t& { return field_as<std::uint32_t>(m_bdat, m_base); }
auto VirtualDeviceField::as_u64() -> std::uint64_t& { return field_as<std::uint64_t>(m_bdat, m_base); }
auto VirtualDeviceField::as_s8() -> std::int8_t& { return field_as<std::int8_t>(m_bdat, m_base); }
auto VirtualDeviceField::as_s16() -> std::int16_t& { return field_as<std::int16_t>(m_bdat, m_base); }
auto VirtualDeviceField::as_s32() -> std::int32_t& { return field_as<std::int32_t>(m_bdat, m_base); }
auto VirtualDeviceField::as_s64() -> std::int64_t& { return field_as<std::int64_t>(m_bdat, m_base); }
auto VirtualDeviceField::as_f32() -> float& { return field_as<float>(m_bdat, m_base); }
auto VirtualDeviceField::as_f64() -> double& { return field_as<double>(m_bdat, m_base); }

auto VirtualDeviceField::as_au8() -> smce_rt::AtomicU8 { return field_as<smce_rt::AtomicU8>(m_bdat, m_base); }
auto VirtualDeviceField::as_au16() -> smce_rt::AtomicU16 { return field_as<smce_rt::AtomicU16>(m_bdat, m_base); }
auto VirtualDeviceField::as_au32() -> smce_rt::AtomicU32 { return field_as<smce_rt::AtomicU32>(m_bdat, m_base); }
auto VirtualDeviceField::as_au64() -> smce_rt::AtomicU64 { return field_as<smce_rt::AtomicU64>(m_bdat, m_base); }
auto VirtualDeviceField::as_as8() -> smce_rt::AtomicS8 { return field_as<smce_rt::AtomicS8>(m_bdat, m_base); }
auto VirtualDeviceField::as_as16() -> smce_rt::AtomicS16 { return field_as<smce_rt::AtomicS16>(m_bdat, m_base); }
auto VirtualDeviceField::as_as32() -> smce_rt::AtomicS32 { return field_as<smce_rt::AtomicS32>(m_bdat, m_base); }
auto VirtualDeviceField::as_as64() -> smce_rt::AtomicS64 { return field_as<smce_rt::AtomicS64>(m_bdat, m_base); }
auto VirtualDeviceField::as_af32() -> smce_rt::AtomicF32 { return field_as<smce_rt::AtomicF32>(m_bdat, m_base); }
auto VirtualDeviceField::as_af64() -> smce_rt::AtomicF64 { return field_as<smce_rt::AtomicF64>(m_bdat, m_base); }
auto VirtualDeviceField::as_mutex() -> smce_rt::Mutex { return field_as<smce_rt::Mutex>(m_bdat, m_base); }

auto VirtualDeviceField::as_opt_u8() -> std::uint8_t* { return exists() ? &as_u8() : nullptr; }
auto VirtualDeviceField::as_opt_u16() -> std::uint16_t* { return exists() ? &as_u16() : nullptr; }
auto VirtualDeviceField::as_opt_u32() -> std::uint32_t* { return exists() ? &as_u32() : nullptr; }
auto VirtualDeviceField::as_opt_u64() -> std::uint64_t* { return exists() ? &as_u64() : nullptr; }
auto VirtualDeviceField::as_opt_s8() -> std::int8_t* { return exists() ? &as_s8() : nullptr; }
auto VirtualDeviceField::as_opt_s16() -> std::int16_t* { return exists() ? &as_s16() : nullptr; }
auto VirtualDeviceField::as_opt_s32() -> std::int32_t* { return exists() ? &as_s32() : nullptr; }
auto VirtualDeviceField::as_opt_s64() -> std::int64_t* { return exists() ? &as_s64() : nullptr; }
auto VirtualDeviceField::as_opt_f32() -> float* { return exists() ? &as_f32() : nullptr; }
auto VirtualDeviceField::as_opt_f64() -> double* { return exists() ? &as_f64() : nullptr; }

auto VirtualDeviceField::as_opt_au8() -> std::optional<smce_rt::AtomicU8> {
    return exists() ? std::optional{as_au8()} : std::nullopt;
}
auto VirtualDeviceField::as_opt_au16() -> std::optional<smce_rt::AtomicU16> {
    return exists() ? std::optional{as_au16()} : std::nullopt;
}
auto VirtualDeviceField::as_opt_au32() -> std::optional<smce_rt::AtomicU32> {
    return exists() ? std::optional{as_au32()} : std::nullopt;
}
auto VirtualDeviceField::as_opt_au64() -> std::optional<smce_rt::AtomicU64> {
    return exists() ? std::optional{as_au64()} : std::nullopt;
}
auto VirtualDeviceField::as_opt_as8() -> std::optional<smce_rt::AtomicS8> {
    return exists() ? std::optional{as_as8()} : std::nullopt;
}
auto VirtualDeviceField::as_opt_as16() -> std::optional<smce_rt::AtomicS16> {
    return exists() ? std::optional{as_as16()} : std::nullopt;
}
auto VirtualDeviceField::as_opt_as32() -> std::optional<smce_rt::AtomicS32> {
    return exists() ? std::optional{as_as32()} : std::nullopt;
}
auto VirtualDeviceField::as_opt_as64() -> std::optional<smce_rt::AtomicS64> {
    return exists() ? std::optional{as_as64()} : std::nullopt;
}
auto VirtualDeviceField::as_opt_af32() -> std::optional<smce_rt::AtomicF32> {
    return exists() ? std::optional{as_af32()} : std::nullopt;
}
auto VirtualDeviceField::as_opt_af64() -> std::optional<smce_rt::AtomicF64> {
    return exists() ? std::optional{as_af64()} : std::nullopt;
}
auto VirtualDeviceField::as_opt_mutex() -> std::optional<smce_rt::Mutex> {
    return exists() ? std::optional{as_mutex()} : std::nullopt;
}

VirtualDevices::VirtualDevices(BoardData* bdat, std::string_view name) noexcept : m_bdat{nullptr}, m_map_index{0} {
    if (!bdat)
        return;
    [[maybe_unused]] const volatile std::size_t n = bdat->device_map.size();
    const auto it = bdat->device_map.find({name.begin(), name.end()});
    if (it == bdat->device_map.end())
        return;

    m_bdat = bdat;
    m_map_index = bdat->device_map.index_of(it);
}

[[nodiscard]] std::size_t VirtualDevices::size() noexcept {
    return exists() ? (m_bdat->device_map.begin() + m_map_index)->second.count : 0;
}

BoardDeviceView::BoardDeviceView(BoardView& bv) noexcept : m_bdat{bv.m_bdat} {}

[[nodiscard]] auto VirtualDevices::Iterator::operator*() const noexcept -> value_type {
    auto& [name, _] = *(m_bdat->device_map.begin() + m_base_index);
    return {{name.data(), name.size()}, {m_bdat, m_map_index, m_base_index}};
}
auto VirtualDevices::Iterator::operator++() noexcept -> Iterator& {
    ++m_base_index;
    return *this;
}
auto VirtualDevices::Iterator::operator++(int) noexcept -> Iterator {
    Iterator prev = *this;
    ++m_base_index;
    return prev;
}
auto VirtualDevices::Iterator::operator+=(std::ptrdiff_t offset) noexcept -> Iterator& {
    m_base_index += offset;
    return *this;
}
auto VirtualDevices::Iterator::operator-=(std::ptrdiff_t offset) noexcept -> Iterator& {
    m_base_index -= offset;
    return *this;
}
auto VirtualDevices::Iterator::operator+(std::ptrdiff_t offset) const noexcept -> Iterator {
    return {m_bdat, m_map_index, m_base_index + offset};
}
auto VirtualDevices::Iterator::operator-(std::ptrdiff_t offset) const noexcept -> Iterator {
    return {m_bdat, m_map_index, m_base_index - offset};
}

} // namespace smce
