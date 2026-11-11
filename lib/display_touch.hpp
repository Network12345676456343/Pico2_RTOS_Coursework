#pragma once

#include <cstdint>
#include <string>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"

// Піни згідно зі схемою Elecrow Pico 2
namespace BoardPins {
    // SPI Дисплей (ST7789)
    inline constexpr uint PIN_LCD_SCLK = 6;
    inline constexpr uint PIN_LCD_MOSI = 7;
    inline constexpr uint PIN_LCD_DC   = 16;
    inline constexpr uint PIN_LCD_CS   = 17;
    inline constexpr uint PIN_LCD_BL   = 0;   // Підсвічування

    // I2C Сенсор (FT6x36 / FT5x06)
    inline constexpr uint PIN_TP_SDA   = 4;
    inline constexpr uint PIN_TP_SCL   = 5;
    inline constexpr uint PIN_TP_RST   = 24;
    inline constexpr uint PIN_TP_INT   = 25;
}

// 16-бітні RGB565 кольори
namespace Colors {
    inline constexpr uint16_t BLACK   = 0x0000;
    inline constexpr uint16_t WHITE   = 0xFFFF;
    inline constexpr uint16_t RED     = 0xF800;
    inline constexpr uint16_t GREEN   = 0x07E0;
    inline constexpr uint16_t BLUE    = 0x001F;
    inline constexpr uint16_t CYAN    = 0x07FF;
    inline constexpr uint16_t MAGENTA = 0xF81F;
    inline constexpr uint16_t YELLOW  = 0xFFE0;
    inline constexpr uint16_t GRAY    = 0x5AEB;
    inline constexpr uint16_t DARKGRAY= 0x2104;
}

// Розміри екрана (альбомна орієнтація)
inline constexpr int16_t LCD_WIDTH  = 320;
inline constexpr int16_t LCD_HEIGHT = 240;

class DisplayDriver {
public:
    static void init();
    static void setBacklight(bool on);
    static void fillScreen(uint16_t color);
    static void drawPixel(int16_t x, int16_t y, uint16_t color);
    static void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    static void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    static void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    static void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    static void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    static void drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size = 1);
    static void drawString(int16_t x, int16_t y, const std::string& str, uint16_t color, uint16_t bg, uint8_t size = 1);

private:
    static void writeCommand(uint8_t cmd);
    static void writeData(uint8_t data);
    static void writeData16(uint16_t data);
    static void setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
};

class TouchDriver {
public:
    static void init();
    static bool getTouch(int16_t &x, int16_t &y);
};