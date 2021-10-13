//
// Created by ziggy on 2021-10-13.
//

#include "MockDisplay.h"

MockDisplayClass::MockDisplayClass(int width, int height) : ArduinoGraphics(width, height) {}

MockDisplayClass::~MockDisplayClass() {}

void MockDisplayClass::beginDraw() { ArduinoGraphics::beginDraw(); }

void MockDisplayClass::endDraw() { ArduinoGraphics::endDraw(); }

void MockDisplayClass::set(int x, int y, uint8_t r, uint8_t g, uint8_t b) {}
