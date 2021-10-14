/*
 *  BoardView.cpp
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

#include "SMCE/BoardView.hpp"

#include <iterator>
#include <mutex>
#include <boost/date_time/microsec_time_clock.hpp>
#include <boost/date_time/posix_time/posix_time_duration.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include "SMCE/internal/BoardData.hpp"
#include "SMCE/internal/utils.hpp"

using microsec_clock = boost::date_time::microsec_clock<boost::posix_time::ptime>;

namespace smce {

[[nodiscard]] std::string_view BoardView::storage_get_root(Link link, std::uint16_t accessor) noexcept {
    if (!m_bdat)
        return {};
    using Bus = BoardData::DirectStorage::Bus;
    constexpr std::array<Bus, 3> link2bus{
        Bus{-1},  // UART
        Bus::SPI, // SPI
        Bus{-1},  // I2C
    };
    const auto bus = link2bus[static_cast<std::size_t>(link)];
    if (bus == Bus{-1})
        return {};

    const auto it = std::find_if(
        m_bdat->direct_storages.begin(), m_bdat->direct_storages.end(),
        [=](const BoardData::DirectStorage& ds) -> bool { return ds.bus == bus && ds.accessor == accessor; });
    if (it == m_bdat->direct_storages.end())
        return {};
    return it->root_dir;
}

[[nodiscard]] bool VirtualAnalogDriver::exists() noexcept { return m_bdat && m_idx < m_bdat->pins.size(); }

[[nodiscard]] bool VirtualAnalogDriver::can_read() noexcept { return exists() && m_bdat->pins[m_idx].can_analog_read; }

[[nodiscard]] bool VirtualAnalogDriver::can_write() noexcept {
    return exists() && m_bdat->pins[m_idx].can_analog_write;
}

[[nodiscard]] std::uint16_t VirtualAnalogDriver::read() noexcept {
    return exists() ? m_bdat->pins[m_idx].value.load() : 0;
}

void VirtualAnalogDriver::write(std::uint16_t value) noexcept {
    if (exists())
        m_bdat->pins[m_idx].value.store(value);
}

[[nodiscard]] bool VirtualDigitalDriver::exists() noexcept { return m_bdat && m_idx < m_bdat->pins.size(); }

[[nodiscard]] bool VirtualDigitalDriver::can_read() noexcept {
    return exists() && m_bdat->pins[m_idx].can_digital_read;
}

[[nodiscard]] bool VirtualDigitalDriver::can_write() noexcept {
    return exists() && m_bdat->pins[m_idx].can_digital_write;
}

[[nodiscard]] bool VirtualDigitalDriver::read() noexcept { return exists() && m_bdat->pins[m_idx].value.load(); }

void VirtualDigitalDriver::write(bool value) noexcept {
    if (exists())
        m_bdat->pins[m_idx].value.store(value ? 255 : 0);
}

[[nodiscard]] bool VirtualPin::exists() noexcept { return m_bdat && m_idx < m_bdat->pins.size(); }

[[nodiscard]] bool VirtualPin::locked() noexcept {
    return !exists() || m_bdat->pins[m_idx].active_driver != BoardData::Pin::ActiveDriver::gpio;
}

void VirtualPin::set_direction(DataDirection dir) noexcept {
    if (exists() && !locked())
        m_bdat->pins[m_idx].data_direction = static_cast<BoardData::Pin::DataDirection>(dir);
}

[[nodiscard]] auto VirtualPin::get_direction() noexcept -> DataDirection {
    return exists() && !locked() ? static_cast<DataDirection>(m_bdat->pins[m_idx].data_direction.load())
                                 : DataDirection::in;
}

VirtualPin VirtualPins::operator[](std::size_t pin_id) noexcept {
    if (!m_bdat)
        return {m_bdat, 0};
    const auto it = std::lower_bound(m_bdat->pins.begin(), m_bdat->pins.end(), pin_id,
                                     [](const auto& pin, std::size_t pin_id) { return pin.id < pin_id; });
    if (it != m_bdat->pins.end()) {
        if (const auto delta = std::distance(m_bdat->pins.begin(), it); delta >= 0 && m_bdat->pins[delta].id == pin_id)
            return {m_bdat, static_cast<std::size_t>(delta)};
    }
    return {nullptr, std::size_t(-1)};
}

[[nodiscard]] bool VirtualUartBuffer::exists() noexcept { return m_bdat && m_index < m_bdat->uart_channels.size(); }

[[nodiscard]] std::size_t VirtualUartBuffer::max_size() noexcept {
    // clang-format off
    return exists() ? (m_dir == Direction::rx
                           ? m_bdat->uart_channels[m_index].max_buffered_rx
                           : m_bdat->uart_channels[m_index].max_buffered_tx) : 0;
    // clang-format on
}

[[nodiscard]] std::size_t VirtualUartBuffer::size() noexcept {
    if (!exists())
        return 0;
    auto& chan = m_bdat->uart_channels[m_index];
    auto [d, mut] = [&] {
        switch (m_dir) {
        case Direction::rx:
            return std::tie(chan.rx, chan.rx_mut);
        case Direction::tx:
            return std::tie(chan.tx, chan.tx_mut);
        }
        unreachable();
    }();
    if (!mut.timed_lock(microsec_clock::universal_time() + boost::posix_time::seconds{1}))
        return 0;
    std::lock_guard lg{mut, std::adopt_lock};
    const auto ret = d.size();
    return ret;
}

std::size_t VirtualUartBuffer::read(std::span<char> buf) noexcept {
    if (!exists())
        return 0;
    auto& chan = m_bdat->uart_channels[m_index];
    auto [d, mut, max_buffered] = [&] {
        switch (m_dir) {
        case Direction::rx:
            return std::tie(chan.rx, chan.rx_mut, chan.max_buffered_rx);
        case Direction::tx:
            return std::tie(chan.tx, chan.tx_mut, chan.max_buffered_tx);
        }
        unreachable();
    }();
    if (!mut.timed_lock(microsec_clock::universal_time() + boost::posix_time::seconds{1}))
        return 0;
    std::lock_guard lg{mut, std::adopt_lock};
    const std::size_t count = std::min(d.size(), buf.size());
    std::copy_n(d.begin(), count, buf.begin());
    d.erase(d.begin(), d.begin() + count);
    return count;
}

std::size_t VirtualUartBuffer::write(std::span<const char> buf) noexcept {
    if (!exists())
        return 0;
    auto& chan = m_bdat->uart_channels[m_index];
    auto [d, mut, max_buffered] = [&] {
        switch (m_dir) {
        case Direction::rx:
            return std::tie(chan.rx, chan.rx_mut, chan.max_buffered_rx);
        case Direction::tx:
            return std::tie(chan.tx, chan.tx_mut, chan.max_buffered_tx);
        }
        unreachable();
    }();
    if (!mut.timed_lock(microsec_clock::universal_time() + boost::posix_time::seconds{1}))
        return 0;
    std::lock_guard lg{mut, std::adopt_lock};
    const std::size_t count = std::min(
        std::clamp(max_buffered - d.size(), std::size_t{0}, static_cast<std::size_t>(max_buffered)), buf.size());
    std::copy_n(buf.begin(), count, std::back_inserter(d));
    return count;
}

[[nodiscard]] char VirtualUartBuffer::front() noexcept {
    if (!exists())
        return '\0';
    auto& chan = m_bdat->uart_channels[m_index];
    auto [d, mut] = [&] {
        switch (m_dir) {
        case Direction::rx:
            return std::tie(chan.rx, chan.rx_mut);
        case Direction::tx:
            return std::tie(chan.tx, chan.tx_mut);
        }
        unreachable();
    }();
    if (!mut.timed_lock(microsec_clock::universal_time() + boost::posix_time::seconds{1}))
        return 0;
    std::lock_guard lg{mut, std::adopt_lock};
    if (d.empty())
        return '\0';
    const char ret = d.front();
    return ret;
}

[[nodiscard]] bool VirtualUart::exists() noexcept { return m_bdat && m_index < m_bdat->uart_channels.size(); }

[[nodiscard]] bool VirtualUart::is_active() noexcept {
    return exists() && m_bdat->uart_channels[m_index].active.load();
}

void VirtualUart::set_active(bool value) noexcept {
    if (exists())
        m_bdat->uart_channels[m_index].active.store(value);
}

[[nodiscard]] VirtualUart VirtualUarts::operator[](std::size_t idx) noexcept {
    if (!m_bdat || m_bdat->uart_channels.size() <= idx)
        return VirtualUart{m_bdat, idx};
    return VirtualUart{m_bdat, idx};
}

[[nodiscard]] auto VirtualUarts::begin() noexcept -> Iterator { return Iterator{*this}; }

[[nodiscard]] auto VirtualUarts::end() noexcept -> Iterator { return Iterator{*this, size()}; }

[[nodiscard]] std::size_t VirtualUarts::size() noexcept { return m_bdat ? m_bdat->uart_channels.size() : 0; }

[[nodiscard]] VirtualUart VirtualUarts::Iterator::operator*() noexcept { return m_vu[m_index]; }

[[nodiscard]] bool FrameBuffer::exists() noexcept { return m_bdat && m_idx < m_bdat->frame_buffers.size(); }

[[nodiscard]] auto FrameBuffer::direction() noexcept -> Direction {
    return exists() ? Direction{static_cast<uint8_t>(m_bdat->frame_buffers[m_idx].direction)} : Direction::in;
}

[[nodiscard]] bool FrameBuffer::needs_horizontal_flip() noexcept {
    return exists() && m_bdat->frame_buffers[m_idx].transform.load().horiz_flip;
}

void FrameBuffer::needs_horizontal_flip(bool val) noexcept {
    if (!exists())
        return;

    auto trans = m_bdat->frame_buffers[m_idx].transform.load();
    trans.horiz_flip = val;
    m_bdat->frame_buffers[m_idx].transform.store(trans);
}

[[nodiscard]] bool FrameBuffer::needs_vertical_flip() noexcept {
    return exists() && m_bdat->frame_buffers[m_idx].transform.load().vert_flip;
}

void FrameBuffer::needs_vertical_flip(bool val) noexcept {
    if (!exists())
        return;

    auto trans = m_bdat->frame_buffers[m_idx].transform.load();
    trans.vert_flip = val;
    m_bdat->frame_buffers[m_idx].transform.store(trans);
}

[[nodiscard]] std::uint16_t FrameBuffer::get_width() noexcept {
    return exists() ? m_bdat->frame_buffers[m_idx].width.load() : 0;
}

void FrameBuffer::set_width(std::uint16_t width) noexcept {
    if (!exists())
        return;
    auto& fb = m_bdat->frame_buffers[m_idx];
    fb.width = width;
    fb.data.resize(width * fb.height * 3);
}

[[nodiscard]] std::uint16_t FrameBuffer::get_height() noexcept {
    return exists() ? m_bdat->frame_buffers[m_idx].height.load() : 0;
}

void FrameBuffer::set_height(std::uint16_t height) noexcept {
    if (!exists())
        return;
    auto& fb = m_bdat->frame_buffers[m_idx];
    fb.height = height;
    fb.data.resize(height * fb.width * 3);
}

[[nodiscard]] std::uint8_t FrameBuffer::get_freq() noexcept {
    return exists() ? m_bdat->frame_buffers[m_idx].freq.load() : 0;
}

void FrameBuffer::set_freq(std::uint8_t freq) noexcept {
    if (!exists())
        return;
    m_bdat->frame_buffers[m_idx].freq = freq;
}

bool FrameBuffer::write_rgb888(std::span<const std::byte> buf) {
    if (!exists())
        return false;

    auto& frame_buf = m_bdat->frame_buffers[m_idx];
    if (buf.size() != frame_buf.data.size())
        return false;

    [[maybe_unused]] std::lock_guard lk{frame_buf.data_mut};
    std::memcpy(frame_buf.data.data(), buf.data(), buf.size());
    return true;
}

bool FrameBuffer::read_rgb888(std::span<std::byte> buf) {
    if (!exists())
        return false;

    auto& frame_buf = m_bdat->frame_buffers[m_idx];
    if (buf.size() != frame_buf.data.size())
        return false;
    [[maybe_unused]] std::lock_guard lk{frame_buf.data_mut};
    std::memcpy(buf.data(), frame_buf.data.data(), buf.size());
    return true;
}

/*
 * MEDIA_BUS_FMT_RGB444_2X8_PADHI_LE is laid as:
 * 76543210 | 76543210
 * GGGGBBBB   0000RRRR
 */
