#include "pico/stdlib.h"
#include "display_touch.hpp"
#include <cstdio>
#include <string>

struct Button {
    int16_t x, y, w, h;
    const char* label;
    uint16_t color;

    bool contains(int16_t px, int16_t py) const {
        return (px >= x && px <= x + w && py >= y && py <= y + h);
    }

    void draw() const {
        DisplayDriver::fillRect(x, y, w, h, color);
        DisplayDriver::drawRect(x, y, w, h, Colors::WHITE);
        DisplayDriver::drawString(x + 10, y + (h / 2) - 6, label, Colors::WHITE, color, 2);
    }
};

int main() {
    stdio_init_all();

    // 1. Ініціалізація периферії
    DisplayDriver::init();
    TouchDriver::init();

    // 2. Інтерфейс
    DisplayDriver::fillScreen(Colors::DARKGRAY);

    // Заголовок
    DisplayDriver::fillRect(0, 0, LCD_WIDTH, 30, Colors::BLUE);
    DisplayDriver::drawString(40, 7, "Pico 2 Display & Touch UI", Colors::WHITE, Colors::BLUE, 2);

    // Кнопки
    Button btnIncrement = { 20,  45, 130, 45, "+ Count",  Colors::GREEN };
    Button btnClear     = { 170, 45, 130, 45, "Clear",    Colors::RED };
    Button btnBL        = { 20,  100, 130, 40, "BL Toggle",Colors::MAGENTA };

    btnIncrement.draw();
    btnClear.draw();
    btnBL.draw();

    // Зона для малювання / дотиків
    DisplayDriver::drawRect(15, 150, 290, 80, Colors::CYAN);
    DisplayDriver::drawString(25, 155, "Touch Pad Area (Draw Here):", Colors::CYAN, Colors::DARKGRAY, 1);

    int count = 0;
    bool backlightState = true;
    int16_t tx = 0, ty = 0;
    bool wasPressed = false;

    char strBuf[32];

    while (true) {
        // Оновлюємо статус лічильника
        snprintf(strBuf, sizeof(strBuf), "Count: %-4d", count);
        DisplayDriver::drawString(175, 112, strBuf, Colors::YELLOW, Colors::DARKGRAY, 2);

        // Перевірка тача
        if (TouchDriver::getTouch(tx, ty)) {
            // Відображаємо координати
            snprintf(strBuf, sizeof(strBuf), "X:%03d Y:%03d", tx, ty);
            DisplayDriver::drawString(10, 232, strBuf, Colors::WHITE, Colors::DARKGRAY, 1);

            // Малювання в нижній сенсорній зоні
            if (tx >= 18 && tx <= 300 && ty >= 170 && ty <= 225) {
                DisplayDriver::fillRect(tx - 2, ty - 2, 5, 5, Colors::YELLOW);
            }

            // Обробка кнопок (одинарне натискання)
            if (!wasPressed) {
                if (btnIncrement.contains(tx, ty)) {
                    count++;
                } else if (btnClear.contains(tx, ty)) {
                    count = 0;
                    DisplayDriver::fillRect(16, 168, 288, 60, Colors::DARKGRAY); // Очистити поле малювання
                } else if (btnBL.contains(tx, ty)) {
                    backlightState = !backlightState;
                    DisplayDriver::setBacklight(backlightState);
                }
                wasPressed = true;
            }
        } else {
            wasPressed = false;
        }

        sleep_ms(15);
    }

    return 0;
}