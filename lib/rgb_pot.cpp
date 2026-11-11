#include "rgb_pot.hpp"
#include "hardware/gpio.h"

void LedController::init() {
    gpio_init(HardwarePins::PIN_LED_RED);
    gpio_set_dir(HardwarePins::PIN_LED_RED, GPIO_OUT);

    gpio_init(HardwarePins::PIN_LED_YELLOW);
    gpio_set_dir(HardwarePins::PIN_LED_YELLOW, GPIO_OUT);

    gpio_init(HardwarePins::PIN_LED_GREEN);
    gpio_set_dir(HardwarePins::PIN_LED_GREEN, GPIO_OUT);

    setOnly(0); // Вимкнути всі на старті
}

void LedController::setRed(bool on) {
    gpio_put(HardwarePins::PIN_LED_RED, on ? 1 : 0);
}

void LedController::setYellow(bool on) {
    gpio_put(HardwarePins::PIN_LED_YELLOW, on ? 1 : 0);
}

void LedController::setGreen(bool on) {
    gpio_put(HardwarePins::PIN_LED_GREEN, on ? 1 : 0);
}

void LedController::setOnly(int ledIndex) {
    setRed(ledIndex == 1);
    setYellow(ledIndex == 2);
    setGreen(ledIndex == 3);
}