#pragma once

#include <cstdint>
#include "pico/stdlib.h"

namespace SoundPins {
    inline constexpr uint PIN_SOUND_ADC = 28; // GP28 (ADC2)
    inline constexpr uint ADC_CHANNEL   = 2;  // Канал ADC2 для GPIO 28
}

class SoundSensor {
public:
    static void init();
    
    // Зчитує пачку семплів за коротке вікно (наприклад 15-20 мс) 
    // і повертає пікову амплітуду (Peak-to-Peak) шуму у відсотках 0..100%
    static float readVolumePercent();
};