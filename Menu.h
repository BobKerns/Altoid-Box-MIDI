#pragma once

#include "lcdgfx.h"

template<class T>
class Menu {
  const char * const * items;
  const uint8_t count;
  uint8_t selection;
  static const uint8_t top = 16;
  static const uint8_t left = 0;
  static const uint8_t width = 128;
  static const uint8_t height = 64 - top;
  public:
    Menu(uint8_t count, const char * const *items): Menu(count, 0, items) {}
    Menu(uint8_t count, uint8_t selection, const char* const*items): count(count), items(items), selection(selection) {
    }
    void select(uint8_t i) {
      if (i < count) {
        selection = i;
      } else {
        selection = count - 1;
      }
    }
    void draw(T &display) {
      auto first = selection == 0 ? 0 : selection == count - 1 ? count < 4 ? 0 : selection - 2 : selection - 1;
      auto selected = selection == 0 ? 0 : selection == count - 1 ? 2 : 1;
      auto show = [this, &display, first, selected](uint8_t i) {
        if (i == selected) {
          display.printFixed(left, top + i * 16, "> ", STYLE_NORMAL);
         display.printFixed(left + 12, top + i * 16, items[first + i], STYLE_NORMAL);
        } else {
         display.printFixed(left, top + i * 16, items[first + i], STYLE_NORMAL);
        }
      };
      show(0);
      show(1);
      show(2);
    }
    inline uint8_t size() const {
      return count;
    }
};
