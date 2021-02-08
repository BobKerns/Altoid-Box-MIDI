#pragma once
#include "lcdgfx.h"
#undef min
#undef max

#include <functional>

inline lcdint_t min(lcdint_t a, lcdint_t b) {
  return a < b ? a : b;
}
inline lcdint_t max(lcdint_t a, lcdint_t b) {
  return a > b ? a : b;
}

class Window {
public:
    /**
     * Sets offset
     * @param ox - X offset in pixels
     * @param oy - Y offset in pixels
     */
    void setOffset(lcdint_t ox, lcdint_t oy) { m_offset_x = ox; m_offset_y = oy; };

    /**
     * Returns right-bottom point of the canvas in offset terms.
     * If offset is (0,0), then offsetEnd() will return (width-1,height-1).
     */
    const NanoPoint offsetEnd() const
    {
        return (NanoPoint){ (lcdint_t)(m_offset_x + m_w-1), (lcdint_t)(m_offset_y + m_h-1) };
    }

    /**
     * Returns rectangle area, covered by canvas in offset terms.
     * If offset is (0,0), then rect() will return ((0,0),(width-1,height-1))
     */
    const NanoRect rect() const
    {
        return { (NanoPoint){0,0}, offsetEnd() };
    }

    /**
     * Returns width of the display in pixels.
     */
    lcduint_t width() { return m_w; }

    /**
     * Returns height of the display in pixels.
     */
    lcduint_t height() { return m_h; }

    /**
     * Swaps width and height dimensions
     */
    void swapDimensions()
    {
        lcduint_t t = m_w;
        m_w = m_h;
        m_h = t;
    }

    /**
     * Sets color
     * @param color - color to set (refer to RGB_COLOR8, RGB_COLOR16 definition)
     */
    void setColor(uint16_t color) { m_color = color; };

    /**
     * Returns currently set color
     */
    uint16_t getColor() { return m_color; };

    /**
     * Sets background color.
     * @param color color to set (refer to RGB_COLOR8, RGB_COLOR16 definition)
     */
    void setBackground(uint16_t color) { m_bgColor = color; }

    /**
     * Changes foreground and background colors
     */
    void invertColors() { uint16_t color = m_color; m_color = m_bgColor; m_bgColor = color; }

    /**
     * Sets new font to use with print functions.
     * If multiple oled displays are used in single application,
     * this method allows to use different fonts for different
     * displays.
     *
     * @param font reference to font object (NanoFont)
     */
    void setFont( NanoFont &font ) { m_font = &font; }

    /**
     * Sets spacing for currently active font.
     * The same can be done via `getFont().setSpacing()`, but this method is
     * more safe since it checks if font is set.
     * @param spacing spacing in pixels
     */
    void setFontSpacing(uint8_t spacing)
    {
        if (m_font) m_font->setSpacing(spacing);
    }

    /**
     * Returns reference to NanoFont object, currently used by Display
     */
    NanoFont &getFont() { return *m_font; }

    /**
     * Sets new font to use with print functions.
     * If multiple oled displays are used in single application,
     * this method can cause conflicts.
     *
     * @warning use this method only if single display is used in project
     *
     * @param progmemFont pointer to font data in flash (refer to NanoFont::loadFixedFont)
     */
    void setFixedFont( const uint8_t *progmemFont )
    {
        g_canvas_font.loadFixedFont( progmemFont );
        setFont( g_canvas_font );
    }

    /**
     * Sets new font to use with print functions.
     * If multiple oled displays are used in single application,
     * this method can cause conflicts.
     *
     * @warning use this method only if single display is used in project
     *
     * @param progmemFont pointer to font data in flash (refer to NanoFont::loadFixedFont)
     * @param secondaryFont pointer to font data in flash (refer to NanoFont::loadSecondaryFont)
     */
    void setFixedFont( const uint8_t *progmemFont, const uint8_t *secondaryFont )
    {
        g_canvas_font.loadFixedFont( progmemFont );
        if ( secondaryFont )
        {
            g_canvas_font.loadSecondaryFont( secondaryFont );
        }
        setFont( g_canvas_font );
    }