bool FrameBuffer::write_rgb444(std::span<const std::byte> buf) {
    if (!exists())
        return false;

    auto& frame_buf = m_bdat->frame_buffers[m_idx];
    if (buf.size() != frame_buf.data.size() / 3 * 2)
        return false;

    [[maybe_unused]] std::lock_guard lk{frame_buf.data_mut};

    auto from = buf.begin();
    auto to = frame_buf.data.begin();
    while (from != buf.end()) {
        const auto gb = *from++;
        const auto xr = *from++;
        *to++ = xr << 4;
        *to++ = gb & std::byte{0xF0};
        *to++ = gb << 4;
    }

    return true;
}

bool FrameBuffer::read_rgb444(std::span<std::byte> buf) {
    if (!exists())
        return false;

    auto& frame_buf = m_bdat->frame_buffers[m_idx];
    if (buf.size() != frame_buf.data.size() / 3 * 2)
        return false;
    [[maybe_unused]] std::lock_guard lk{frame_buf.data_mut};

    const auto* from = frame_buf.data.data();
    auto to = buf.begin();
    while (to != buf.end()) {
        const auto r = *from++;
        const auto g = *from++;
        const auto b = *from++;
        *to++ = (g & std::byte{0xF0}) | (b >> 4);
        *to++ = r >> 4;
    }
    return true;
}

