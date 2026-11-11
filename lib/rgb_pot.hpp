#pragma once

#include <cstdint>
#include "pico/stdlib.h"

namespace HardwarePins {
    inline constexpr uint PIN_LED_RED    = 18; // RP2350A: IO18
    inline constexpr uint PIN_LED_YELLOW = 20; // RP2350A: IO20
    inline constexpr uint PIN_LED_GREEN  = 19; // RP2350A: IO19
}

class LedController {
public:
    static void init();
    static void setRed(bool on);
    static void setYellow(bool on);
    static void setGreen(bool on);
    static void setOnly(int ledIndex); // 0: All Off, 1: Red, 2: Yellow, 3: Green
};