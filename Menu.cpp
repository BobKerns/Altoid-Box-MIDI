#include "Menu.h"

template<class T>
Menu<T>::Menu(uint8_t count, const char * const *items): Menu(count, 0, items) {}

template<class T>
Menu<T>::Menu(uint8_t count, uint8_t selection, const char* const*items): count(count), items(items), selection(selection) {
}

template<class T>
Menu<T> &Menu<T>::select(uint8_t i) {
  if (i < count) {
    selection = i;
  } else {
    selection = count - 1;
  }
  return *this;
}

template<class T>
void Menu<T>::draw(T &display) {
  auto first = wrap_items 
      ? selection - 1
      : selection == 0 
          ? 0 
          : selection == count - 1 
            ? count < 4 
              ? 0 
              : selection - 2
           : selection - 1;
  auto selected = wrap_items
      ? count > 1 
        ? 1 : 0
      : selection == 0 
        ? 0 
        : selection == count - 1 
          ? 2 : 1;
  auto show = [this, &display, first, selected](uint8_t i) {
    auto idx = first + i;
    auto item = idx < 0
        ? items[count + idx]
        : idx >= count
          ? items[idx - count]
          : items[idx];
    if (i == selected) {
      display.printFixed(left, top + i * 16, "> ", STYLE_NORMAL);
      display.printFixed(left + 12, top + i * 16, item, STYLE_NORMAL);
    } else {
     display.printFixed(left, top + i * 16, item, STYLE_NORMAL);
    }
  };
  show(0);
  show(1);
  show(2);
}
