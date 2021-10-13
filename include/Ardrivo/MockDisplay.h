//
// Created by ziggy on 2021-10-13.
//

#include <cstdint>
#include "ArduinoGraphics.h"

#ifndef LIBSMCE_MOCKDISPLAY_H
#define LIBSMCE_MOCKDISPLAY_H

#define DISPLAY_WIDTH  12
#define DISPLAY_HEIGHT 7


class MockDisplayClass : public ArduinoGraphics {
  public:
    MockDisplayClass(int width, int height);
    virtual ~MockDisplayClass();


    virtual void beginDraw();
    virtual void endDraw();

    virtual void set(int x, int y, uint8_t r, uint8_t g, uint8_t b);

};

#endif // LIBSMCE_MOCKDISPLAY_H




