/*
 *  BoardConf.hpp
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

#ifndef SMCE_BOARDCONF_HPP
#define SMCE_BOARDCONF_HPP

#include <cstdint>
#include <functional>
#include <optional>
#include <vector>
#include "SMCE/SMCE_fs.hpp"
#include "SMCE/SMCE_iface.h"
#include "SMCE/fwd.hpp"

namespace smce {

/**
 * Configuration for running a sketch
 **/
struct SMCE_API BoardConfig {
    struct SMCE_API GpioDrivers {
        struct DigitalDriver {
            bool board_read;
            bool board_write;
        };
        struct AnalogDriver {
            bool board_read;
            bool board_write;
            // std::size_t width;
        };
        std::uint16_t pin_id{};
        std::optional<DigitalDriver> digital_driver;
        std::optional<AnalogDriver> analog_driver;
    };
    struct SMCE_API UartChannel {
        std::optional<std::uint16_t> rx_pin_override;
        std::optional<std::uint16_t> tx_pin_override;
        std::uint16_t baud_rate = 9600;
        std::size_t rx_buffer_length = 64;
        std::size_t tx_buffer_length = 64;
        std::size_t flushing_threshold = 0;
    };
    /*
    struct SMCE_API I2cBus {
        std::optional<std::uint16_t> rx_pin_override;
        std::optional<std::uint16_t> tx_pin_override;
    };
    */
    struct SMCE_API SecureDigitalStorage {
        std::uint16_t cspin = 0; /// SPI Chip-Select pin; default one opened is 0
        stdfs::path root_dir;    /// Path to root directory
    };

    struct SMCE_API FrameBuffer {
        // clang-format off
        /// \ref smce::FrameBuffer::Direction
        enum struct Direction {
            in,
            out,
        };
        // clang-format on
        std::size_t key;
        Direction direction;
    };

    struct BoardDevice {
        std::reference_wrapper<const BoardDeviceSpecification> spec;
        std::size_t count;
    };

    std::vector<std::uint16_t> pins;        /// GPIO pins
    std::vector<GpioDrivers> gpio_drivers;  /// GPIO drivers to apply on existing pins
    std::vector<UartChannel> uart_channels; /// UART channels
    // std::vector<I2cBus> i2c_buses;
    std::vector<SecureDigitalStorage> sd_cards;
    std::vector<FrameBuffer> frame_buffers; /// Frame-buffers (cameras & screens)
    std::vector<BoardDevice> board_devices; /// Board devices to install
};

[[nodiscard]] SMCE_API bool operator==(const BoardConfig::GpioDrivers&, const BoardConfig::GpioDrivers&) noexcept;
[[nodiscard]] SMCE_API bool operator==(const BoardConfig::GpioDrivers::DigitalDriver&,
                                       const BoardConfig::GpioDrivers::DigitalDriver&) noexcept;
[[nodiscard]] SMCE_API bool operator==(const BoardConfig::GpioDrivers::AnalogDriver&,
                                       const BoardConfig::GpioDrivers::AnalogDriver&) noexcept;
[[nodiscard]] SMCE_API bool operator==(const BoardConfig::UartChannel&, const BoardConfig::UartChannel&) noexcept;
[[nodiscard]] SMCE_API bool operator==(const BoardConfig::SecureDigitalStorage&,
                                       const BoardConfig::SecureDigitalStorage&) noexcept;
[[nodiscard]] SMCE_API bool operator==(const BoardConfig::FrameBuffer&, const BoardConfig::FrameBuffer&) noexcept;
[[nodiscard]] SMCE_API bool operator==(const BoardConfig::BoardDevice&, const BoardConfig::BoardDevice&) noexcept;

} // namespace smce

#endif // SMCE_BOARDCONF_HPP
