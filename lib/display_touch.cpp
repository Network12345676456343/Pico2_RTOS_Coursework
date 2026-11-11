#include "display_touch.hpp"
#include "hardware/gpio.h"
#include <algorithm>

// Базовий растровий шрифт 5x7
static const uint8_t font5x7[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, // [space]
    0x00, 0x00, 0x5F, 0x00, 0x00, // !
    0x00, 0x07, 0x00, 0x07, 0x00, // "
    0x14, 0x7F, 0x14, 0x7F, 0x14, // #
    0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
    0x23, 0x13, 0x08, 0x64, 0x62, // %
    0x36, 0x49, 0x55, 0x22, 0x50, // &
    0x00, 0x05, 0x03, 0x00, 0x00, // '
    0x00, 0x1C, 0x22, 0x41, 0x00, // (
    0x00, 0x41, 0x22, 0x1C, 0x00, // )
    0x14, 0x08, 0x3E, 0x08, 0x14, // *
    0x08, 0x08, 0x3E, 0x08, 0x08, // +
    0x00, 0x50, 0x30, 0x00, 0x00, // ,
    0x08, 0x08, 0x08, 0x08, 0x08, // -
    0x00, 0x60, 0x60, 0x00, 0x00, // .
    0x20, 0x10, 0x08, 0x04, 0x02, // /
    0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
    0x00, 0x42, 0x7F, 0x40, 0x00, // 1
    0x42, 0x61, 0x51, 0x49, 0x46, // 2
    0x21, 0x41, 0x45, 0x4B, 0x31, // 3
    0x18, 0x14, 0x12, 0x7F, 0x10, // 4
    0x27, 0x45, 0x45, 0x45, 0x39, // 5
    0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
    0x01, 0x71, 0x09, 0x05, 0x03, // 7
    0x36, 0x49, 0x49, 0x49, 0x36, // 8
    0x06, 0x49, 0x49, 0x29, 0x1E, // 9
    0x00, 0x36, 0x36, 0x00, 0x00, // :
    0x00, 0x56, 0x36, 0x00, 0x00, // ;
    0x08, 0x14, 0x22, 0x41, 0x00, // <
    0x14, 0x14, 0x14, 0x14, 0x14, // =
    0x00, 0x41, 0x22, 0x14, 0x08, // >
    0x02, 0x01, 0x51, 0x09, 0x06, // ?
    0x32, 0x49, 0x79, 0x41, 0x3E, // @
    0x7E, 0x11, 0x11, 0x11, 0x7E, // A
    0x7F, 0x49, 0x49, 0x49, 0x36, // B
    0x3E, 0x41, 0x41, 0x41, 0x22, // C
    0x7F, 0x41, 0x41, 0x22, 0x1C, // D
    0x7F, 0x49, 0x49, 0x49, 0x41, // E
    0x7F, 0x09, 0x09, 0x09, 0x01, // F
    0x3E, 0x41, 0x49, 0x49, 0x7A, // G
    0x7F, 0x08, 0x08, 0x08, 0x7F, // H
    0x00, 0x41, 0x7F, 0x41, 0x00, // I
    0x20, 0x40, 0x41, 0x3F, 0x01, // J
    0x7F, 0x08, 0x14, 0x22, 0x41, // K
    0x7F, 0x40, 0x40, 0x40, 0x40, // L
    0x7F, 0x02, 0x0C, 0x02, 0x7F, // M
    0x7F, 0x04, 0x08, 0x10, 0x7F, // N
    0x3E, 0x41, 0x41, 0x41, 0x3E, // O
    0x7F, 0x09, 0x09, 0x09, 0x06, // P
    0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
    0x7F, 0x09, 0x19, 0x29, 0x46, // R
    0x46, 0x49, 0x49, 0x49, 0x31, // S
    0x01, 0x01, 0x7F, 0x01, 0x01, // T
    0x3F, 0x40, 0x40, 0x40, 0x3F, // U
    0x1F, 0x20, 0x40, 0x20, 0x1F, // V
    0x3F, 0x40, 0x38, 0x40, 0x3F, // W
    0x63, 0x14, 0x08, 0x14, 0x63, // X
    0x07, 0x08, 0x70, 0x08, 0x07, // Y
    0x61, 0x51, 0x49, 0x45, 0x43, // Z
    0x00, 0x7F, 0x41, 0x41, 0x00, // [
    0x02, 0x04, 0x08, 0x10, 0x20, // backslash
    0x00, 0x41, 0x41, 0x7F, 0x00, // ]
    0x04, 0x02, 0x01, 0x02, 0x04, // ^
    0x40, 0x40, 0x40, 0x40, 0x40, // _
    0x00, 0x01, 0x02, 0x04, 0x00, // `
    0x20, 0x54, 0x54, 0x54, 0x78, // a
    0x7F, 0x48, 0x44, 0x44, 0x38, // b
    0x38, 0x44, 0x44, 0x44, 0x20, // c
    0x38, 0x44, 0x44, 0x48, 0x7F, // d
    0x38, 0x54, 0x54, 0x54, 0x18, // e
    0x08, 0x7E, 0x09, 0x01, 0x02, // f
    0x0C, 0x52, 0x52, 0x52, 0x3E, // g
    0x7F, 0x08, 0x04, 0x04, 0x78, // h
    0x00, 0x44, 0x7D, 0x40, 0x00, // i
    0x20, 0x40, 0x44, 0x3D, 0x00, // j
    0x7F, 0x10, 0x28, 0x44, 0x00, // k
    0x00, 0x41, 0x7F, 0x40, 0x00, // l
    0x7C, 0x04, 0x18, 0x04, 0x78, // m
    0x7C, 0x08, 0x04, 0x04, 0x78, // n
    0x38, 0x44, 0x44, 0x44, 0x38, // o
    0x7C, 0x14, 0x14, 0x14, 0x08, // p
    0x08, 0x14, 0x14, 0x18, 0x7C, // q
    0x7C, 0x08, 0x04, 0x04, 0x08, // r
    0x48, 0x54, 0x54, 0x54, 0x20, // s
    0x04, 0x3F, 0x44, 0x40, 0x20, // t
    0x3C, 0x40, 0x40, 0x20, 0x7C, // u
    0x1C, 0x20, 0x40, 0x20, 0x1C, // v
    0x3C, 0x40, 0x30, 0x40, 0x3C, // w
    0x44, 0x28, 0x10, 0x28, 0x44, // x
    0x0C, 0x50, 0x50, 0x50, 0x3C, // y
    0x44, 0x64, 0x54, 0x4C, 0x44  // z
};

