/*
 *  BoardData.cpp
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

#include "SMCE/internal/BoardData.hpp"

#include <algorithm>
#include <iterator>
#include <numeric>
#include <string_view>
#include <boost/hana/size.hpp>
#include "SMCE/BoardConf.hpp"
#include "SMCE/BoardDeviceSpecification.hpp"

namespace bip = boost::interprocess;

namespace smce {

BoardData::UartChannel::UartChannel(const ShmAllocator<void>& shm_valloc) : rx{shm_valloc}, tx{shm_valloc} {}

BoardData::DirectStorage::DirectStorage(const ShmAllocator<void>& shm_valloc) : root_dir{shm_valloc} {}

BoardData::FrameBuffer::FrameBuffer(const ShmAllocator<void>& shm_valloc) : data{shm_valloc} {}

static DeviceFieldBanks banks_init(const ShmAllocator<void>& shm_valloc) {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        return DeviceFieldBanks{(static_cast<void>(Is), shm_valloc)...};
    }
    (std::make_index_sequence<boost::hana::size(device_field_bank_types)>());
}

BoardData::BoardData(const ShmAllocator<void>& shm_valloc, const BoardConfig& c) noexcept
    : pins{shm_valloc}, uart_channels{shm_valloc}, direct_storages{shm_valloc}, frame_buffers{shm_valloc},
      device_map{shm_valloc}, banks{banks_init(shm_valloc)} {
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

    // Count how many elements are needed per bank
    DeviceFieldBaseGroups needed{0};
    for (const auto& bd : c.board_devices) {
        for (const auto& [_, type] : bd.spec)
            needed[device_field_type_to_bank_idx[static_cast<std::size_t>(type)]] += bd.count;
    }

    // Resize each bank according to how many elements we need
    [&]<std::size_t... Is>(std::index_sequence<Is...>) { (banks[boost::hana::size_c<Is>].resize(needed[Is]), ...); }
    (std::make_index_sequence<std::tuple_size_v<decltype(needed)>>{});

    // Insert each device
    DeviceFieldBaseGroups bases{0};
    device_map.reserve(c.board_devices.size());
    for (const auto& bd : c.board_devices) {
        const std::size_t count = bd.count;
        const auto name = bd.spec.name();
        // clang-format off
        auto& dev = device_map.emplace<StaticCharVec32, Device>(
                        {name.begin(), name.end()},
                        {decltype(Device::fields){shm_valloc}, count, bases}
                    ).first->second;
        // clang-format on
        dev.fields.reserve(bd.spec.size());

        for (const auto& [field_name, type] : bd.spec) {
            // clang-format off
            dev.fields.emplace<StaticCharVec32, BoardDeviceFieldType>(
                {field_name.begin(), field_name.end()},
                BoardDeviceFieldType{type}
            );
            // clang-format on
            bases[device_field_type_to_bank_idx[static_cast<std::size_t>(type)]] += count;
        }
    }
}

} // namespace smce