    /**
     * Sets new font to use with print functions.
     * If multiple oled displays are used in single application,
     * this method can cause conflicts.
     *
     * @warning use this method only if single display is used in project
     *
     * @param progmemFont pointer to font data in flash (refer to NanoFont::loadFreeFont)
     * @param secondaryFont pointer to font data in flash (refer to NanoFont::loadSecondaryFont)
     */
    void setFreeFont( const uint8_t *progmemFont, const uint8_t *secondaryFont = nullptr )
    {
        g_canvas_font.loadFreeFont( progmemFont );
        if ( secondaryFont )
        {
            g_canvas_font.loadSecondaryFont( secondaryFont );
        }
        setFont( g_canvas_font );
    }

    /**
     * Function sets text cursor position for write() functions
     *
     * @param x horizontal position in pixels
     * @param y vertical position in pixels
     */
    void setTextCursor( lcdint_t x, lcdint_t y )
    {
        m_cursorX = x;
        m_cursorY = y;
    }

protected:
    lcduint_t m_offset_x = 0;
    lcduint_t m_offset_y = 0;
    lcduint_t m_w = 0;    ///< width of NanoCanvas area in pixels
    lcduint_t m_h = 0;    ///< height of NanoCanvas area in pixels
    lcduint_t m_p = 0;    ///< number of bits, used by width value: 3 equals to 8 pixels width
    lcdint_t  m_cursorX = 0;  ///< current X cursor position for text output
    lcdint_t  m_cursorY = 0;  ///< current Y cursor position for text output
    uint8_t   m_textMode = 0; ///< Flags for current NanoCanvas mode
    EFontStyle   m_fontStyle = STYLE_NORMAL; ///< currently active font style
    uint16_t  m_color = 0xFFFF;    ///< current foreground color
    uint16_t  m_bgColor = 0x0000;  ///< current background color
    NanoFont *m_font = nullptr; ///< currently set font

    void xlateArea(lcdint_t x, lcdint_t y, lcdint_t w, lcdint_t h, std::function<void (lcdint_t x, lcdint_t y, lcdint_t w, lcdint_t h)> fn) {
      fn(max(m_offset_x, x + m_offset_x), max(m_offset_y, y + m_offset_y), max(0, min(w, m_w - x)), max(0, min(h, m_h - y)));
    }
    void xlate(lcdint_t x, lcdint_t y, std::function<void (lcdint_t x, lcdint_t y, lcdint_t w, lcdint_t h)> fn) {
      fn(max(m_offset_x, x + m_offset_x), max(m_offset_y, y + m_offset_y), max(0, m_w - x), max(0, m_h - y));
    }
    void xlate2(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2, std::function<void (lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2)> fn) {
      fn(max(m_offset_x, x1 + m_offset_x), max(m_offset_y, y1 + m_offset_y), max(m_offset_x, x2 + m_offset_x), max(m_offset_y, y2 + m_offset_y));
    }

public:
    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @note color can be set via setColor()
     */
    virtual void putPixel(lcdint_t x, lcdint_t y) __attribute__ ((noinline)) = 0;

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    virtual void drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2) = 0;

    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @note color can be set via setColor()
     */
    virtual void drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2) = 0;

    /**
     * Fills rectangle area
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    virtual void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2) __attribute__((noinline)) = 0;

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
    virtual void drawXBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) = 0;

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
    virtual void drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) __attribute__ ((noinline)) = 0;

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
    virtual void gfx_drawMonoBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf) = 0;

    /**
     * @brief Draws 4-bit gray-color bitmap in color buffer.
     * Draws 4-bit gray-color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 4-bit gray-color bitmap data, located in flash
     */
    virtual void drawBitmap4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) = 0;

    /**
     * @brief Draws 8-bit color bitmap in color buffer.
     * Draws 8-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 8-bit color bitmap data, located in flash
     */
    virtual void drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap) = 0;

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
    virtual void drawBitmap16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap) = 0;

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
    virtual void drawBuffer1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer) __attribute__ ((noinline)) = 0;

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
    virtual void drawBuffer4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer) __attribute__ ((noinline)) = 0;

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
    virtual void drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer) __attribute__ ((noinline)) = 0;

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
    virtual void drawBuffer16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer) __attribute__ ((noinline)) = 0;

    /**
     * Clears canvas
     */
    virtual void clear() = 0;

    /**
     * Fill screen content with specified color
     *
     * @param color color to fill display with
     */
    virtual void fill(uint16_t color) = 0;

    /**
     * Draws single character to canvas
     * @param c - character code to print
     * @returns 0 if char is not printed
     */
    virtual uint8_t printChar(uint8_t c) = 0;

    /**
     * Writes single character to canvas
     * @param c - character code to print
     */
    virtual size_t write(uint8_t c) __attribute__ ((noinline)) = 0;

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
    virtual void printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style = STYLE_NORMAL) __attribute__ ((noinline)) = 0;
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
    virtual void printFixedN(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style, uint8_t factor) __attribute__ ((noinline)) = 0;