/*
 * 76543210 | 76543210
 * GGGBBBBB   RRRRRGGG
 */
void rgb565ToRgb888(std::span<const std::byte> buf, std::byte * res) {
    // read two bytes
    // read from rgb565 then write to the rgb888
    for (auto i = buf.begin(); i != buf.end(); ++i) {
        std::byte low = *i;
        *i++;
        std::byte high = *i;

        // red
        *res++ = (high & (std::byte)0xF8) | (high >> 5);
        // green
        *res++ = ((high & (std::byte)0x07) << 5) | ((low & (std::byte)0xE0) >> 3) |
                 ((high & (std::byte)0x06) >> 1);
        // blue
        *res++ = (low << 3) | ((low & (std::byte)0x1F) >> 2);
    }
}

bool FrameBuffer::write_rgb565(std::span<const std::byte> buf) {
    if (!exists())
        return false;

    auto& frame_buf = m_bdat->frame_buffers[m_idx];
    if (buf.size() != frame_buf.data.size() / 3 * 2)
        return false;
    [[maybe_unused]] std::lock_guard lk{frame_buf.data_mut};

    auto* res = frame_buf.data.data();
    rgb565ToRgb888(buf, res);

    return true;
}

void rgb888ToRgb565(const std::byte* buf, std::span<std::byte> res) {
    // read two bytes(16bits)
    for (auto i = res.begin(); i != res.end(); ++i) {

        // pick out 24bits in buffer
        std::byte red = *buf++;
        std::byte green = *buf++;
        std::byte blue = *buf++;

        // read from rgb888 then write to the rgb565
        *i++ = ((green & (std::byte)0x1C) << 3) | ((blue & (std::byte)0xF8) >> 3);
        *i = (red & (std::byte)0xF8) | ((green & (std::byte)0xE0) >> 5);
    }
}

