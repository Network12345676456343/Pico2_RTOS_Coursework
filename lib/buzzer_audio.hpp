#pragma once

#include <cstdint>
#include "pico/stdlib.h"

namespace AudioPins {
    inline constexpr uint PIN_BUZZER = 12; // Пін зумера згідно з Лабораторною №4
}

namespace Notes {
    inline constexpr uint16_t REST = 0;
    inline constexpr uint16_t C4  = 262, CS4 = 277, D4  = 294, DS4 = 311, E4  = 330, F4  = 349, FS4 = 370, G4  = 392, GS4 = 415, A4  = 440, AS4 = 466, B4  = 494;
    inline constexpr uint16_t C5  = 523, CS5 = 554, D5  = 587, DS5 = 622, E5  = 659, F5  = 698, FS5 = 740, G5  = 784, GS5 = 830, A5  = 880, AS5 = 932, B5  = 988;
    inline constexpr uint16_t C6  = 1047, D6 = 1175, E6 = 1319, F6 = 1397, G6 = 1568;
}

struct NoteEvent {
    uint16_t frequency; // Гц
    uint16_t duration;  // мс
};

enum MelodyId {
    MELODY_PIRATES = 0,
    MELODY_JINGLE_BELLS,
    MELODY_SHCHEDRYK,
    MELODY_COUNT
};

class BuzzerAudio {
public:
    static void init();
    static void setVolume(uint8_t volPercent); // 0 .. 100%
    static uint8_t getVolume();
    static void volumeUp();
    static void volumeDown();

    static void playMelody(MelodyId id);
    static void stop();
    static bool isPlaying();
    static MelodyId getCurrentMelody();

    static void update(uint32_t deltaMs);
    static void beep(uint16_t freq = 1000, uint16_t durationMs = 120);

private:
    static void applyTone(uint16_t frequency);
    static void silence();
};