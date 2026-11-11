#include "buzzer_audio.hpp"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// 1. Мелодія: Pirates of the Caribbean
static const NoteEvent melodyPirates[] = {
    {Notes::E4, 140}, {Notes::G4, 140}, {Notes::A4, 280}, {Notes::A4, 140}, {Notes::REST, 40},
    {Notes::A4, 140}, {Notes::B4, 140}, {Notes::C5, 280}, {Notes::C5, 140}, {Notes::REST, 40},
    {Notes::C5, 140}, {Notes::D5, 140}, {Notes::B4, 280}, {Notes::B4, 140}, {Notes::REST, 40},
    {Notes::A4, 140}, {Notes::G4, 140}, {Notes::A4, 320}, {Notes::REST, 140},

    {Notes::E4, 140}, {Notes::G4, 140}, {Notes::A4, 280}, {Notes::A4, 140}, {Notes::REST, 40},
    {Notes::A4, 140}, {Notes::B4, 140}, {Notes::C5, 280}, {Notes::C5, 140}, {Notes::REST, 40},
    {Notes::C5, 140}, {Notes::D5, 140}, {Notes::B4, 280}, {Notes::B4, 140}, {Notes::REST, 40},
    {Notes::A4, 140}, {Notes::G4, 140}, {Notes::A4, 380}, {Notes::REST, 200},

    {Notes::E4, 140}, {Notes::G4, 140}, {Notes::A4, 280}, {Notes::A4, 140}, {Notes::REST, 40},
    {Notes::A4, 140}, {Notes::C5, 140}, {Notes::D5, 280}, {Notes::D5, 140}, {Notes::REST, 40},
    {Notes::D5, 140}, {Notes::E5, 140}, {Notes::F5, 280}, {Notes::F5, 140}, {Notes::REST, 40},
    {Notes::E5, 140}, {Notes::D5, 140}, {Notes::E5, 280}, {Notes::A4, 140}, {Notes::REST, 40},
    {Notes::A4, 140}, {Notes::B4, 140}, {Notes::C5, 280}, {Notes::C5, 140}, {Notes::REST, 40},
    {Notes::D5, 250}, {Notes::E5, 140}, {Notes::A4, 450}, {Notes::REST, 300}
};

// 2. Мелодія: Jingle Bells (відповідно до Лабораторної №4)
static const NoteEvent melodyJingleBells[] = {
    {Notes::E4, 250}, {Notes::E4, 250}, {Notes::E4, 500}, {Notes::REST, 50},
    {Notes::E4, 250}, {Notes::E4, 250}, {Notes::E4, 500}, {Notes::REST, 50},
    {Notes::E4, 250}, {Notes::G4, 250}, {Notes::C4, 250}, {Notes::D4, 250},
    {Notes::E4, 1000}, {Notes::REST, 100},

    {Notes::F4, 250}, {Notes::F4, 250}, {Notes::F4, 250}, {Notes::F4, 250},
    {Notes::F4, 500}, {Notes::E4, 250}, {Notes::E4, 250}, {Notes::E4, 250},
    {Notes::E4, 250}, {Notes::D4, 500}, {Notes::D4, 250}, {Notes::E4, 250},
    {Notes::D4, 1000}, {Notes::G4, 1000}, {Notes::REST, 250}
};

// 3. Мелодія: Щедрик (Carol of the Bells)
static const NoteEvent melodyShchedryk[] = {
    {Notes::AS4, 250}, {Notes::A4, 125}, {Notes::AS4, 125}, {Notes::G4, 250},
    {Notes::AS4, 250}, {Notes::A4, 125}, {Notes::AS4, 125}, {Notes::G4, 250},
    {Notes::AS4, 250}, {Notes::A4, 125}, {Notes::AS4, 125}, {Notes::G4, 250},
    {Notes::AS4, 250}, {Notes::A4, 125}, {Notes::AS4, 125}, {Notes::G4, 250},

    {Notes::D5,  250}, {Notes::C5, 125}, {Notes::D5,  125}, {Notes::AS4, 250},
    {Notes::D5,  250}, {Notes::C5, 125}, {Notes::D5,  125}, {Notes::AS4, 250},
    {Notes::D5,  250}, {Notes::C5, 125}, {Notes::D5,  125}, {Notes::AS4, 250},
    {Notes::D5,  250}, {Notes::C5, 125}, {Notes::D5,  125}, {Notes::AS4, 250},

    {Notes::G5,  350}, {Notes::G5, 150}, {Notes::F5,  150}, {Notes::DS5, 150},
    {Notes::D5,  350}, {Notes::C5, 150}, {Notes::AS4, 150}, {Notes::A4,  150},
    {Notes::G4,  600}, {Notes::REST, 300}
};

