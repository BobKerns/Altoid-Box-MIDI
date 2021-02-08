#pragma once

#include "lcdgfx.h"
#undef min
#undef max
#include <functional>
#include "Window.h"

using RawDisplay = DisplaySSD1306_128x64_I2C;
extern RawDisplay rawDisplay;
using Display = WindowImpl<RawDisplay>;
extern Display display;

using DisplayFn = std::function<void()>;

const auto SCREEN_WIDTH = 128; // OLED display width, in pixels
const auto SCREEN_HEIGHT = 32; // OLED display height, in pixels

// Called from loop()
extern void doDisplay();

extern void defaultDisplayHead();
extern void defaultDisplayBody();

extern DisplayFn displayHead;
extern DisplayFn displayBody;

// Make these be the current display until changed.
extern void show(DisplayFn head = displayHead, DisplayFn body = displayBody);

// Make these be the current display until ms milliseconds have passed.
extern void showFor(uint32_t ms, DisplayFn head = displayHead, DisplayFn body = displayBody);

// Make these be the current display heading until ms milliseconds have passed.
extern void showHeadFor(uint32_t ms, DisplayFn head);

// Make these be the current display body until ms milliseconds have passed.
extern void showBodyFor(uint32_t ms, DisplayFn body);
