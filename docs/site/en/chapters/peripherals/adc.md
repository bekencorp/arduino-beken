# ADC

ADC (Analog-to-Digital Converter) converts analog voltage levels to digital values. The BK7239N features a high-performance AUX ADC for analog signal acquisition with 14-bit resolution (0-16383). The ADC supports up to 8 external analog input channels (ADC3/4/5/6/12/13/14/15), as well as internal channels for VBAT monitoring, temperature sensing, and debugging purposes.

## ADC API Reference

### analogRead(pin)

Read the analog value from the specified pin.

```cpp
int value = analogRead(uint8_t pin);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pin` | `uint8_t` | Analog pin to read |

**Return:** Analog value between 0 and 16383 for 14-bit resolution, or -1 if the pin is not an analog pin.

## ADC Examples

### Basic ADC Usage

```cpp
#include <Arduino.h>

static const uint8_t kAdcPin = 4;
static const size_t kSampleCount = 16;

void setup() {
    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("arduino-beken adc read serial");
    Serial.print("Reading analog pin A0 (GPIO ");
    Serial.print(kAdcPin);
    Serial.println(").");
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
```