static uint s_slice = 0;
static uint s_chan  = 0;
static uint8_t s_volume = 70; // 0..100%

static const NoteEvent* s_currentNotes = nullptr;
static size_t s_melodyLength = 0;
static size_t s_noteIndex = 0;
static uint32_t s_noteTimeElapsed = 0;
static bool s_isPlaying = false;
static MelodyId s_activeId = MELODY_PIRATES;

void BuzzerAudio::init() {
    gpio_set_function(AudioPins::PIN_BUZZER, GPIO_FUNC_PWM);
    s_slice = pwm_gpio_to_slice_num(AudioPins::PIN_BUZZER);
    s_chan  = pwm_gpio_to_channel(AudioPins::PIN_BUZZER);
    silence();
}

void BuzzerAudio::setVolume(uint8_t volPercent) {
    if (volPercent > 100) volPercent = 100;
    s_volume = volPercent;

    if (s_isPlaying && s_currentNotes) {
        applyTone(s_currentNotes[s_noteIndex].frequency);
    }
}

uint8_t BuzzerAudio::getVolume() {
    return s_volume;
}

void BuzzerAudio::volumeUp() {
    if (s_volume <= 90) setVolume(s_volume + 10);
    else setVolume(100);
}

void BuzzerAudio::volumeDown() {
    if (s_volume >= 10) setVolume(s_volume - 10);
    else setVolume(0);
}

void BuzzerAudio::applyTone(uint16_t frequency) {
    if (frequency == 0 || s_volume == 0) {
        silence();
        return;
    }

    // 1 МГц базова частота таймера PWM (clkdiv = f_sys / 1_000_000)
    uint32_t f_sys = clock_get_hz(clk_sys);
    float clkdiv = static_cast<float>(f_sys) / 1000000.0f;
    pwm_set_clkdiv(s_slice, clkdiv);

    // Період wrap: 1 000 000 / частота ноти
    uint32_t wrap = 1000000 / frequency;
    if (wrap > 65535) wrap = 65535;
    pwm_set_wrap(s_slice, wrap);

    // Гучність: масштабування шпаруватості від 0 до wrap/2 (50% max для чистого тону)
    uint32_t level = (wrap * s_volume) / 200;
    if (s_volume > 0 && level == 0) level = 1;

    pwm_set_chan_level(s_slice, s_chan, level);
    pwm_set_enabled(s_slice, true);
}

void BuzzerAudio::silence() {
    pwm_set_chan_level(s_slice, s_chan, 0);
    pwm_set_enabled(s_slice, false);
}

void BuzzerAudio::beep(uint16_t freq, uint16_t durationMs) {
    applyTone(freq);
    sleep_ms(durationMs);
    silence();
}

void BuzzerAudio::playMelody(MelodyId id) {
    s_activeId = id;
    if (id == MELODY_PIRATES) {
        s_currentNotes = melodyPirates;
        s_melodyLength = sizeof(melodyPirates) / sizeof(NoteEvent);
    } else if (id == MELODY_JINGLE_BELLS) {
        s_currentNotes = melodyJingleBells;
        s_melodyLength = sizeof(melodyJingleBells) / sizeof(NoteEvent);
    } else {
        s_currentNotes = melodyShchedryk;
        s_melodyLength = sizeof(melodyShchedryk) / sizeof(NoteEvent);
    }

    s_noteIndex = 0;
    s_noteTimeElapsed = 0;
    s_isPlaying = true;
    applyTone(s_currentNotes[0].frequency);
}

void BuzzerAudio::stop() {
    s_isPlaying = false;
    silence();
}

bool BuzzerAudio::isPlaying() {
    return s_isPlaying;
}

MelodyId BuzzerAudio::getCurrentMelody() {
    return s_activeId;
}

void BuzzerAudio::update(uint32_t deltaMs) {
    if (!s_isPlaying || !s_currentNotes) return;

    s_noteTimeElapsed += deltaMs;
    if (s_noteTimeElapsed >= s_currentNotes[s_noteIndex].duration) {
        s_noteTimeElapsed = 0;
        s_noteIndex++;

        if (s_noteIndex >= s_melodyLength) {
            stop();
            return;
        }

        applyTone(s_currentNotes[s_noteIndex].frequency);
    }
}