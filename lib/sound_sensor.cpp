#include "sound_sensor.hpp"
#include "hardware/adc.h"
#include <algorithm>

void SoundSensor::init() {
    adc_init();
    adc_gpio_init(SoundPins::PIN_SOUND_ADC);
}

float SoundSensor::readVolumePercent() {
    adc_select_input(SoundPins::ADC_CHANNEL);

    uint16_t min_val = 4095;
    uint16_t max_val = 0;

    // Знімаємо вибірку за короткий проміжок часу для фіксації звукової хвилі
    for (int i = 0; i < 200; ++i) {
        uint16_t sample = adc_read(); // 12-бітне значення RP2350: 0 .. 4095
        if (sample < min_val) min_val = sample;
        if (sample > max_val) max_val = sample;
        sleep_us(50);
    }

    uint16_t peak_to_peak = (max_val >= min_val) ? (max_val - min_val) : 0;

    // Масштабуємо амплітуду до 0..100% (відсікаємо дрібний шум мікрофона)
    if (peak_to_peak < 15) peak_to_peak = 0;
    
    float percent = (static_cast<float>(peak_to_peak) / 1800.0f) * 100.0f;
    if (percent > 100.0f) percent = 100.0f;

    return percent;
}