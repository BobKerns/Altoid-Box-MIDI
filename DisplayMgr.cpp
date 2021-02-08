
#include "DisplayMgr.h"
extern Display rawDisplay;
Display display(-1);


DisplayFn tmpDisplay;
uint32_t tmpDisplay_end = 0;
DisplayFn displayHead = defaultDisplayHead;
DisplayFn displayBody = defaultDisplayBody;

WindowImpl<Display> window(display);

void show(DisplayFn head, DisplayFn body) {
  display.clear();
  display.setTextCursor(0, 0);
  display.setOffset(0, 0);
  display.setFixedFont(ssd1306xled_font8x16);

  head();
  display.setOffset(0, 16);
  body();
}

void showFor(uint32_t ms, DisplayFn head, DisplayFn body) {
  tmpDisplay_end = ms + millis();
  tmpDisplay = [head, body]{
    show(head, body);
    tmpDisplay = NULL;
  };
}

void showHeadFor(uint32_t ms, DisplayFn head) {
  showFor(ms, head, displayBody);
}

void showBodyFor(uint32_t ms, DisplayFn body) {
  showFor(ms, displayHead, body);
}

void doDisplay() {
  if (tmpDisplay_end >  millis()) {
    if (tmpDisplay) {
      tmpDisplay();
      tmpDisplay = NULL;
    }
  } else if (tmpDisplay_end > 0) {
    tmpDisplay_end = 0;
    tmpDisplay = NULL;
    show();
  }
}
