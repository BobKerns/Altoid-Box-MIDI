
#include "DisplayMgr.h"
RawDisplay rawDisplay(-1);


DisplayFn tmpDisplay;
uint32_t tmpDisplay_end = 0;
DisplayFn displayHead = defaultDisplayHead;
DisplayFn displayBody = defaultDisplayBody;

WindowImpl<RawDisplay> display(rawDisplay);

void show(DisplayFn head, DisplayFn body) {
  display.clear();
  display.setTextCursor(0, 0);
  display.setOffset(0, 0);
  display.setFixedFont(ssd1306xled_font8x16);

  head();
  display.setOffset(0, 16);
  body();
}

// Rate limit the temporary displays
static uint32_t last_tmp = 0;
static const uint32_t tmp_rate = 200;

void showFor(uint32_t ms, DisplayFn head, DisplayFn body) {
  auto now = millis();
  if (last_tmp + tmp_rate < now) {
    tmpDisplay_end = ms + now;
    tmpDisplay = [now, head, body]{
      last_tmp = now;
      show(head, body);
      tmpDisplay = NULL;
    };
  }
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
