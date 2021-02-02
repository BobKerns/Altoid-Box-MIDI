#pragma once

#include "lcdgfx.h"

template<class T>
class Menu {
  const char * const * items;
  const uint8_t count;
  uint8_t selection;
  bool wrap_items = false;;
  static const uint8_t top = 16;
  static const uint8_t left = 0;
  static const uint8_t width = 128;
  static const uint8_t height = 64 - top;
  public:
    Menu(uint8_t count, const char * const *items);
    Menu(uint8_t count, uint8_t selection, const char* const*items);
    Menu &select(uint8_t i);
    void draw(T &display);
    inline Menu &wrap(bool wrapItems = true) {
      wrap_items = wrapItems;
      return *this;
    }
    inline uint8_t size() const {
      return count;
    }
};