// ==================== ДИСПЛЕЙ (ST7789) ====================

void DisplayDriver::writeCommand(uint8_t cmd) {
    gpio_put(BoardPins::PIN_LCD_DC, 0);
    gpio_put(BoardPins::PIN_LCD_CS, 0);
    spi_write_blocking(spi0, &cmd, 1);
    gpio_put(BoardPins::PIN_LCD_CS, 1);
}

void DisplayDriver::writeData(uint8_t data) {
    gpio_put(BoardPins::PIN_LCD_DC, 1);
    gpio_put(BoardPins::PIN_LCD_CS, 0);
    spi_write_blocking(spi0, &data, 1);
    gpio_put(BoardPins::PIN_LCD_CS, 1);
}

void DisplayDriver::writeData16(uint16_t data) {
    uint8_t d[2] = { static_cast<uint8_t>(data >> 8), static_cast<uint8_t>(data & 0xFF) };
    gpio_put(BoardPins::PIN_LCD_DC, 1);
    gpio_put(BoardPins::PIN_LCD_CS, 0);
    spi_write_blocking(spi0, d, 2);
    gpio_put(BoardPins::PIN_LCD_CS, 1);
}

void DisplayDriver::setAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    writeCommand(0x2A); // CASET
    writeData(x0 >> 8);
    writeData(x0 & 0xFF);
    writeData(x1 >> 8);
    writeData(x1 & 0xFF);

    writeCommand(0x2B); // RASET
    writeData(y0 >> 8);
    writeData(y0 & 0xFF);
    writeData(y1 >> 8);
    writeData(y1 & 0xFF);

    writeCommand(0x2C); // RAMWR
}

