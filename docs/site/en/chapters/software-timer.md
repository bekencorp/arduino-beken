# Ticker

Software Timer (Ticker) provides lightweight software timer functionality, implemented based on RTOS software timer, supporting periodic and one-shot callbacks.

## Ticker API Reference

### attach

Attach a callback function to be called periodically in seconds.

```cpp
void attach(float seconds, ticker_callback_t callback);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| seconds | float | Period time in seconds (supports decimal) |
| callback | ticker_callback_t | Callback function with signature `void callback(void)` |

### attach_ms

Attach a callback function to be called periodically in milliseconds.

```cpp
void attach_ms(uint32_t milliseconds, ticker_callback_t callback);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| milliseconds | uint32_t | Period time in milliseconds |
| callback | ticker_callback_t | Callback function with signature `void callback(void)` |

### once_ms

Attach a callback function to be called only once (one-shot timer).

```cpp
void once_ms(uint32_t milliseconds, ticker_callback_t callback);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| milliseconds | uint32_t | Delay time in milliseconds |
| callback | ticker_callback_t | Callback function with signature `void callback(void)` |

### detach

Detach the timer, stop the timer and clear the callback function.

```cpp
void detach();
```

### active

Check if the timer is in active state.

```cpp
bool active() const;
```

**Returns:** `true` if timer is active, `false` otherwise.

## Ticker Examples

### Ticker Example

This example demonstrates how to use a Ticker to control an LED with periodic callbacks:

```cpp
#include <Ticker.h>

Ticker ledTicker;
volatile bool led_state = false;

void toggleLed() {
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state ? HIGH : LOW);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(115200);
    Serial.println("arduino-beken ticker blink");

    ledTicker.attach(0.5f, toggleLed);
}

void loop() {
    static unsigned long last_report = 0;
    if (millis() - last_report >= 2000) {
        last_report = millis();
        Serial.print("Ticker active: ");
        Serial.println(ledTicker.active() ? "yes" : "no");
    }
}
```