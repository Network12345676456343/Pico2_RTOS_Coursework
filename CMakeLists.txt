cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico2)
set(PICO_PLATFORM rp2350-arm-s)

include(pico_sdk_import.cmake)

project(Pico2_RTOS_Coursework C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(${PROJECT_NAME}
    main.cpp
    lib/display_touch.cpp
    lib/rgb_pot.cpp
    lib/buzzer_audio.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
    lib
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(${PROJECT_NAME}
    pico_stdlib
    hardware_spi
    hardware_i2c
    hardware_gpio
    hardware_adc
    hardware_pwm
)

pico_add_extra_outputs(${PROJECT_NAME})