void DisplayDriver::init() {
    spi_init(spi0, 40 * 1000 * 1000); // 40 МГц
    gpio_set_function(BoardPins::PIN_LCD_SCLK, GPIO_FUNC_SPI);
    gpio_set_function(BoardPins::PIN_LCD_MOSI, GPIO_FUNC_SPI);

    gpio_init(BoardPins::PIN_LCD_DC);
    gpio_set_dir(BoardPins::PIN_LCD_DC, GPIO_OUT);

    gpio_init(BoardPins::PIN_LCD_CS);
    gpio_set_dir(BoardPins::PIN_LCD_CS, GPIO_OUT);
    gpio_put(BoardPins::PIN_LCD_CS, 1);

    gpio_init(BoardPins::PIN_LCD_BL);
    gpio_set_dir(BoardPins::PIN_LCD_BL, GPIO_OUT);
    setBacklight(false);

    sleep_ms(120);

    writeCommand(0x01); // SWRESET
    sleep_ms(150);

    writeCommand(0x11); // SLPOUT
    sleep_ms(120);

    writeCommand(0x36); // MADCTL
    writeData(0x70);    // Альбомна орієнтація

    writeCommand(0x3A); // COLMOD
    writeData(0x55);    // 16-bit RGB565

    writeCommand(0xB2); // PORCTRL
    writeData(0x0C);
    writeData(0x0C);
    writeData(0x00);
    writeData(0x33);
    writeData(0x33);

    writeCommand(0xB7); // GCTRL
    writeData(0x35);

    writeCommand(0xBB); // VCOMS
    writeData(0x19);

    writeCommand(0xC0); // LCMCTRL
    writeData(0x2C);

    writeCommand(0xC2); // VDVVRHEN
    writeData(0x01);

    writeCommand(0xC3); // VRHS
    writeData(0x12);

    writeCommand(0xC4); // VDVS
    writeData(0x20);

    writeCommand(0xC6); // FRCTRL2
    writeData(0x0F);

    writeCommand(0xD0); // PWCTRL1
    writeData(0xA4);
    writeData(0xA1);

    writeCommand(0x21); // INVON
    writeCommand(0x29); // DISPON
    sleep_ms(50);

    fillScreen(Colors::BLACK);
    setBacklight(true);
}

void DisplayDriver::setBacklight(bool on) {
    gpio_put(BoardPins::PIN_LCD_BL, on ? 1 : 0);
}

void DisplayDriver::fillScreen(uint16_t color) {
    fillRect(0, 0, LCD_WIDTH, LCD_HEIGHT, color);
}

void DisplayDriver::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if (x < 0 || x >= LCD_WIDTH || y < 0 || y >= LCD_HEIGHT) return;
    setAddressWindow(x, y, x, y);
    writeData16(color);
}

void DisplayDriver::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    if (x >= LCD_WIDTH || y >= LCD_HEIGHT || w <= 0 || h <= 0) return;
    if (x + w > LCD_WIDTH)  w = LCD_WIDTH - x;
    if (y + h > LCD_HEIGHT) h = LCD_HEIGHT - y;

    setAddressWindow(x, y, x + w - 1, y + h - 1);

    gpio_put(BoardPins::PIN_LCD_DC, 1);
    gpio_put(BoardPins::PIN_LCD_CS, 0);

    uint8_t hi = color >> 8;
    uint8_t lo = color & 0xFF;
    constexpr size_t BUF_PIXELS = 64;
    uint8_t buffer[BUF_PIXELS * 2];
    for (size_t i = 0; i < BUF_PIXELS; ++i) {
        buffer[i * 2]     = hi;
        buffer[i * 2 + 1] = lo;
    }

    size_t total_pixels = static_cast<size_t>(w) * h;
    while (total_pixels > 0) {
        size_t chunk = (total_pixels > BUF_PIXELS) ? BUF_PIXELS : total_pixels;
        spi_write_blocking(spi0, buffer, chunk * 2);
        total_pixels -= chunk;
    }

    gpio_put(BoardPins::PIN_LCD_CS, 1);
}

void DisplayDriver::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    drawFastHLine(x, y, w, color);
    drawFastHLine(x, y + h - 1, w, color);
    drawFastVLine(x, y, h, color);
    drawFastVLine(x + w - 1, y, h, color);
}

void DisplayDriver::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    fillRect(x, y, w, 1, color);
}

void DisplayDriver::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    fillRect(x, y, 1, h, color);
}

