#include "pico/stdlib.h"
#include "display_touch.hpp"
#include "rgb_pot.hpp"
#include <cstdio>

enum AppState {
    STATE_MENU,
    STATE_TOUCH_DEMO,
    STATE_LED_DEMO
};

struct TouchButton {
    int16_t x, y, w, h;
    const char* label;
    uint16_t bgColor;
    uint16_t textColor;

    bool contains(int16_t px, int16_t py) const {
        return (px >= x && px <= x + w && py >= y && py <= y + h);
    }

    void draw(bool selected = false, uint8_t textSize = 2) const {
        DisplayDriver::fillRect(x, y, w, h, bgColor);
        DisplayDriver::drawRect(x, y, w, h, selected ? Colors::WHITE : Colors::DARKGRAY);
        if (selected) {
            DisplayDriver::drawRect(x + 1, y + 1, w - 2, h - 2, Colors::WHITE);
        }
        DisplayDriver::drawString(x + 15, y + (h / 2) - (4 * textSize), label, textColor, bgColor, textSize);
    }
};

// Екран 1: Головне меню з 2 блоками
void renderMainMenu() {
    DisplayDriver::fillScreen(Colors::DARKGRAY);

    // Верхній банер
    DisplayDriver::fillRect(0, 0, LCD_WIDTH, 34, Colors::BLUE);
    DisplayDriver::drawString(70, 9, "MAIN SELECT MENU", Colors::WHITE, Colors::BLUE, 2);

    // Блок 1: Touch Demo
    DisplayDriver::fillRect(20, 48, 280, 80, 0x18E3); // темний синьо-сірий
    DisplayDriver::drawRect(20, 48, 280, 80, Colors::CYAN);
    DisplayDriver::drawString(40, 65, "1. TOUCH & DRAW TEST", Colors::CYAN, 0x18E3, 2);
    DisplayDriver::drawString(40, 95, "Screen draw pad, coords, count", Colors::WHITE, 0x18E3, 1);

    // Блок 2: LED Control
    DisplayDriver::fillRect(20, 142, 280, 80, 0x2124); // темний графіт
    DisplayDriver::drawRect(20, 142, 280, 80, Colors::YELLOW);
    DisplayDriver::drawString(40, 159, "2. LED CONTROLLER", Colors::YELLOW, 0x2124, 2);
    DisplayDriver::drawString(40, 189, "Hardware LEDs: Red, Yellow, Green", Colors::WHITE, 0x2124, 1);
}

// Екран 2: Інтерфейс перевірки сенсора
void renderTouchDemoScreen(const TouchButton& btnBack, const TouchButton& btnInc, const TouchButton& btnClr) {
    DisplayDriver::fillScreen(Colors::DARKGRAY);

    DisplayDriver::fillRect(0, 0, LCD_WIDTH, 32, Colors::BLUE);
    DisplayDriver::drawString(75, 8, "TOUCH TEST DEMO", Colors::WHITE, Colors::BLUE, 2);

    btnBack.draw(false, 1);
    btnInc.draw(false, 2);
    btnClr.draw(false, 2);

    DisplayDriver::drawRect(15, 145, 290, 85, Colors::CYAN);
    DisplayDriver::drawString(25, 150, "Touch Pad Area (Draw Here):", Colors::CYAN, Colors::DARKGRAY, 1);
}

// Екран 3: Інтерфейс перевірки діодів
void renderLedScreen(const TouchButton& btnBack, const TouchButton& bR, const TouchButton& bY, const TouchButton& bG, const TouchButton& bOff, int activeLed) {
    DisplayDriver::fillScreen(Colors::DARKGRAY);

    DisplayDriver::fillRect(0, 0, LCD_WIDTH, 32, Colors::BLUE);
    DisplayDriver::drawString(65, 8, "LED CONTROLLER", Colors::WHITE, Colors::BLUE, 2);

    btnBack.draw(false, 1);

    bR.draw(activeLed == 1, 2);
    bY.draw(activeLed == 2, 2);
    bG.draw(activeLed == 3, 2);
    bOff.draw(activeLed == 0, 2);
}

