/*
 *  BoardView.hpp
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

#ifndef SMCE_BOARDVIEW_HPP
#define SMCE_BOARDVIEW_HPP

#include <cstdint>
#include <span>
#include <string_view>
#include "SMCE/SMCE_iface.h"
#include "SMCE/fwd.hpp"

namespace smce {

/**
 * Analog driver for a GPIO pin
 **/
class SMCE_API VirtualAnalogDriver {
    friend class VirtualPin;
    BoardData* m_bdat;
    std::size_t m_idx;
    constexpr VirtualAnalogDriver(BoardData* bdat, std::size_t idx) : m_bdat{bdat}, m_idx{idx} {}

    friend constexpr bool operator==(const VirtualAnalogDriver& lhs, const VirtualAnalogDriver& rhs) noexcept;

  public:
    /// Object validity check
    [[nodiscard]] bool exists() noexcept;
    [[nodiscard]] bool can_read() noexcept;
    [[nodiscard]] bool can_write() noexcept;
    [[nodiscard]] std::uint16_t read() noexcept;
    void write(std::uint16_t) noexcept;
};

constexpr bool operator==(const VirtualAnalogDriver& lhs, const VirtualAnalogDriver& rhs) noexcept {
    return lhs.m_bdat == rhs.m_bdat && lhs.m_idx == rhs.m_idx;
}

class SMCE_API VirtualDigitalDriver {
    friend class VirtualPin;
    BoardData* m_bdat;
    std::size_t m_idx;
    constexpr VirtualDigitalDriver(BoardData* bdat, std::size_t idx) : m_bdat{bdat}, m_idx{idx} {}

    friend constexpr bool operator==(const VirtualDigitalDriver& lhs, const VirtualDigitalDriver& rhs) noexcept;

  public:
    /// Object validity check
    [[nodiscard]] bool exists() noexcept;
    [[nodiscard]] bool can_read() noexcept;
    [[nodiscard]] bool can_write() noexcept;
    [[nodiscard]] bool read() noexcept;
    void write(bool) noexcept;
};

constexpr bool operator==(const VirtualDigitalDriver& lhs, const VirtualDigitalDriver& rhs) noexcept {
    return lhs.m_bdat == rhs.m_bdat && lhs.m_idx == rhs.m_idx;
}

class SMCE_API VirtualPin {
    friend class VirtualPins;
    BoardData* m_bdat;
    std::size_t m_idx;
    constexpr VirtualPin(BoardData* bdat, std::size_t idx) : m_bdat{bdat}, m_idx{idx} {}

    friend constexpr bool operator==(const VirtualPin& lhs, const VirtualPin& rhs) noexcept;

  public:
    // clang-format off
    enum class DataDirection {
        in,
        out
    };
    // clang-format on
    /// Object validity check
    [[nodiscard]] bool exists() noexcept;
    [[nodiscard]] bool locked() noexcept;
    void set_direction(DataDirection) noexcept;
    [[nodiscard]] DataDirection get_direction() noexcept;

    [[nodiscard]] VirtualDigitalDriver digital() noexcept { return {m_bdat, m_idx}; }
    [[nodiscard]] VirtualAnalogDriver analog() noexcept { return {m_bdat, m_idx}; }
};

constexpr bool operator==(const VirtualPin& lhs, const VirtualPin& rhs) noexcept {
    return lhs.m_bdat == rhs.m_bdat && lhs.m_idx == rhs.m_idx;
}

class SMCE_API VirtualPins {
    friend BoardView;
    BoardData* m_bdat;
    explicit VirtualPins(BoardData* bdat) : m_bdat{bdat} {}

    friend constexpr bool operator==(const VirtualPins& lhs, const VirtualPins& rhs) noexcept;

  public:
    // struct Iterator;

    [[nodiscard]] VirtualPin operator[](std::size_t idx) noexcept;
    // [[nodiscard]] Iterator begin() noexcept;
    // [[nodiscard]] Iterator end() noexcept;
    // [[nodiscard]] std::size_t size() noexcept;
};

constexpr bool operator==(const VirtualPins& lhs, const VirtualPins& rhs) noexcept { return lhs.m_bdat == rhs.m_bdat; }

