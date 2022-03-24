/*
 *  BoardConf.cpp
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

#include "SMCE/BoardConf.hpp"

namespace smce {

bool operator==(const BoardConfig::GpioDrivers& lhs, const BoardConfig::GpioDrivers& rhs) noexcept {
    return lhs.pin_id == rhs.pin_id && lhs.digital_driver == rhs.digital_driver &&
           lhs.analog_driver == rhs.analog_driver;
}

bool operator==(const BoardConfig::GpioDrivers::DigitalDriver& lhs,
                const BoardConfig::GpioDrivers::DigitalDriver& rhs) noexcept {
    return lhs.board_read == rhs.board_read && lhs.board_write == rhs.board_write;
}

bool operator==(const BoardConfig::GpioDrivers::AnalogDriver& lhs,
                const BoardConfig::GpioDrivers::AnalogDriver& rhs) noexcept {
    return lhs.board_read == rhs.board_read && lhs.board_write == rhs.board_write;
}

bool operator==(const BoardConfig::UartChannel& lhs, const BoardConfig::UartChannel& rhs) noexcept {
    return lhs.rx_pin_override == rhs.rx_pin_override && lhs.tx_pin_override == rhs.tx_pin_override &&
           lhs.baud_rate == rhs.baud_rate && lhs.rx_buffer_length == rhs.rx_buffer_length &&
           lhs.tx_buffer_length == rhs.tx_buffer_length && lhs.flushing_threshold == rhs.flushing_threshold;
}

bool operator==(const BoardConfig::SecureDigitalStorage& lhs, const BoardConfig::SecureDigitalStorage& rhs) noexcept {
    return lhs.cspin == rhs.cspin && lhs.root_dir == rhs.root_dir;
}

bool operator==(const BoardConfig::FrameBuffer& lhs, const BoardConfig::FrameBuffer& rhs) noexcept {
    return lhs.key == rhs.key && lhs.direction == rhs.direction;
}

bool operator==(const BoardConfig::BoardDevice& lhs, const BoardConfig::BoardDevice& rhs) noexcept {
    return lhs.count == rhs.count && lhs.spec == rhs.spec;
}

} // namespace smce