int main() {
    stdio_init_all();

    // 1. Ініціалізація апаратної частини
    DisplayDriver::init();
    TouchDriver::init();
    LedController::init();

    AppState state = STATE_MENU;
    renderMainMenu();

    // Сенсорні зони головного меню
    TouchButton menuBlockTouch = { 20, 48,  280, 80, "", Colors::BLACK, Colors::WHITE };
    TouchButton menuBlockLed   = { 20, 142, 280, 80, "", Colors::BLACK, Colors::WHITE };

    // Кнопка повернення в меню (спільна для підменю)
    TouchButton btnBack = { 5, 5, 52, 22, "< Menu", Colors::RED, Colors::WHITE };

    // Елементи для тесту сенсора
    TouchButton btnInc = { 20,  45, 130, 45, "+ Count", Colors::GREEN, Colors::WHITE };
    TouchButton btnClr = { 170, 45, 130, 45, "Clear",   Colors::RED,   Colors::WHITE };

    // Елементи для керування діодами
    TouchButton btnRed    = { 20, 45,  280, 40, "1. RED LED   (IO18)", Colors::RED,    Colors::WHITE };
    TouchButton btnYellow = { 20, 92,  280, 40, "2. YELLOW LED(IO20)", Colors::YELLOW, Colors::BLACK };
    TouchButton btnGreen  = { 20, 139, 280, 40, "3. GREEN LED (IO19)", Colors::GREEN,  Colors::BLACK };
    TouchButton btnOff    = { 20, 186, 280, 38, "4. ALL OFF",          Colors::BLACK,  Colors::WHITE };

    int count = 0;
    int activeLed = 0;
    int16_t tx = 0, ty = 0;
    bool wasPressed = false;
    char textBuf[32];

    while (true) {
        bool isTouched = TouchDriver::getTouch(tx, ty);

        // ------------------ ГОЛОВНЕ МЕНЮ ------------------
        if (state == STATE_MENU) {
            if (isTouched && !wasPressed) {
                if (menuBlockTouch.contains(tx, ty)) {
                    state = STATE_TOUCH_DEMO;
                    renderTouchDemoScreen(btnBack, btnInc, btnClr);
                    wasPressed = true;
                } else if (menuBlockLed.contains(tx, ty)) {
                    state = STATE_LED_DEMO;
                    renderLedScreen(btnBack, btnRed, btnYellow, btnGreen, btnOff, activeLed);
                    wasPressed = true;
                }
            }
        }
        // ------------------ РЕЖИМ 1: ТЕСТ СЕНСОРА ------------------
        else if (state == STATE_TOUCH_DEMO) {
            snprintf(textBuf, sizeof(textBuf), "Count: %-4d", count);
            DisplayDriver::drawString(100, 105, textBuf, Colors::YELLOW, Colors::DARKGRAY, 2);

            if (isTouched) {
                // Координати внизу
                snprintf(textBuf, sizeof(textBuf), "X:%03d Y:%03d", tx, ty);
                DisplayDriver::drawString(220, 12, textBuf, Colors::WHITE, Colors::BLUE, 1);

                // Малювання у виділеній сенсорній рамці
                if (tx >= 18 && tx <= 300 && ty >= 162 && ty <= 225) {
                    DisplayDriver::fillRect(tx - 2, ty - 2, 5, 5, Colors::YELLOW);
                }

                if (!wasPressed) {
                    if (btnBack.contains(tx, ty)) {
                        state = STATE_MENU;
                        renderMainMenu();
                    } else if (btnInc.contains(tx, ty)) {
                        count++;
                    } else if (btnClr.contains(tx, ty)) {
                        count = 0;
                        DisplayDriver::fillRect(16, 160, 288, 68, Colors::DARKGRAY);
                    }
                    wasPressed = true;
                }
            }
        }
        // ------------------ РЕЖИМ 2: СВІТЛОДІОДИ ------------------
        else if (state == STATE_LED_DEMO) {
            if (isTouched && !wasPressed) {
                if (btnBack.contains(tx, ty)) {
                    // Вимикаємо діоди при виході в меню
                    activeLed = 0;
                    LedController::setOnly(0);
                    state = STATE_MENU;
                    renderMainMenu();
                } else {
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

                        btnRed.draw(activeLed == 1, 2);
                        btnYellow.draw(activeLed == 2, 2);
                        btnGreen.draw(activeLed == 3, 2);
                        btnOff.draw(activeLed == 0, 2);
                    }
                }
                wasPressed = true;
            }
        }

        if (!isTouched) {
            wasPressed = false;
        }

        sleep_ms(15);
    }

    return 0;
}