protected:
};

template<class D>
class WindowImpl: public Window {
  protected:
    D &_display;
  public:
    void begin() { _display.begin(); }
    void end() { _display.end(); }
    WindowImpl(D &display): _display(display) {}

    operator D&() { return _display; }
     
    /**
     * Draws pixel on specified position
     * @param x - position X
     * @param y - position Y
     * @note color can be set via setColor()
     */
    void putPixel(lcdint_t x, lcdint_t y);
    
    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void drawVLine(lcdint_t x1, lcdint_t y1, lcdint_t y2);
    
    /**
     * Draws horizontal or vertical line
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @note color can be set via setColor()
     */
    void drawHLine(lcdint_t x1, lcdint_t y1, lcdint_t x2);

    /*
     * Fills rectangle area
     * @param x1 - position X
     * @param y1 - position Y
     * @param x2 - position X
     * @param y2 - position Y
     * @note color can be set via setColor()
     */
    void fillRect(lcdint_t x1, lcdint_t y1, lcdint_t x2, lcdint_t y2);
    
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
    void drawXBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);
    
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
    void drawBitmap1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);
    
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
    void gfx_drawMonoBitmap(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buf);

    /**
     * @brief Draws 4-bit gray-color bitmap in color buffer.
     * Draws 4-bit gray-color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 4-bit gray-color bitmap data, located in flash
     */
    void drawBitmap4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);
    
    /**
     * @brief Draws 8-bit color bitmap in color buffer.
     * Draws 8-bit color bitmap in color buffer.
     * @param x - position X in pixels
     * @param y - position Y in pixels
     * @param w - width in pixels
     * @param h - height in pixels
     * @param bitmap - 8-bit color bitmap data, located in flash
     */
    void drawBitmap8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *bitmap);
    
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
    void drawBitmap16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *bitmap);
    
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
    void drawBuffer1(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer);
    
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
    void drawBuffer4(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer);
    
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
    void drawBuffer8(lcdint_t x, lcdint_t y, lcduint_t w, lcduint_t h, const uint8_t *buffer);
    
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
    void drawBuffer16(lcdint_t xpos, lcdint_t ypos, lcduint_t w, lcduint_t h, const uint8_t *buffer);
    
    /**
     * Clears canvas
     */
    void clear();
    
    /**
     * Fill screen content with specified color
     *
     * @param color color to fill display with
     */
    void fill(uint16_t color);
    
    /**
     * Draws single character to canvas
     * @param c - character code to print
     * @returns 0 if char is not printed
     */
    uint8_t printChar(uint8_t c);
    
    /**
     * Writes single character to canvas
     * @param c - character code to print
     */
    size_t write(uint8_t c);
    
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
    void printFixed(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style);
    
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
    void printFixedN(lcdint_t xpos, lcdint_t y, const char *ch, EFontStyle style, uint8_t factor);
 };