bool FrameBuffer::read_rgb565(std::span<std::byte> res) {
    if (!exists())
        return false;

    auto& frame_buf = m_bdat->frame_buffers[m_idx];
    if (res.size() != frame_buf.data.size() / 3 * 2)
        return false;
    [[maybe_unused]] std::lock_guard lk{frame_buf.data_mut};

    const auto* buf = frame_buf.data.data();
    rgb888ToRgb565(buf, res);

    return true;
}

/*
 * R1 G1 B1 R2 G2 B2
 * Y1   U   Y2   V
 * keep Y, two Y shared a U and a V so compressing the memory to two-thirds of the original
 */
void yuv422ToRgb888(std::span<const std::byte> buf, std::byte* res) {
    // read four bytes of yuv422 then write six bytes of rgb888
    for (auto i = buf.begin(); i != buf.end();) {
        std::byte y1 = *i;
        *i++;
        std::byte u = *i;
        *i++;
        std::byte y2 = *i;
        *i++;
        std::byte v = *i;
        *i++;

        /*
         * convert yuv422 to rgb888 of six bytes
         * https://zh.wikipedia.org/wiki/YUV
         * using the formula
         */
        double r1 = (double)y1 + 1.13983 * ((double)v - 128);
        double g1 = (double)y1 - 0.39465 * ((double)u - 128) - 0.58060 * ((double)v - 128);
        double b1 = (double)y1 + 2.03211 * ((double)u - 128);
        double r2 = (double)y2 + 1.13983 * ((double)v - 128);
        double g2 = (double)y2 - 0.39465 * ((double)u - 128) - 0.58060 * ((double)v - 128);
        double b2 = (double)y2 + 2.03211 * ((double)u - 128);

        // write rgb for two pixels
        *res++ = (std::byte)std::clamp(r1, 0.0, 255.0);
        *res++ = (std::byte)std::clamp(g1, 0.0, 255.0);
        *res++ = (std::byte)std::clamp(b1, 0.0, 255.0);
        *res++ = (std::byte)std::clamp(r2, 0.0, 255.0);
        *res++ = (std::byte)std::clamp(g2, 0.0, 255.0);
        *res++ = (std::byte)std::clamp(b2, 0.0, 255.0);
    }
}

