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

#ifndef LIBSMCE_BOARDDEVICESPECIFICATION_HPP
#define LIBSMCE_BOARDDEVICESPECIFICATION_HPP

#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <variant>
#include <SMCE/BoardDeviceFieldType.hpp>
#include <SMCE/SMCE_iface.h>
#include <SMCE/fwd.hpp>

namespace smce {

/**
 * BindGen-generated host board device specification
 **/
class SMCE_API BoardDeviceNativeSpecification {
    friend BoardDeviceSpecification;
    class SMCE_API Iterator {
        friend BoardDeviceNativeSpecification;

#if _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4251)
#endif
        std::string_view m_full;
        std::size_t m_off;
#if _MSC_VER
#    pragma warning(pop)
#endif

        constexpr Iterator(std::string_view full, std::size_t off) noexcept : m_full{full}, m_off{off} {}
        [[nodiscard]] std::string_view view() const noexcept;

      public:
        using difference_type = void;
        using value_type = std::pair<std::string_view, BoardDeviceFieldType>;
        using pointer = void;
        using reference = void;
        using iterator_category = std::forward_iterator_tag;

        Iterator& operator++() noexcept;
        Iterator operator++(int) noexcept;
        [[nodiscard]] value_type operator*() const noexcept;

        friend SMCE_API_FRIEND bool operator==(const Iterator& lhs, const Iterator& rhs) noexcept;
    };

#if _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4251)
#endif
    std::string_view m_full;
#if _MSC_VER
#    pragma warning(pop)
#endif

  public:
    explicit BoardDeviceNativeSpecification(std::string_view specstr) noexcept : m_full{specstr} {}

    using iterator = Iterator;

    [[nodiscard]] std::string_view name() const noexcept;    /// Device name
    [[nodiscard]] std::string_view version() const noexcept; /// Device semver
    [[nodiscard]] std::size_t size() const noexcept;         // Field count
    [[nodiscard]] Iterator begin() const noexcept;           // Fields begin iterator
    [[nodiscard]] Iterator end() const noexcept;             // Fields end iterator
    [[nodiscard]] Iterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] Iterator cend() const noexcept { return end(); }

    friend SMCE_API_FRIEND bool operator==(const BoardDeviceNativeSpecification& lhs,
                                           const BoardDeviceNativeSpecification& rhs) noexcept;
};

[[nodiscard]] SMCE_API bool operator==(const BoardDeviceNativeSpecification::iterator& lhs,
                                       const BoardDeviceNativeSpecification::iterator& rhs) noexcept;
[[nodiscard]] SMCE_API bool operator==(const BoardDeviceNativeSpecification& lhs,
                                       const BoardDeviceNativeSpecification& rhs) noexcept;

/**
 * User-defined, crafted board device specification
 **/
class BoardDeviceSyntheticSpecification {
  public:
    std::string name;
    std::string version;
    std::unordered_map<std::string, BoardDeviceFieldType> fields;
};

SMCE_API bool operator==(const BoardDeviceSyntheticSpecification& lhs,
                         const BoardDeviceSyntheticSpecification& rhs) noexcept;

/**
 * Read-only board device specification
 **/
class SMCE_API BoardDeviceSpecification {
    using Underlying = std::variant<BoardDeviceNativeSpecification, BoardDeviceSyntheticSpecification>;
#if _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4251)
#endif
    Underlying m_u;
#if _MSC_VER
#    pragma warning(pop)
#endif

    class SMCE_API Iterator {
        friend BoardDeviceSpecification;

        using Underlying = std::variant<BoardDeviceNativeSpecification::iterator,
                                        decltype(BoardDeviceSyntheticSpecification::fields)::const_iterator>;
#if _MSC_VER
#    pragma warning(push)
#    pragma warning(disable : 4251)
#endif
        Underlying m_u;
#if _MSC_VER
#    pragma warning(pop)
#endif

        constexpr Iterator(BoardDeviceNativeSpecification::iterator underlying) noexcept : m_u{underlying} {}
        Iterator(decltype(BoardDeviceSyntheticSpecification::fields)::const_iterator underlying) noexcept
            : m_u{underlying} {}

      public:
        using difference_type = void;
        using value_type = std::pair<std::string_view, BoardDeviceFieldType>;
        using pointer = void;
        using reference = void;
        using iterator_category = std::forward_iterator_tag;

        Iterator& operator++() noexcept;
        Iterator operator++(int) noexcept;
        [[nodiscard]] value_type operator*() const noexcept;

        friend SMCE_API_FRIEND bool operator==(const Iterator& lhs, const Iterator& rhs) noexcept;
    };

  public:
    using iterator = Iterator;

    explicit(false) BoardDeviceSpecification(BoardDeviceNativeSpecification u) : m_u{u} {}
    explicit(false) BoardDeviceSpecification(BoardDeviceSyntheticSpecification u) : m_u{std::move(u)} {}

    [[nodiscard]] std::string_view name() const noexcept;    /// Device name
    [[nodiscard]] std::string_view version() const noexcept; /// Device semver
    [[nodiscard]] std::size_t size() const noexcept;         /// Field count
    [[nodiscard]] Iterator begin() const noexcept;
    [[nodiscard]] Iterator end() const noexcept;
    [[nodiscard]] Iterator cbegin() const noexcept { return begin(); }
    [[nodiscard]] Iterator cend() const noexcept { return end(); }

    [[nodiscard]] std::string to_cmake() const;

    friend SMCE_API_FRIEND bool operator==(const BoardDeviceSpecification& lhs,
                                           const BoardDeviceSpecification& rhs) noexcept;
};

[[nodiscard]] SMCE_API bool operator==(const BoardDeviceSpecification::iterator& lhs,
                                       const BoardDeviceSpecification::iterator& rhs) noexcept;
[[nodiscard]] SMCE_API bool operator==(const BoardDeviceSpecification& lhs,
                                       const BoardDeviceSpecification& rhs) noexcept;

} // namespace smce

#endif // LIBSMCE_BOARDDEVICESPECIFICATION_HPP