class SMCE_API VirtualUartBuffer {
    friend class VirtualUart;
    // clang-format off
    enum class Direction { rx, tx };
    // clang-format on
    BoardData* m_bdat;
    std::size_t m_index;
    Direction m_dir;
    constexpr VirtualUartBuffer(BoardData* bdat, std::size_t idx, Direction dir)
        : m_bdat{bdat}, m_index{idx}, m_dir{dir} {}

    friend constexpr bool operator==(const VirtualUartBuffer& lhs, const VirtualUartBuffer& rhs) noexcept;

  public:
    /// Object validity check
    [[nodiscard]] bool exists() noexcept;
    [[nodiscard]] std::size_t max_size() noexcept;
    [[nodiscard]] std::size_t size() noexcept;
    std::size_t read(std::span<char>) noexcept;
    std::size_t write(std::span<const char>) noexcept;
    [[nodiscard]] char front() noexcept;
};

constexpr bool operator==(const VirtualUartBuffer& lhs, const VirtualUartBuffer& rhs) noexcept {
    return lhs.m_bdat == rhs.m_bdat && lhs.m_index == rhs.m_index && lhs.m_dir == rhs.m_dir;
}

class SMCE_API VirtualUart {
    friend class VirtualUarts;
    BoardData* m_bdat;
    std::size_t m_index;
    constexpr VirtualUart(BoardData* bdat, std::size_t idx) : m_bdat{bdat}, m_index{idx} {}

    friend constexpr bool operator==(const VirtualUart& lhs, const VirtualUart& rhs) noexcept;

  public:
    /// Object validity check
    [[nodiscard]] bool exists() noexcept;
    [[nodiscard]] bool is_active() noexcept;
    void set_active(bool) noexcept; // Board-only
    VirtualUartBuffer rx() noexcept { return {m_bdat, m_index, VirtualUartBuffer::Direction::rx}; }
    VirtualUartBuffer tx() noexcept { return {m_bdat, m_index, VirtualUartBuffer::Direction::tx}; }
};

constexpr bool operator==(const VirtualUart& lhs, const VirtualUart& rhs) noexcept {
    return lhs.m_bdat == rhs.m_bdat && lhs.m_index == rhs.m_index;
}

class SMCE_API VirtualUarts {
    friend BoardView;
    BoardData* m_bdat;
    constexpr VirtualUarts() noexcept = default;
    constexpr explicit VirtualUarts(BoardData* bdat) : m_bdat{bdat} {}

    friend constexpr bool operator==(const VirtualUarts& lhs, const VirtualUarts& rhs) noexcept;

  public:
    class Iterator;
    friend Iterator;

    using iterator = Iterator;

    constexpr VirtualUarts(const VirtualUarts&) noexcept = default;
    constexpr VirtualUarts& operator=(const VirtualUarts&) noexcept = default;

    [[nodiscard]] VirtualUart operator[](std::size_t) noexcept;
    [[nodiscard]] Iterator begin() noexcept;
    [[nodiscard]] Iterator end() noexcept;
    [[nodiscard]] std::size_t size() noexcept;
};

constexpr bool operator==(const VirtualUarts& lhs, const VirtualUarts& rhs) noexcept {
    return lhs.m_bdat == rhs.m_bdat;
}

/**
 * An RGB888 framebuffer, holding a single frame.
 * Intended to be used to implement cameras and screen library shims.
 **/
class SMCE_API FrameBuffer {
    friend class FrameBuffers;
    BoardData* m_bdat;
    std::size_t m_idx;

    constexpr FrameBuffer(BoardData* bdat, std::size_t idx) noexcept : m_bdat{bdat}, m_idx{idx} {}

  public:
    // clang-format off
    /// Data direction
    enum struct Direction {
        in, /// host-to-board (camera)
        out, /// board-to-host (screen)
    };
    // clang-format on

    /// Object validity check
    [[nodiscard]] bool exists() noexcept;
    /// Data direction getter
    [[nodiscard]] Direction direction() noexcept;

    /// Flag getter for hflip
    [[nodiscard]] bool needs_horizontal_flip() noexcept;
    /// Flag setter for hflip
    void needs_horizontal_flip(bool) noexcept;
    /// Flag getter for vflip
    [[nodiscard]] bool needs_vertical_flip() noexcept;
    /// Flag setter for vflip
    void needs_vertical_flip(bool) noexcept;

    /// \note Size in px
    [[nodiscard]] std::uint16_t get_width() noexcept;
    /// \note Size in px
    void set_width(std::uint16_t) noexcept;
    /// \note Size in px
    [[nodiscard]] std::uint16_t get_height() noexcept;
    /// \note Size in px
    void set_height(std::uint16_t) noexcept;

