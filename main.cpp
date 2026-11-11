#include "pico/stdlib.h"
#include "display_touch.hpp"
#include "rgb_pot.hpp"
#include "buzzer_audio.hpp"
#include <cstdio>

enum AppState {
    STATE_MENU,
    STATE_TOUCH_DEMO,
    STATE_LED_DEMO,
    STATE_AUDIO_PLAYER
};

struct TouchBtn {
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
        DisplayDriver::drawString(x + 10, y + (h / 2) - (4 * textSize), label, textColor, bgColor, textSize);
    }
};

void renderMainMenu() {
    DisplayDriver::fillScreen(Colors::DARKGRAY);

    DisplayDriver::fillRect(0, 0, LCD_WIDTH, 30, Colors::BLUE);
    DisplayDriver::drawString(75, 7, "MAIN SELECT MENU", Colors::WHITE, Colors::BLUE, 2);

    // Блок 1: Сенсор
    DisplayDriver::fillRect(20, 38, 280, 56, 0x18E3);
    DisplayDriver::drawRect(20, 38, 280, 56, Colors::CYAN);
    DisplayDriver::drawString(35, 46, "1. TOUCH & DRAW TEST", Colors::CYAN, 0x18E3, 2);
    DisplayDriver::drawString(35, 70, "Draw pad, coordinates, count", Colors::WHITE, 0x18E3, 1);

    // Блок 2: Діоди
    DisplayDriver::fillRect(20, 102, 280, 56, 0x2124);
    DisplayDriver::drawRect(20, 102, 280, 56, Colors::YELLOW);
    DisplayDriver::drawString(35, 110, "2. LED CONTROLLER", Colors::YELLOW, 0x2124, 2);
    DisplayDriver::drawString(35, 134, "Hardware LEDs: Red, Yellow, Green", Colors::WHITE, 0x2124, 1);

    // Блок 3: Аудіоплеєр
    DisplayDriver::fillRect(20, 166, 280, 56, 0x3106);
    DisplayDriver::drawRect(20, 166, 280, 56, Colors::MAGENTA);
    DisplayDriver::drawString(35, 174, "3. BUZZER MUSIC PLAYER", Colors::MAGENTA, 0x3106, 2);
    DisplayDriver::drawString(35, 198, "Pirates, Jingle Bells, Shchedryk", Colors::WHITE, 0x3106, 1);
}

void renderTouchDemo(const TouchBtn& btnBack, const TouchBtn& btnInc, const TouchBtn& btnClr) {
    DisplayDriver::fillScreen(Colors::DARKGRAY);
    DisplayDriver::fillRect(0, 0, LCD_WIDTH, 30, Colors::BLUE);
    DisplayDriver::drawString(75, 7, "TOUCH TEST DEMO", Colors::WHITE, Colors::BLUE, 2);

    btnBack.draw(false, 1);
    btnInc.draw(false, 2);
    btnClr.draw(false, 2);

    DisplayDriver::drawRect(15, 140, 290, 90, Colors::CYAN);
    DisplayDriver::drawString(25, 146, "Touch Pad Area (Draw Here):", Colors::CYAN, Colors::DARKGRAY, 1);
}

void renderLedDemo(const TouchBtn& btnBack, const TouchBtn& bR, const TouchBtn& bY, const TouchBtn& bG, const TouchBtn& bOff, int activeLed) {
    DisplayDriver::fillScreen(Colors::DARKGRAY);
    DisplayDriver::fillRect(0, 0, LCD_WIDTH, 30, Colors::BLUE);
    DisplayDriver::drawString(65, 7, "LED CONTROLLER", Colors::WHITE, Colors::BLUE, 2);

    btnBack.draw(false, 1);
    bR.draw(activeLed == 1, 2);
    bY.draw(activeLed == 2, 2);
    bG.draw(activeLed == 3, 2);
    bOff.draw(activeLed == 0, 2);
}

