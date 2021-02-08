#include "Window.h"
#include "DisplayMgr.h"
/**
 * Draws pixel on specified position
 * @param x - position X
 * @param y - position Y
 * @note color can be set via setColor()
 */
template<class D>
void WindowImpl<D>::putPixel(lcdint_t x, lcdint_t y) {
  xlate(x, y, [this](auto x, auto y, auto w, auto h){
    _display.putPixel(x, y);
  });
};

/**
 * Draws horizontal or vertical line
 * @param x1 - position X
 * @param y1 - position Y
 * @param y2 - position Y
 * @note color can be set via setColor()
 */
template<class D>
void WindowImpl<D>::drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2) {
  xlate2(x1, y1, x1, y2, [this](auto x1, auto y1, auto x2, auto y2){
    _display.drawVLine(x1, y1, y2);
  });
};

/**
 * Draws horizontal or vertical line
 * @param x1 - position X
 * @param y1 - position Y
 * @param x2 - position X
 * @note color can be set via setColor()
 */
template<class D>
void WindowImpl<D>::drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2) {
  xlate2(x1, y1, x2, y1, [this](auto x1, auto y1, auto x2, auto y2){
    _display.drawHLine(x1, y1, x2);
  });
}
;
/**
 * Fills rectangle area
 * @param x1 - position X
 * @param y1 - position Y
 * @param x2 - position X
 * @param y2 - position Y
 * @note color can be set via setColor()
 */
 template<class D>
void WindowImpl<D>::fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2) {
  xlate2(x1, y1, x2, y2, [this](auto x1, auto y1, auto x2, auto y2){
    _display.fillRect(x1, y1, x2, y2);
  });
};

/**
 * Draws bitmap, located in Flash, on the display
 * The bitmap should be in XBMP format
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in blocks (pixels/8)
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param bitmap - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 */
template<class D>
void WindowImpl<D>::drawXBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) {
  xlateArea(x, y, w, h, [this, bitmap](auto x, auto y, auto w, auto h){
    _display.drawXBitmap(x, y, w, h, bitmap);
  });
};

/**
 * @brief Draws monochrome bitmap in color buffer using color, specified via setColor() method
 * Draws monochrome bitmap in color buffer using color, specified via setColor() method
 * @param x - position X in pixels
 * @param y - position Y in pixels
 * @param w - width in pixels
 * @param h - height in pixels
 * @param bitmap - monochrome bitmap data, located in flash
 *
 * @note There are 2 modes: transparent and non-transparent mode, - and 2 colors available: black and white.
 *       In non-transparent mode, when black color is selected, the monochrome image just inverted.
 *       In transparent mode, those pixels of source monochrome image, which are black, do not overwrite pixels
 *       in the screen buffer.
 */
template<class D>
void WindowImpl<D>::drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) {
  xlateArea(x, y, w, h, [this, bitmap](auto x, auto y, auto w, auto h){
    _display.drawBitmap1(x, y, w, h, bitmap);
  });
};

/**
 * Draws bitmap, located in Flash, on the display
 *
 * @param x - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param w - width of bitmap in pixels
 * @param h - height of bitmap in pixels (must be divided by 8)
 * @param buf - pointer to data, located in Flash: each byte represents 8 vertical pixels.
 * @warning only for monochrome displays
 */
template<class D>
void WindowImpl<D>::gfx_drawMonoBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf) {
  xlateArea(x, y, w, h, [this, buf](auto x, auto y, auto w, auto h){
    _display.gfx_drawMonoBitmap(x, y, w, h, buf);
  });
};

/**
 * @brief Draws 4-bit gray-color bitmap in color buffer.
 * Draws 4-bit gray-color bitmap in color buffer.
 * @param x - position X in pixels
 * @param y - position Y in pixels
 * @param w - width in pixels
 * @param h - height in pixels
 * @param bitmap - 4-bit gray-color bitmap data, located in flash
 */
template<class D>
void WindowImpl<D>::drawBitmap4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) {
  xlateArea(x, y, w, h, [this, bitmap](auto x, auto y, auto w, auto h){
    _display.drawBitmap4(x, y, w, h, bitmap);
  });
};

/**
 * @brief Draws 8-bit color bitmap in color buffer.
 * Draws 8-bit color bitmap in color buffer.
 * @param x - position X in pixels
 * @param y - position Y in pixels
 * @param w - width in pixels
 * @param h - height in pixels
 * @param bitmap - 8-bit color bitmap data, located in flash
 */
template<class D>
void WindowImpl<D>::drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) {
  xlateArea(x, y, w, h, [this, bitmap](auto x, auto y, auto w, auto h){
    _display.drawBitmap8(x, y, w, h, bitmap);
  });
};

/**
 * Draw 16-bit color bitmap, located in Flash, directly to OLED display GDRAM.
 * Each pixel of the bitmap is expected in 5-6-5 format.
 *
 * @param xpos start horizontal position in pixels
 * @param ypos start vertical position in pixels
 * @param w bitmap width in pixels
 * @param h bitmap height in pixels
 * @param bitmap pointer to Flash data, containing 16-bit color bitmap.
 */
template<class D>
void WindowImpl<D>::drawBitmap16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap) {
  xlateArea(xpos, ypos, w, h, [this, bitmap](auto xpos, auto ypos, auto w, auto h){
    _display.drawBitmap16(xpos, ypos, w, h, bitmap);
  });
};

