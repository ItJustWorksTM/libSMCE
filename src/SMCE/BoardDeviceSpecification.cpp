/*
 *  BoardDeviceSpecification.cpp
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

#include <algorithm>
#include <sstream>
#include <string>
#include "SMCE/BoardDeviceSpecification.hpp"
#include "SMCE/internal/utils.hpp"

namespace smce {

std::string_view BoardDeviceNativeSpecification::Iterator::view() const noexcept {
    const auto opening_quote = m_off + 1;
    const auto closing_quote = m_full.find('"', opening_quote + 1);
    return m_full.substr(opening_quote + 1, closing_quote - opening_quote - 1);
}
auto BoardDeviceNativeSpecification::Iterator::operator++() noexcept -> Iterator& {
    m_off = m_full.find(':', m_off + 1);
    return *this;
}
auto BoardDeviceNativeSpecification::Iterator::operator++(int) noexcept -> Iterator {
    const auto prev = *this;
    ++*this;
    return prev;
}
auto BoardDeviceNativeSpecification::Iterator::operator*() const noexcept -> value_type {
    const auto element = view();
    const auto separator = element.find(' ');
    const auto type = element.substr(0, separator);
    const auto name = element.substr(separator + 1);
    return {name, to_board_device_field_type(type)};
}

std::string_view BoardDeviceNativeSpecification::name() const noexcept { return Iterator{m_full, 0}.view(); }
std::string_view BoardDeviceNativeSpecification::version() const noexcept { return (++Iterator{m_full, 0}).view(); }

std::size_t BoardDeviceNativeSpecification::size() const noexcept {
    return std::count(m_full.begin(), m_full.end(), ':') - 1;
}

auto BoardDeviceNativeSpecification::begin() const noexcept -> Iterator { return ++ ++Iterator{m_full, 0}; }
auto BoardDeviceNativeSpecification::end() const noexcept -> Iterator {
    return Iterator{m_full, std::string_view::npos};
}

SMCE_API bool operator==(const BoardDeviceNativeSpecification::iterator& lhs,
                         const BoardDeviceNativeSpecification::iterator& rhs) noexcept {
    return lhs.m_off == rhs.m_off && lhs.m_full == rhs.m_full;
}
SMCE_API bool operator==(const BoardDeviceNativeSpecification& lhs,
                         const BoardDeviceNativeSpecification& rhs) noexcept {
    return lhs.m_full == rhs.m_full;
}

SMCE_API bool operator==(const BoardDeviceSyntheticSpecification& lhs,
                         const BoardDeviceSyntheticSpecification& rhs) noexcept {
    return lhs.name == rhs.name && lhs.version == rhs.version && lhs.fields == rhs.fields;
}

auto BoardDeviceSpecification::Iterator::operator++() noexcept -> Iterator& {
    std::visit([](auto& it) { ++it; }, m_u);
    return *this;
}
auto BoardDeviceSpecification::Iterator::operator++(int) noexcept -> Iterator {
    const auto prev = *this;
    ++*this;
    return prev;
}
auto BoardDeviceSpecification::Iterator::operator*() const noexcept -> value_type {
    return std::visit([](const auto& it) -> value_type { return *it; }, m_u);
}

std::string_view BoardDeviceSpecification::name() const noexcept {
    // clang-format off
    return std::visit(
        Visitor{
            [](const BoardDeviceNativeSpecification& e) { return e.name(); },
            [](const BoardDeviceSyntheticSpecification& e) { return std::string_view(e.name); }
        },
        m_u);
    // clang-format on
}
std::string_view BoardDeviceSpecification::version() const noexcept {
    // clang-format off
    return std::visit(
        Visitor{
            [](const BoardDeviceNativeSpecification& e) { return e.version(); },
            [](const BoardDeviceSyntheticSpecification& e) { return std::string_view(e.version); }
        },
        m_u);
    // clang-format on
}

std::size_t BoardDeviceSpecification::size() const noexcept {
    // clang-format off
    return std::visit(
        Visitor{
            [](const BoardDeviceNativeSpecification& e) { return e.size(); },
            [](const BoardDeviceSyntheticSpecification& e) { return e.fields.size(); }
        },
        m_u);
    // clang-format on
}

auto BoardDeviceSpecification::begin() const noexcept -> Iterator {
    // clang-format off
    return std::visit(
        Visitor{
            [](const BoardDeviceNativeSpecification& e) { return Iterator{e.begin()}; },
            [](const BoardDeviceSyntheticSpecification& e) { return Iterator{e.fields.cbegin()}; }
        },
        m_u);
}

auto BoardDeviceSpecification::end() const noexcept -> Iterator {
    // clang-format off
    return std::visit(
        Visitor{
            [](const BoardDeviceNativeSpecification& e) { return Iterator{e.end()}; },
            [](const BoardDeviceSyntheticSpecification& e) { return Iterator{e.fields.cend()}; }
        },
        m_u);
    // clang-format on
}

std::string smce::BoardDeviceSpecification::to_cmake() const {
    // clang-format off
    return std::visit(
        Visitor{
            [](const BoardDeviceNativeSpecification& e) {
                std::string str;
                str.resize(e.m_full.size());
                std::replace_copy(e.m_full.begin(), e.m_full.end(), str.begin(), ':', ' ');
                return str;
            },
            [](const BoardDeviceSyntheticSpecification& e) {
                std::ostringstream oss;
                oss << '\"' << e.name << '\"' << ';';
                oss << '\"' << e.version << '\"' << ';';
                for (const auto& [field_name, field_type] : e.fields)
                    oss << '\"' << field_name << ';' << to_string(field_type) << '\"' << ';';
                auto str = std::move(oss).str();
                str.pop_back();
                return str;
            }
        },
        m_u);
    // clang-format on
}

SMCE_API bool operator==(const BoardDeviceSpecification::iterator& lhs,
                         const BoardDeviceSpecification::iterator& rhs) noexcept {
    return lhs.m_u == rhs.m_u;
}
SMCE_API bool operator==(const BoardDeviceSpecification& lhs, const BoardDeviceSpecification& rhs) noexcept {
    return lhs.m_u == rhs.m_u;
}

} // namespace smce
