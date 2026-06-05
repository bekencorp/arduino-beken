#pragma once

#include <stdint.h>
#include <stddef.h>

// Replace these aliases once the real BK7239N reference board pinout is confirmed.
static const uint8_t NUM_DIGITAL_PINS = 56;

static const uint8_t LED_BUILTIN = 24;
static const uint8_t BUILTIN_LED = LED_BUILTIN;

static const uint8_t TX = 0;
static const uint8_t RX = 1;

static const uint8_t SDA = 19;
static const uint8_t SCL = 18;

#define PIN_WIRE_SDA SDA
#define PIN_WIRE_SCL SCL

static const uint8_t SS = 15;
static const uint8_t MOSI = 16;
static const uint8_t MISO = 17;
static const uint8_t SCK = 14;

#define PIN_SPI_SS SS
#define PIN_SPI_MOSI MOSI
#define PIN_SPI_MISO MISO
#define PIN_SPI_SCK SCK

static const uint8_t A0 = 4;  // GPIO4 maps to ADC3
static const uint8_t A1 = 5;  // GPIO5 maps to ADC4
static const uint8_t A2 = 6;  // GPIO6 maps to ADC5
static const uint8_t A3 = 7;  // GPIO7 maps to ADC6
static const uint8_t A4 = 0;  // GPIO0 maps to ADC12
static const uint8_t A5 = 1;  // GPIO1 maps to ADC13
static const uint8_t A6 = 12; // GPIO12 maps to ADC14
static const uint8_t A7 = 13; // GPIO13 maps to ADC15
static const uint8_t A8 = 20; // GPIO20 is available (though not ADC-capable)
static const uint8_t A9 = 19; // GPIO19 is available (though not ADC-capable)

// Check if the pin is an analog pin and return its ADC channel for initialization.
// This function is used to validate if a pin supports analog reading and map it to ADC channel.
static inline int digitalPinToAnalogChannel(uint8_t pin) {
    // Check if the pin is one of the GPIO pins that support ADC and map to corresponding channel
    switch (pin) {
        case 4:  // Maps to ADC channel 3 (was A0)
            return 3;
        case 5:  // Maps to ADC channel 4 (was A1)
            return 4;
        case 6:  // Maps to ADC channel 5 (was A2)
            return 5;
        case 7:  // Maps to ADC channel 6 (was A3)
            return 6;
        case 0:  // Maps to ADC channel 12 (was A4)
            return 12;
        case 1:  // Maps to ADC channel 13 (was A5)
            return 13;
        case 12: // Maps to ADC channel 14 (was A6)
            return 14;
        case 13: // Maps to ADC channel 15 (was A7)
            return 15;
        case 20: // A8 is not ADC-capable, return -1
            return -1;
        case 19: // A9 is not ADC-capable, return -1
            return -1;
        default:
            return -1;  // Pin doesn't support ADC
    }
}

static inline int digitalPinToPwmChannel(uint8_t pin) {
    switch (pin) {
        case 5:
            return 9;   // PWM9 -> GPIO_5
        case 6:
            return 0;   // PWM0 -> GPIO_6
        case 7:
            return 1;   // PWM1 -> GPIO_7
        case 8:
            return 2;   // PWM2 -> GPIO_8
        case 9:
            return 3;   // PWM3 -> GPIO_9
        case 12:
            return 10;  // PWM10 -> GPIO_12
        case 13:
            return 11;  // PWM11 -> GPIO_13
        case 18:
            return 4;   // PWM4 -> GPIO_18
        case 19:
            return 5;   // PWM5 -> GPIO_19
        case 20:
            return 6;   // PWM6 -> GPIO_20
        case 21:
            return 7;   // PWM7 -> GPIO_21
        case 22:
            return 8;   // PWM8 -> GPIO_22
        case LED_BUILTIN:  // 即 pin 24
            return 4;      // LED_BUILTIN also maps to PWM4 (GPIO_18)
        default:
            return -1;
    }
}