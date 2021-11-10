//
// Created by danie on 2021-11-02.
//

#ifndef FRAMEBUFFER_ACCESS_H
#define FRAMEBUFFER_ACCESS_H

#include "SMCE_dll.hpp"

enum SMCE_Pixel_Format {
    RGB888, // RRRRRRRRGGGGGGGGBBBBBBBB // SMCE extension
    RGB444, // GGGGBBBB----RRRR
};

class SMCE__DLL_RT_API FramebufferAccess {
  private:
    std::size_t m_key = 0;
    SMCE_Pixel_Format m_format = RGB888;
    bool m_begun = false;
  public:
    int begin(std::uint16_t width, std::uint16_t height, SMCE_Pixel_Format format, std::uint8_t fps);
    void end();
    bool read(void* buffer);
    bool write(void* buffer);

    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;
    [[nodiscard]] int bitsPerPixel() const;
    [[nodiscard]] int bytesPerPixel() const;

    void horizontalFlip(bool flipped);
    void verticalFlip(bool flipped);
};

#endif // FRAMEBUFFER_ACCESS_H