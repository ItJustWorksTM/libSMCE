//
// Created by danie on 2021-11-02.
//

#include <array>
#include <iostream>
#include <limits>
#include <SMCE/BoardView.hpp>
#include "../../include/Ardrivo/FrameBufferAccess.h"

namespace smce {
extern BoardView board_view;
extern void maybe_init();
} // namespace smce

int FramebufferAccess::begin(std::uint16_t width, std::uint16_t height, SMCE_Pixel_Format format, std::uint8_t fps) {
    const auto error = [=](const char* msg) {
        std::cerr << "ERROR: FramebufferAccess::begin(" << width << "x" << height << ", " << format << ", " << fps
                  << "): " << msg << std::endl;
        return -1;
    };
    if (m_begun) {
        std::cerr << "FramebufferAccess::begin: device already active" << std::endl;
        return -1;
    }
    switch (format) {
    case RGB888:
    case RGB444:
        m_format = format;
        break;
    default:
        return error("Unknown format");
    }

    auto fb = smce::board_view.frame_buffers[m_key];
    if (!fb.exists())
        return error("Framebuffer does not exist");
    if (fb.direction() != smce::FrameBuffer::Direction::in)
        return error("Framebuffer not in input mode");

    fb.set_width(width);
    fb.set_height(height);
    fb.set_freq(fps);

    m_begun = true;
    return 0;
}

void FramebufferAccess::end() {
    if (!m_begun) {
        std::cerr << "FramebufferAccess::end: device inactive" << std::endl;
        return;
    }
    auto fb = smce::board_view.frame_buffers[m_key];
    fb.set_width(0);
    fb.set_height(0);
    fb.set_freq(0);
    m_begun = false;
}

bool FramebufferAccess::read(void* buffer) {
    if (!m_begun) {
        std::cerr << "FramebufferAccess::read: device inactive" << std::endl;
        return false;
    }
    using ReadType = std::add_const_t<decltype(&smce::FrameBuffer::read_rgb888)>;
    constexpr ReadType format_read[2] = {
        &smce::FrameBuffer::read_rgb888,
        &smce::FrameBuffer::read_rgb444,
    };
    (smce::board_view.frame_buffers[m_key].*format_read[m_format])(
        {static_cast<std::byte*>(buffer), static_cast<std::size_t>(bitsPerPixel() * width() * height() / CHAR_BIT)});
    return true;
}

bool FramebufferAccess::write(void* buffer) {
    if (!m_begun) {
        std::cerr << "FramebufferAccess::write: device inactive" << std::endl;
        return false;
    }
    using WriteType = std::add_const_t<decltype(&smce::FrameBuffer::write_rgb888)>;
    constexpr WriteType format_write[2] = {
        &smce::FrameBuffer::write_rgb888,
        &smce::FrameBuffer::write_rgb444,
    };
    (smce::board_view.frame_buffers[m_key].*format_write[m_format])(
        {static_cast<std::byte*>(buffer), static_cast<std::size_t>(bitsPerPixel() * width() * height() / CHAR_BIT)});
    return true;
}

int FramebufferAccess::width() const {
    if (!m_begun) {
        std::cerr << "FramebufferAccess::width: device inactive" << std::endl;
        return 0;
    }
    return smce::board_view.frame_buffers[m_key].get_width();
}

int FramebufferAccess::height() const {
    if (!m_begun) {
        std::cerr << "FramebufferAccess::height: device inactive" << std::endl;
        return 0;
    }
    return smce::board_view.frame_buffers[m_key].get_height();
}

constexpr std::array<std::pair<int, int>, 2> bits_bytes_pixel_formats{{{24, 3}, {16, 2}}};

int FramebufferAccess::bitsPerPixel() const {
    if (!m_begun) {
        std::cerr << "FramebufferAccess::bitsPerPixel: device inactive" << std::endl;
        return 0;
    }
    return bits_bytes_pixel_formats[m_format].first;
}

int FramebufferAccess::bytesPerPixel() const {
    if (!m_begun) {
        std::cerr << "FramebufferAccess::bytesPerPixel: device inactive" << std::endl;
        return 0;
    }
    return bits_bytes_pixel_formats[m_format].second;
}

void FramebufferAccess::horizontalFlip(bool flipped) {
    if (!m_begun) {
        std::cerr << "OV767X::horizontalFlip: device inactive" << std::endl;
        return;
    }
    smce::board_view.frame_buffers[m_key].needs_horizontal_flip(flipped);
}

void FramebufferAccess::verticalFlip(bool flipped) {
    if (!m_begun) {
        std::cerr << "OV767X::verticalFlip: device inactive" << std::endl;
        return;
    }
    smce::board_view.frame_buffers[m_key].needs_vertical_flip(flipped);
}