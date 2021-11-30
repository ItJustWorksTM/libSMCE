/*
 *  BoardData.cpp
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

#include "SMCE/internal/BoardData.hpp"

#include <algorithm>
#include <numeric>
#include "SMCE/BoardConf.hpp"
#include "SMCE/internal/BoardDeviceSpecification.hpp"

namespace bip = boost::interprocess;

namespace smce {

BoardData::UartChannel::UartChannel(const ShmAllocator<void>& shm_valloc) : rx{shm_valloc}, tx{shm_valloc} {}

BoardData::DirectStorage::DirectStorage(const ShmAllocator<void>& shm_valloc) : root_dir{shm_valloc} {}

BoardData::FrameBuffer::FrameBuffer(const ShmAllocator<void>& shm_valloc) : data{shm_valloc} {}

BoardData::BoardData(const ShmAllocator<void>& shm_valloc, const BoardConfig& c) noexcept
    : pins{shm_valloc}, uart_channels{shm_valloc}, direct_storages{shm_valloc}, frame_buffers{shm_valloc},
      device_allocation_map{shm_valloc}, raw_bank{shm_valloc}, a8_bank{shm_valloc}, a16_bank{shm_valloc},
      a32_bank{shm_valloc}, a64_bank{shm_valloc}, mtx_bank{shm_valloc} {
    auto sorted_pins = c.pins;
    std::sort(sorted_pins.begin(), sorted_pins.end());

    pins.reserve(sorted_pins.size());
    for (const auto pin_id : sorted_pins) {
        auto& pin_obj = pins.emplace_back();
        pin_obj.id = pin_id;
    }

    for (const auto& gpio_driver : c.gpio_drivers) {
        const auto it = std::find(sorted_pins.begin(), sorted_pins.end(), gpio_driver.pin_id);
        if (it == sorted_pins.end())
            continue;
        const auto pin_idx = std::distance(sorted_pins.begin(), it);
        auto& pin = pins[pin_idx];
        if (gpio_driver.analog_driver) {
            auto& driver = gpio_driver.analog_driver.value();
            pin.can_analog_read = driver.board_read;
            pin.can_analog_write = driver.board_write;
        }
        if (gpio_driver.digital_driver) {
            auto& driver = gpio_driver.digital_driver.value();
            pin.can_digital_read = driver.board_read;
            pin.can_digital_write = driver.board_write;
        }
    }

    uart_channels.reserve(c.uart_channels.size());
    for (const auto& conf : c.uart_channels) {
        auto& data = uart_channels.emplace_back(shm_valloc);
        data.baud_rate = conf.baud_rate;
        data.rx_pin_override = conf.rx_pin_override;
        data.tx_pin_override = conf.tx_pin_override;
        data.max_buffered_rx = static_cast<std::uint16_t>(conf.rx_buffer_length);
        data.max_buffered_tx = static_cast<std::uint16_t>(conf.tx_buffer_length);
    }

    direct_storages.reserve(c.sd_cards.size());
    for (const auto& conf : c.sd_cards) {
        auto& data = direct_storages.emplace_back(shm_valloc);
        data.bus = BoardData::DirectStorage::Bus::SPI;
        data.accessor = conf.cspin;
        {
            auto rt_str = conf.root_dir.generic_string();
            data.root_dir.assign(std::string_view{rt_str});
        }
    }

    frame_buffers.reserve(c.frame_buffers.size());
    for (const auto& conf : c.frame_buffers) {
        auto& data = frame_buffers.emplace_back(shm_valloc);
        data.key = conf.key;
        data.direction = BoardData::FrameBuffer::Direction{static_cast<std::uint8_t>(conf.direction)};
    }

    std::size_t r8s_needed = 0;
    std::size_t r16s_needed = 0;
    std::size_t r32s_needed = 0;
    std::size_t r64s_needed = 0;
    std::size_t a8s_needed = 0;
    std::size_t a16s_needed = 0;
    std::size_t a32s_needed = 0;
    std::size_t a64s_needed = 0;
    std::size_t mtxs_needed = 0;

    for (const auto& bd : c.board_devices) {
        r8s_needed += bd.spec.get().r8_count * bd.count;
        r16s_needed += bd.spec.get().r16_count * bd.count;
        r32s_needed += bd.spec.get().r32_count * bd.count;
        r64s_needed += bd.spec.get().r64_count * bd.count;
        a8s_needed += bd.spec.get().a8_count * bd.count;
        a16s_needed += bd.spec.get().a16_count * bd.count;
        a32s_needed += bd.spec.get().a32_count * bd.count;
        a64s_needed += bd.spec.get().a64_count * bd.count;
        mtxs_needed += bd.spec.get().mtx_count * bd.count;
    }

    smce_rt::BoardDeviceAllocationIntBases bases{};
    bases.r64 = 0;
    bases.r32 = bases.r64 + r64s_needed * sizeof(std::uint64_t);
    bases.r16 = bases.r32 + r32s_needed * sizeof(std::uint32_t);
    bases.r8 = bases.r16 + r16s_needed * sizeof(std::uint16_t);

    raw_bank.resize(bases.r8 + r8s_needed * sizeof(std::uint8_t));
    a8_bank.resize(a8s_needed);
    a16_bank.resize(a16s_needed);
    a32_bank.resize(a32s_needed);
    a64_bank.resize(a64s_needed);
    mtx_bank.resize(mtxs_needed);

    for (const auto& bd : c.board_devices) {
        bases.count = bd.count;
        device_allocation_map.emplace(ShmString{bd.spec.get().name, shm_valloc}, bases);
        bases.r8 += bd.spec.get().r8_count * sizeof(std::uint8_t);
        bases.r16 += bd.spec.get().r16_count * sizeof(std::uint16_t);
        bases.r32 += bd.spec.get().r32_count * sizeof(std::uint32_t);
        bases.r64 += bd.spec.get().r64_count * sizeof(std::uint64_t);
        bases.a8 += bd.spec.get().a8_count;
        bases.a16 += bd.spec.get().a16_count;
        bases.a32 += bd.spec.get().a32_count;
        bases.a64 += bd.spec.get().a64_count;
        bases.mtx += bd.spec.get().mtx_count;
    }
}

} // namespace smce