bool FrameBuffer::write_yuv422(std::span<const std::byte> buf) {
    if (!exists())
        return false;

    auto& frame_buf = m_bdat->frame_buffers[m_idx];
    if (buf.size() != frame_buf.data.size() / 3 * 4)
        return false;
    [[maybe_unused]] std::lock_guard lk{frame_buf.data_mut};

    auto* res = frame_buf.data.data();
    yuv422ToRgb888(buf, res);

    return true;
}

void rgb888ToYuv422(const std::byte* buf, std::span<std::byte> res) {
    for (auto i = res.begin(); i != res.end();) {
        // read six bytes of rgb888 then write four bits of yuv422
        std::byte r1 = *buf++;
        std::byte g1 = *buf++;
        std::byte b1 = *buf++;
        std::byte r2 = *buf++;
        std::byte g2 = *buf++;
        std::byte b2 = *buf++;

        // using the formula
        double y1 = 0.299 * (double)r1 + 0.587 * (double)g1 + 0.114 * (double)b1;
        double u = ((-0.169 * (double)r1 - 0.331 * (double)g1 + 0.5 * (double)b1 + 128)+
                   (-0.169 * (double)r2 - 0.331 * (double)g2 + 0.5 * (double)b2 + 128))
                   / 2;
        double y2 = 0.299 * (double)r2 + 0.587 * (double)g2 + 0.114 * (double)b2;
        double v = ((0.5 * (double)r1 - 0.419 * (double)g1 - 0.081 * (double)b1 + 128)+
                   (0.5 * (double)r2 - 0.419 * (double)g2 - 0.081 * (double)b2 + 128))
                   / 2;
        /*
         * write to the yuv422, two pixels(32bits, 4bits)
         * y represents black and white,u for luma and v for chroma
         * https://stackoverflow.com/questions/63213344/how-to-write-a-yuv422-yuv420-video-data-y-u-v-buffers-to-buffer-of-a-video-pla
         */
        *i = (std::byte)y1;
        *i++;
        *i = (std::byte)u;
        *i++;
        *i = (std::byte)y2;
        *i++;
        *i = (std::byte)v;
        *i++;
    }
}

bool FrameBuffer::read_yuv422(std::span<std::byte> res) {
    if (!exists())
        return false;

    auto& frame_buf = m_bdat->frame_buffers[m_idx];
    if (res.size() != frame_buf.data.size() / 3 * 4)
        return false;
    [[maybe_unused]] std::lock_guard lk{frame_buf.data_mut};

    const auto* buf = frame_buf.data.data();
    rgb888ToYuv422(buf, res);

    return true;
}

    FrameBuffer FrameBuffers::operator[](std::size_t key) noexcept {
        if (!m_bdat)
            return {m_bdat, 0};
        const auto it = std::lower_bound(m_bdat->frame_buffers.begin(), m_bdat->frame_buffers.end(), key,
                                         [](const auto& pin, std::size_t key) { return pin.key < key; });
        if (it != m_bdat->frame_buffers.end()) {
            if (const auto delta = std::distance(m_bdat->frame_buffers.begin(), it);
                delta >= 0 && m_bdat->frame_buffers[delta].key == key)
                return {m_bdat, static_cast<std::size_t>(delta)};
        }
        return {nullptr, std::size_t(-1)};
    }

} // namespace smce