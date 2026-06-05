# PWM

Pulse Width Modulation (PWM) allows you to control the average power delivered to a device by rapidly switching between HIGH and LOW states. The BK7239N provides 12 independent PWM channels (PWM0-PWM11) with configurable frequency and resolution.<br>
The PWM specifications include: Source Clock of 26MHz, Default Frequency of 1kHz, Resolution from 1-15 bits (default: 8 bits), Duty Cycle from 0-100%, 12 channels (PWM0-PWM11), and Max Duty Value of (2^resolution - 1).

PWM Channel to GPIO Pin Mapping:

| PWM Channel | GPIO Pin |
|-------------|----------|
| PWM0 | GPIO_6 |
| PWM1 | GPIO_7 |
| PWM2 | GPIO_8 |
| PWM3 | GPIO_9 |
| PWM4 | GPIO_18 |
| PWM5 | GPIO_19 |
| PWM6 | GPIO_20 |
| PWM7 | GPIO_21 |
| PWM8 | GPIO_22 |
| PWM9 | GPIO_5 |
| PWM10 | GPIO_12 |
| PWM11 | GPIO_13 |

## PWM API Reference

### analogWrite()

Set PWM output value on a pin.

```cpp
void analogWrite(uint8_t pin, int value);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pin` | `uint8_t` | PWM-capable pin number |
| `value` | `int` | PWM duty cycle value (0 to max_duty based on resolution) |

**Note:** The `value` range depends on the current resolution setting. With default 8-bit resolution, valid values are 0-255.

### analogWriteFrequency()

Set PWM frequency for a pin.

```cpp
void analogWriteFrequency(uint8_t pin, uint32_t frequency);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pin` | `uint8_t` | PWM-capable pin number |
| `frequency` | `uint32_t` | PWM frequency in Hz (default: 1000) |

### analogWriteResolution()

Set PWM resolution for a pin.

```cpp
void analogWriteResolution(uint8_t pin, uint8_t resolution_bits);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pin` | `uint8_t` | PWM-capable pin number |
| `resolution_bits` | `uint8_t` | Resolution in bits (1-15, default: 8) |


## PWM Examples

### Basic PWM Fade Example

This example demonstrates fading an LED by controlling the PWM duty cycle:

```cpp
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
```

**Key Points:**
1. PWM frequency and resolution are configured per-pin
2. Changing frequency or resolution will reinitialize the PWM channel
3. The maximum frequency depends on the resolution setting and the 26MHz source clock
4. Default behavior (without calling `analogWriteFrequency` or `analogWriteResolution`):
   - Frequency: 1kHz
   - Resolution: 8-bit (0-255)