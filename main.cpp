#include "pico/stdlib.h"
#include "display_touch.hpp"
#include "rgb_pot.hpp"
#include <cstdio>

struct TouchButton {
    int16_t x, y, w, h;
    const char* label;
    uint16_t bgColor;
    uint16_t textColor;

    bool contains(int16_t px, int16_t py) const {
        return (px >= x && px <= x + w && py >= y && py <= y + h);
    }

    void draw(bool selected = false) const {
        DisplayDriver::fillRect(x, y, w, h, bgColor);
        // Відображаємо білу рамку навколо обраної кнопки
        DisplayDriver::drawRect(x, y, w, h, selected ? Colors::WHITE : Colors::DARKGRAY);
        if (selected) {
            DisplayDriver::drawRect(x + 1, y + 1, w - 2, h - 2, Colors::WHITE);
        }
        DisplayDriver::drawString(x + 20, y + (h / 2) - 8, label, textColor, bgColor, 2);
    }
};

int main() {
    stdio_init_all();

    // 1. Ініціалізація периферії
    DisplayDriver::init();
    TouchDriver::init();
    LedController::init();

    DisplayDriver::fillScreen(Colors::DARKGRAY);

    // Заголовок
    DisplayDriver::fillRect(0, 0, LCD_WIDTH, 35, Colors::BLUE);
    DisplayDriver::drawString(25, 9, "TOUCH LED CONTROLLER", Colors::WHITE, Colors::BLUE, 2);

    // 4 сенсорні кнопки (під піни IO18, IO20, IO19)
    TouchButton btnRed    = { 20,  50, 280, 40, "1. RED LED   (IO18)", Colors::RED,    Colors::WHITE };
    TouchButton btnYellow = { 20,  98, 280, 40, "2. YELLOW LED(IO20)", Colors::YELLOW, Colors::BLACK };
    TouchButton btnGreen  = { 20, 146, 280, 40, "3. GREEN LED (IO19)", Colors::GREEN,  Colors::BLACK };
    TouchButton btnOff    = { 20, 194, 280, 38, "4. ALL OFF",          Colors::BLACK,  Colors::WHITE };

    btnRed.draw(false);
    btnYellow.draw(false);
    btnGreen.draw(false);
    btnOff.draw(true); // За замовчуванням усі діоди вимкнені

    int activeLed = 0; // 0 = off, 1 = red, 2 = yellow, 3 = green
    int16_t tx = 0, ty = 0;
    bool wasPressed = false;

    while (true) {
        bool isTouched = TouchDriver::getTouch(tx, ty);

        if (isTouched && !wasPressed) {
            int newLed = activeLed;

            if (btnRed.contains(tx, ty)) {
                newLed = 1;
            } else if (btnYellow.contains(tx, ty)) {
                newLed = 2;
            } else if (btnGreen.contains(tx, ty)) {
                newLed = 3;
            } else if (btnOff.contains(tx, ty)) {
                newLed = 0;
            }

            if (newLed != activeLed) {
                activeLed = newLed;
                LedController::setOnly(activeLed);

                // Оновлення підсвічування кнопок на дисплеї
                btnRed.draw(activeLed == 1);
                btnYellow.draw(activeLed == 2);
                btnGreen.draw(activeLed == 3);
                btnOff.draw(activeLed == 0);
            }

            wasPressed = true;
        } else if (!isTouched) {
            wasPressed = false;
        }

        sleep_ms(15);
    }

    return 0;
}