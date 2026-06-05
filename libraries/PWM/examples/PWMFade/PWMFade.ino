#include <Arduino.h>
static constexpr uint8_t kPwmPin = LED_BUILTIN;
static constexpr uint8_t kPwmResolutionBits = 10;
static constexpr uint32_t kPwmFrequencyHz = 1000;
static constexpr uint32_t kPwmPeriod = (1UL << kPwmResolutionBits) - 1UL;

static uint32_t g_duty = 0;
static int g_step = 25;

void setup() {
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("arduino-beken pwm fade");
    Serial.print("Driving PWM through analogWrite() on GPIO ");
    Serial.print(kPwmPin);
    Serial.println(" (LED_BUILTIN on the generic variant).");
    Serial.println("Expected output is about 1 kHz with a 0-100% duty sweep.");
    analogWriteResolution(kPwmPin, kPwmResolutionBits);
    analogWriteFrequency(kPwmPin, kPwmFrequencyHz);
}

void loop() {
    analogWrite(kPwmPin, static_cast<int>(g_duty));

    Serial.print("duty=");
    Serial.print((g_duty * 100) / kPwmPeriod);
    Serial.println("%");

    const int next_duty = static_cast<int>(g_duty) + g_step;
    if (next_duty <= 0 || next_duty >= static_cast<int>(kPwmPeriod)) {
        g_step = -g_step;
    }
    g_duty = static_cast<uint32_t>(constrain(next_duty, 0, static_cast<int>(kPwmPeriod)));

    delay(40);
}