/**
 * Draws bitmap, located in RAM, on the display
 * Each byte represents 8 vertical pixels.
 *
 * ~~~~~~~~~~~~~~~{.cpp}
 * // Draw small rectangle 3x8 at position 10,8
 * uint8_t buffer[3] = { 0xFF, 0x81, 0xFF };
 * display.drawBuffer1(10, 1, 3, 8, buffer);
 * ~~~~~~~~~~~~~~~
 *
 * @param x horizontal position in pixels
 * @param y vertical position in pixels
 * @param w width of bitmap in pixels
 * @param h height of bitmap in pixels (must be divided by 8)
 * @param buffer pointer to data, located in SRAM: each byte represents 8 vertical pixels.
 */
template<class D>
void WindowImpl<D>::drawBuffer1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer) {
  xlateArea(x, y, w, h, [this, buffer](auto x, auto y, auto w, auto h){
    _display.drawBuffer1(x, y, w, h, buffer);
  });
};

/**
 * Draws 4-bit bitmap, located in RAM, on the display
 * Each byte represents two pixels in 4-4 format:
 * refer to GRAY_COLOR4 to understand scheme, being used.
 *
 * @param x horizontal position in pixels
 * @param y vertical position in pixels
 * @param w width of bitmap in pixels
 * @param h height of bitmap in pixels
 * @param buffer pointer to data, located in SRAM.
 */
template<class D>
void WindowImpl<D>::drawBuffer4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer) {
  xlateArea(x, y, w, h, [this, buffer](auto x, auto y, auto w, auto h){
    _display.drawBuffer4(x, y, w, h, buffer);
  });
};

/**
 * Draws 8-bit bitmap, located in RAM, on the display
 * Each byte represents one pixel in 2-2-3 format:
 * refer to RGB_COLOR8 to understand RGB scheme, being used.
 *
 * @param x horizontal position in pixels
 * @param y vertical position in pixels
 * @param w width of bitmap in pixels
 * @param h height of bitmap in pixels
 * @param buffer pointer to data, located in SRAM.
 */
template<class D>
void WindowImpl<D>::drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer) {
  xlateArea(x, y, w, h, [this, buffer](auto x, auto y, auto w, auto h){
    _display.drawBuffer8(x, y, w, h, buffer);
  });
};

/**
 * Draws 16-bit bitmap, located in RAM, on the display
 * Each pixel occupies 2 bytes (5-6-5 format): refer to RGB_COLOR16 to understand RGB scheme, being used.
 *
 * @param xpos horizontal position in pixels
 * @param ypos vertical position in pixels
 * @param w width of bitmap in pixels
 * @param h height of bitmap in pixels
 * @param buffer pointer to data, located in RAM.
 */
template<class D>
void WindowImpl<D>::drawBuffer16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer) {
  xlateArea(xpos, ypos, w, h, [this, buffer](auto xpos, auto ypos, auto w, auto h){
    _display.drawBuffer16(xpos, ypos, w, h, buffer);
  });
};

/**
 * Clears canvas
 */
template<class D>
void WindowImpl<D>::clear() {
  _display.clear();
};

/**
 * Fill screen content with specified color
 *
 * @param color color to fill display with
 */
template<class D>
void WindowImpl<D>::fill(uint16_t color) {
  _display.fill(color);
};

/**
 * Draws single character to canvas
 * @param c - character code to print
 * @returns 0 if char is not printed
 */
template<class D>
uint8_t WindowImpl<D>::printChar(uint8_t c) {
  return _display.printChar(c);
};

/**
 * Writes single character to canvas
 * @param c - character code to print
 */
template<class D>
size_t WindowImpl<D>::write(uint8_t c) {
  return _display.write(c);
};

/**
 * Print text at specified position to canvas
 *
 * @param xpos  position in pixels
 * @param y     position in pixels
 * @param ch    pointer to NULL-terminated string.
 * @param style specific font style to use
 *
 * @note Supports only STYLE_NORMAL and STYLE_BOLD
 */
 using Display = DisplaySSD1306_128x64_I2C;
extern Display rawDisplay;
template<class D>
void WindowImpl<D>::printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style) {
  xlate(xpos, y, [this, ch, style](auto xpos, auto y, auto w, auto h){
    _display.printFixed(xpos, y, ch, style);
  });
};

/**
 * Prints text to screen using size fixed font, scaled by factor value. <br>
 * Factor value 0 gives regular font size (6x8 for example) <br>
 * Factor value 1 gives double font size (12x16 if 6x8 font is used) <br>
 * Factor value 2 gives fourth font size (24x32 if 6x8 font is used) <br>
 * Factor value 3 gives eighth font size (48x64 if 6x8 font is used) <br>
 * @param xpos - horizontal position in pixels
 * @param y - vertical position in pixels
 * @param ch - NULL-terminated string to print
 * @param style - font style (EFontStyle), normal by default.
 * @param factor - 0, 1, 2, 3.
 * @returns number of chars in string
 * @see ssd1306_setFixedFont
 * @warning ssd1306_printFixed2x() can output chars at fixed y positions: 0, 8, 16, 24, 32, etc.
 *          If you specify [10,18], ssd1306_printFixed2x() will output text starting at [10,16] position.
 * @warning Be careful with you flash space! Do not mix too many different functions in single sketch.
 *          ssd1306_printFixedN() uses much flash: ~474 bytes, ssd1306_printFixed() needs 388 bytes.
 *          Placing both of these functions to your sketch will consume almost 1KiB.
 */
template<class D>
void WindowImpl<D>::printFixedN(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style, uint8_t factor) {
  xlate(xpos, y, [this, ch, style, factor](auto xpos, auto y, auto w, auto h){
    _display.printFixedN(xpos, y, ch, style, factor);
  });
};
