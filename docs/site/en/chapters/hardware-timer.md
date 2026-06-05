# HardwareTimer

HardwareTimer class is used to configure and control hardware timers, supporting millisecond and microsecond precision interrupt callbacks. BK7239N has 6 32-bit general-purpose timers built-in.

## HardwareTimer API Reference

### HardwareTimer()

Constructor to create a HardwareTimer instance.

```cpp
HardwareTimer(uint8_t timer_id = 4);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| timer_id | uint8_t | Hardware timer ID (0-5, default is 4) |

### ~HardwareTimer()

Destructor to clean up the HardwareTimer instance.

```cpp
~HardwareTimer();
```

### begin

Initialize timer hardware. Timer will automatically start after successful initialization.

```cpp
bool begin();
```

**Returns:** `true` if successful, `false` otherwise.

### attachInterrupt

Set timer interrupt callback function.

```cpp
void attachInterrupt(hardware_timer_callback_t callback);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| callback | hardware_timer_callback_t | Interrupt callback function with signature `void callback(void)` |

### start

Start the timer with period in milliseconds.

```cpp
bool start(uint32_t period_ms);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| period_ms | uint32_t | Timer period in milliseconds |

**Returns:** `true` if successful, `false` otherwise.

### startMicroseconds

Start the timer with period in microseconds.

```cpp
bool startMicroseconds(uint64_t period_us);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| period_us | uint64_t | Timer period in microseconds |

**Returns:** `true` if successful, `false` otherwise.

### stop

Stop the timer counter.

```cpp
void stop();
```

### read

Get the current timer counter value.

```cpp
uint32_t read() const;
```

**Returns:** Current counter value.

### period

Get the configured timer period (milliseconds).

```cpp
uint32_t period() const;
```

**Returns:** Timer period in milliseconds.

### running

Check if the timer is currently running.

```cpp
bool running() const;
```

**Returns:** `true` if timer is running, `false` otherwise.

### id

Get the timer ID.

```cpp
uint8_t id() const;
```

**Returns:** Timer ID.

### invoke

Manually trigger the timer callback function.

```cpp
void invoke() const;
```

## HardwareTimer Examples

### Hardware Timer Example

This example demonstrates how to use a hardware timer to control an LED with interrupt callbacks:

```cpp
#include <Arduino.h>
static HardwareTimer g_timer(4);
static constexpr uint32_t kTimerPeriodMs = 250;

volatile uint32_t g_tick_count = 0;

void onHardwareTimer() {
    ++g_tick_count;
    digitalWrite(LED_BUILTIN, (g_tick_count & 1U) ? HIGH : LOW);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("arduino-beken hardware timer");
    Serial.print("Starting timer ");
    Serial.print(g_timer.id());
    Serial.print(" with period ");
    Serial.print(kTimerPeriodMs);
    Serial.println(" ms.");

    if (!g_timer.begin()) {
        Serial.println("HardwareTimer.begin() failed");
        return;
    }

    g_timer.attachInterrupt(onHardwareTimer);
    if (!g_timer.start(kTimerPeriodMs)) {
        Serial.println("HardwareTimer.start() failed");
        return;
    }
}

void loop() {
    static unsigned long last_report = 0;
    if (millis() - last_report < 1000) {
        delay(10);
        return;
    }

    last_report = millis();

    Serial.print("ticks=");
    Serial.print(g_tick_count);
    Serial.print(" counter=");
    Serial.print(g_timer.read());
    Serial.print(" running=");
    Serial.println(g_timer.running() ? "yes" : "no");
}
```