void DisplayDriver::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    int16_t a, b, y, last;
    if (y0 > y1) { std::swap(y0, y1); std::swap(x0, x1); }
    if (y1 > y2) { std::swap(y2, y1); std::swap(x2, x1); }
    if (y0 > y1) { std::swap(y0, y1); std::swap(x0, x1); }

    if (y0 == y2) {
        a = b = x0;
        if (x1 < a) a = x1; else if (x1 > b) b = x1;
        if (x2 < a) a = x2; else if (x2 > b) b = x2;
        drawFastHLine(a, y0, b - a + 1, color);
        return;
    }

    int16_t dx01 = x1 - x0, dy01 = y1 - y0;
    int16_t dx02 = x2 - x0, dy02 = y2 - y0;
    int16_t dx12 = x2 - x1, dy12 = y2 - y1;
    int32_t sa = 0, sb = 0;

    last = (y1 == y2) ? y1 : y1 - 1;

    for (y = y0; y <= last; y++) {
        a = x0 + sa / dy01;
        b = x0 + sb / dy02;
        sa += dx01;
        sb += dx02;
        if (a > b) std::swap(a, b);
        drawFastHLine(a, y, b - a + 1, color);
    }

    sa = static_cast<int32_t>(dx12) * (y - y1);
    sb = static_cast<int32_t>(dx02) * (y - y0);
    for (; y <= y2; y++) {
        a = x1 + sa / dy12;
        b = x0 + sb / dy02;
        sa += dx12;
        sb += dx02;
        if (a > b) std::swap(a, b);
        drawFastHLine(a, y, b - a + 1, color);
    }
}

void DisplayDriver::drawChar(int16_t x, int16_t y, char c, uint16_t color, uint16_t bg, uint8_t size) {
    if (c < 32 || c > 126) c = '?';
    const uint8_t* charData = &font5x7[(c - 32) * 5];

    for (int8_t i = 0; i < 5; ++i) {
        uint8_t line = charData[i];
        for (int8_t j = 0; j < 8; ++j) {
            uint16_t drawColor = (line & 0x01) ? color : bg;
            if (size == 1) {
                drawPixel(x + i, y + j, drawColor);
            } else {
                fillRect(x + i * size, y + j * size, size, size, drawColor);
            }
            line >>= 1;
        }
    }
    if (size == 1) {
        drawFastVLine(x + 5, y, 8, bg);
    } else {
        fillRect(x + 5 * size, y, size, 8 * size, bg);
    }
}

void DisplayDriver::drawString(int16_t x, int16_t y, const std::string& str, uint16_t color, uint16_t bg, uint8_t size) {
    int16_t curX = x;
    for (char c : str) {
        if (c == '\n') {
            curX = x;
            y += 8 * size;
            continue;
        }
        drawChar(curX, y, c, color, bg, size);
        curX += 6 * size;
    }
}

// ==================== СЕНСОР (FT6x36 / FT5x06) ====================

void TouchDriver::init() {
    i2c_init(i2c0, 400 * 1000); // 400 кГц
    gpio_set_function(BoardPins::PIN_TP_SDA, GPIO_FUNC_I2C);
    gpio_set_function(BoardPins::PIN_TP_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(BoardPins::PIN_TP_SDA);
    gpio_pull_up(BoardPins::PIN_TP_SCL);

    gpio_init(BoardPins::PIN_TP_RST);
    gpio_set_dir(BoardPins::PIN_TP_RST, GPIO_OUT);

    gpio_init(BoardPins::PIN_TP_INT);
    gpio_set_dir(BoardPins::PIN_TP_INT, GPIO_IN);

    // Скидання контролера тачскріна
    gpio_put(BoardPins::PIN_TP_RST, 0);
    sleep_ms(20);
    gpio_put(BoardPins::PIN_TP_RST, 1);
    sleep_ms(150);
}

bool TouchDriver::getTouch(int16_t &x, int16_t &y) {
    constexpr uint8_t I2C_ADDR = 0x38;
    uint8_t reg = 0x02; // TD_STATUS
    uint8_t data[5] = {0};

    if (i2c_write_blocking(i2c0, I2C_ADDR, &reg, 1, true) < 0) return false;
    if (i2c_read_blocking(i2c0, I2C_ADDR, data, 5, false) < 0) return false;

    uint8_t touches = data[0] & 0x0F;
    if (touches == 0 || touches > 2) return false;

    uint16_t raw_x = ((data[1] & 0x0F) << 8) | data[2];
    uint16_t raw_y = ((data[3] & 0x0F) << 8) | data[4];

    // Трансформація координат
    x = static_cast<int16_t>(raw_y);
    y = static_cast<int16_t>(240 - raw_x);

    if (x < 0) x = 0;
    if (x >= LCD_WIDTH) x = LCD_WIDTH - 1;
    if (y < 0) y = 0;
    if (y >= LCD_HEIGHT) y = LCD_HEIGHT - 1;

    return true;
}