#include <Arduino.h>

static const uint8_t kAdcPin = 4;  // Using GPIO pin number directly (GPIO4 maps to ADC3, valid for BK7239N)
static const size_t kSampleCount = 16;

void setup() {
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("arduino-beken adc read serial");
    Serial.print("Reading analog pin GPIO ");
    Serial.print(kAdcPin);
    Serial.println(".");
    Serial.println("Connect a variable voltage source within the board's ADC input range.");
}

void loop() {
    long total = 0;
    int min_value = 0x7FFF;
    int max_value = 0;

    for (size_t i = 0; i < kSampleCount; ++i) {
        const int sample = analogRead(kAdcPin);
        if (sample < 0) {
            Serial.println("analogRead() failed");
            delay(1000);
            return;
        }

        total += sample;
        if (sample < min_value) {
            min_value = sample;
        }
        if (sample > max_value) {
            max_value = sample;
        }
        delay(5);
    }

    Serial.print("raw avg/min/max = ");
    Serial.print(total / static_cast<long>(kSampleCount));
    Serial.print(" / ");
    Serial.print(min_value);
    Serial.print(" / ");
    Serial.println(max_value);

    delay(1000);
}