    /// \note Frequency is in Hz
    [[nodiscard]] std::uint8_t get_freq() noexcept;
    /// \note Frequency is in Hz
    void set_freq(std::uint8_t) noexcept;

    /// Copies a frame from a packed buffer of pixels in the format RRRRRRRRGGGGGGGGBBBBBBBB
    bool write_rgb888(std::span<const std::byte>);
    /// Copies a frame into a packed buffer of pixels in the format RRRRRRRRGGGGGGGGBBBBBBBB
    bool read_rgb888(std::span<std::byte>);
    /// Copies a frame from a packed buffer of pixels in the format GGGGBBBB0000RRRR
    bool write_rgb444(std::span<const std::byte>);
    /// Copies a frame into a packed buffer of pixels in the format GGGGBBBB0000RRRR
    bool read_rgb444(std::span<std::byte>);
};

class SMCE_API FrameBuffers {
    friend BoardView;
    BoardData* m_bdat;
    constexpr FrameBuffers() noexcept = default;
    constexpr explicit FrameBuffers(BoardData* bdat) noexcept : m_bdat{bdat} {}

    friend constexpr bool operator==(const FrameBuffers& lhs, const FrameBuffers& rhs) noexcept;

  public:
    constexpr FrameBuffers(const FrameBuffers&) noexcept = default;
    constexpr FrameBuffers& operator=(const FrameBuffers&) noexcept = default;

    [[nodiscard]] FrameBuffer operator[](std::size_t) noexcept;
};

constexpr bool operator==(const FrameBuffers& lhs, const FrameBuffers& rhs) noexcept {
    return lhs.m_bdat == rhs.m_bdat;
}

/**
 * Mutable view of the virtual board.
 * \note Must stay a no-fail interface (operations all silently fail on error and never cause UB)
 **/
class SMCE_API BoardView {
    BoardData* m_bdat{};

    friend BoardDeviceView;
    friend constexpr bool operator==(const BoardView& lhs, const BoardView& rhs) noexcept;

  public:
    // clang-format off
    enum class Link {
        UART,
        SPI,
        I2C,
    };
    // clang-format on

    VirtualPins pins{m_bdat};           /// GPIO pins
    VirtualUarts uart_channels{m_bdat}; /// UART channels
    // VirtualI2cs i2c_buses;
    // VirtualOpaqueDevices opaque_devices;
    FrameBuffers frame_buffers{m_bdat}; /// Camera/Screen frame-buffers

    constexpr BoardView() noexcept = default;
    explicit BoardView(BoardData& bdat) : m_bdat{&bdat} {}

    /// Object validity check
    [[nodiscard]] bool valid() noexcept { return m_bdat; }

    /// Whether or not there is an active stop request from the host
    [[nodiscard]] bool stop_requested() noexcept;

    /// Obtain the path to the root file of a storage device
    [[nodiscard]] std::string_view storage_get_root(Link link, std::uint16_t accessor) noexcept;
};

constexpr bool operator==(const BoardView& lhs, const BoardView& rhs) noexcept { return lhs.m_bdat == rhs.m_bdat; }

class SMCE_API VirtualUarts::Iterator {
    friend VirtualUarts;
    VirtualUarts m_vu{};
    std::size_t m_index = 0;
    constexpr Iterator() noexcept = default;
    constexpr explicit Iterator(const VirtualUarts& vu, std::size_t idx = 0) noexcept : m_vu{vu}, m_index{idx} {}

    friend constexpr bool operator==(const VirtualUarts::Iterator& lhs, const VirtualUarts::Iterator& rhs) noexcept;

  public:
    using value_type = VirtualUart;
    using difference_type = int;

    [[nodiscard]] VirtualUart operator*() noexcept;
    constexpr Iterator& operator++() noexcept {
        ++m_index;
        return *this;
    }
    inline Iterator operator++(int) noexcept {
        const auto ret = *this;
        ++m_index;
        return ret;
    }
};

constexpr bool operator==(const VirtualUarts::Iterator& lhs, const VirtualUarts::Iterator& rhs) noexcept {
    return lhs.m_vu == rhs.m_vu && lhs.m_index == rhs.m_index;
}

} // namespace smce

#endif // SMCE_BOARDVIEW_HPP