void renderAudioPlayer(const TouchBtn& btnBack, const TouchBtn& bM1, const TouchBtn& bM2, const TouchBtn& bM3,
                       const TouchBtn& bPlay, const TouchBtn& bStop, const TouchBtn& bBeep,
                       const TouchBtn& bVDown, const TouchBtn& bVUp, MelodyId selectedMelody) {
    DisplayDriver::fillScreen(Colors::DARKGRAY);
    DisplayDriver::fillRect(0, 0, LCD_WIDTH, 30, Colors::BLUE);
    DisplayDriver::drawString(50, 7, "BUZZER MUSIC PLAYER", Colors::WHITE, Colors::BLUE, 2);

    btnBack.draw(false, 1);

    bM1.draw(selectedMelody == MELODY_PIRATES, 1);
    bM2.draw(selectedMelody == MELODY_JINGLE_BELLS, 1);
    bM3.draw(selectedMelody == MELODY_SHCHEDRYK, 1);

    bPlay.draw(false, 2);
    bStop.draw(false, 2);
    bBeep.draw(false, 1);

    bVDown.draw(false, 1);
    bVUp.draw(false, 1);
}

int main() {
    stdio_init_all();

    DisplayDriver::init();
    TouchDriver::init();
    LedController::init();
    BuzzerAudio::init();

    AppState state = STATE_MENU;
    renderMainMenu();

    TouchBtn menuTouch = { 20, 38,  280, 56, "", 0, 0 };
    TouchBtn menuLed   = { 20, 102, 280, 56, "", 0, 0 };
    TouchBtn menuAudio = { 20, 166, 280, 56, "", 0, 0 };

    TouchBtn btnBack = { 5, 5, 52, 20, "< Menu", Colors::RED, Colors::WHITE };

    // Сенсор
    TouchBtn btnInc = { 20,  42, 130, 42, "+ Count", Colors::GREEN, Colors::WHITE };
    TouchBtn btnClr = { 170, 42, 130, 42, "Clear",   Colors::RED,   Colors::WHITE };
    int count = 0;

    // Діоди
    TouchBtn btnRed    = { 20, 42,  280, 38, "1. RED LED   (IO18)", Colors::RED,    Colors::WHITE };
    TouchBtn btnYellow = { 20, 88,  280, 38, "2. YELLOW LED(IO20)", Colors::YELLOW, Colors::BLACK };
    TouchBtn btnGreen  = { 20, 134, 280, 38, "3. GREEN LED (IO19)", Colors::GREEN,  Colors::BLACK };
    TouchBtn btnOff    = { 20, 180, 280, 38, "4. ALL OFF",          Colors::BLACK,  Colors::WHITE };
    int activeLed = 0;

    // Плеєр
    TouchBtn btnM1    = { 15,  38, 90, 38, "Pirates",  0x18E3, Colors::WHITE };
    TouchBtn btnM2    = { 115, 38, 90, 38, "Jingle",   0x18E3, Colors::WHITE };
    TouchBtn btnM3    = { 215, 38, 90, 38, "Shchedryk",0x18E3, Colors::WHITE };

    TouchBtn btnPlay  = { 15,  85, 95, 42, "PLAY",  Colors::GREEN, Colors::WHITE };
    TouchBtn btnStop  = { 115, 85, 95, 42, "STOP",  Colors::RED,   Colors::WHITE };
    TouchBtn btnBeep  = { 215, 85, 90, 42, "BEEP",  Colors::BLUE,  Colors::WHITE };

    TouchBtn btnVDown = { 15,  136, 65, 36, "Vol -", Colors::BLACK, Colors::WHITE };
    TouchBtn btnVUp   = { 240, 136, 65, 36, "Vol +", Colors::BLACK, Colors::WHITE };

    MelodyId selectedMelody = MELODY_PIRATES;

    int16_t tx = 0, ty = 0;
    bool wasPressed = false;
    char buf[32];

    while (true) {
        bool isTouched = TouchDriver::getTouch(tx, ty);
        BuzzerAudio::update(15);

        // ------------------ ГОЛОВНЕ МЕНЮ ------------------
        if (state == STATE_MENU) {
            if (isTouched && !wasPressed) {
                if (menuTouch.contains(tx, ty)) {
                    state = STATE_TOUCH_DEMO;
                    renderTouchDemo(btnBack, btnInc, btnClr);
                    wasPressed = true;
                } else if (menuLed.contains(tx, ty)) {
                    state = STATE_LED_DEMO;
                    renderLedDemo(btnBack, btnRed, btnYellow, btnGreen, btnOff, activeLed);
                    wasPressed = true;
                } else if (menuAudio.contains(tx, ty)) {
                    state = STATE_AUDIO_PLAYER;
                    renderAudioPlayer(btnBack, btnM1, btnM2, btnM3, btnPlay, btnStop, btnBeep, btnVDown, btnVUp, selectedMelody);
                    wasPressed = true;
                }
            }
        }
        // ------------------ РЕЖИМ 1: СЕНСОР ------------------
        else if (state == STATE_TOUCH_DEMO) {
            snprintf(buf, sizeof(buf), "Count: %-4d", count);
            DisplayDriver::drawString(100, 100, buf, Colors::YELLOW, Colors::DARKGRAY, 2);

            if (isTouched) {
                snprintf(buf, sizeof(buf), "X:%03d Y:%03d", tx, ty);
                DisplayDriver::drawString(220, 10, buf, Colors::WHITE, Colors::BLUE, 1);

                if (tx >= 18 && tx <= 300 && ty >= 155 && ty <= 225) {
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
                        DisplayDriver::fillRect(16, 150, 288, 78, Colors::DARKGRAY);
                    }
                    wasPressed = true;
                }
            }
        }
        // ------------------ РЕЖИМ 2: ДІОДИ ------------------
        else if (state == STATE_LED_DEMO) {
            if (isTouched && !wasPressed) {
                if (btnBack.contains(tx, ty)) {
                    activeLed = 0;
                    LedController::setOnly(0);
                    state = STATE_MENU;
                    renderMainMenu();
                } else {
                    int newLed = activeLed;
                    if (btnRed.contains(tx, ty))         newLed = 1;
                    else if (btnYellow.contains(tx, ty)) newLed = 2;
                    else if (btnGreen.contains(tx, ty))  newLed = 3;
                    else if (btnOff.contains(tx, ty))    newLed = 0;

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
        // ------------------ РЕЖИМ 3: ПЛЕЄР ------------------
        else if (state == STATE_AUDIO_PLAYER) {
            // Графічна шкала гучності
            uint8_t vol = BuzzerAudio::getVolume();
            snprintf(buf, sizeof(buf), "Vol: %3d%%", vol);
            DisplayDriver::drawString(92, 147, buf, Colors::YELLOW, Colors::DARKGRAY, 2);

            // Прогрес-бар гучності
            DisplayDriver::drawRect(20, 180, 280, 16, Colors::WHITE);
            int16_t fillW = (276 * vol) / 100;
            DisplayDriver::fillRect(22, 182, fillW, 12, Colors::GREEN);
            DisplayDriver::fillRect(22 + fillW, 182, 276 - fillW, 12, Colors::DARKGRAY);

            // Статус треку
            const char* status = BuzzerAudio::isPlaying() ? "STATUS: PLAYING" : "STATUS: STOPPED";
            DisplayDriver::drawString(70, 210, status, BuzzerAudio::isPlaying() ? Colors::GREEN : Colors::WHITE, Colors::DARKGRAY, 2);

            if (isTouched && !wasPressed) {
                if (btnBack.contains(tx, ty)) {
                    BuzzerAudio::stop();
                    state = STATE_MENU;
                    renderMainMenu();
                } else if (btnM1.contains(tx, ty)) {
                    selectedMelody = MELODY_PIRATES;
                    btnM1.draw(true, 1); btnM2.draw(false, 1); btnM3.draw(false, 1);
                    if (BuzzerAudio::isPlaying()) BuzzerAudio::playMelody(selectedMelody);
                } else if (btnM2.contains(tx, ty)) {
                    selectedMelody = MELODY_JINGLE_BELLS;
                    btnM1.draw(false, 1); btnM2.draw(true, 1); btnM3.draw(false, 1);
                    if (BuzzerAudio::isPlaying()) BuzzerAudio::playMelody(selectedMelody);
                } else if (btnM3.contains(tx, ty)) {
                    selectedMelody = MELODY_SHCHEDRYK;
                    btnM1.draw(false, 1); btnM2.draw(false, 1); btnM3.draw(true, 1);
                    if (BuzzerAudio::isPlaying()) BuzzerAudio::playMelody(selectedMelody);
                } else if (btnPlay.contains(tx, ty)) {
                    BuzzerAudio::playMelody(selectedMelody);
                } else if (btnStop.contains(tx, ty)) {
                    BuzzerAudio::stop();
                } else if (btnBeep.contains(tx, ty)) {
                    BuzzerAudio::beep(1200, 150); // Миттєвий перевірочний писк
                } else if (btnVDown.contains(tx, ty)) {
                    BuzzerAudio::volumeDown();
                } else if (btnVUp.contains(tx, ty)) {
                    BuzzerAudio::volumeUp();
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