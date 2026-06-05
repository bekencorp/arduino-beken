# Ticker

软件定时器(Ticker)提供轻量级的软件定时功能，基于RTOS软件定时器实现，支持周期性和一次性回调。

## Ticker API 参考

### attach

附加一个回调函数，以秒为单位定期调用。

```cpp
void attach(float seconds, ticker_callback_t callback);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| seconds | float | 周期时间（单位：秒，支持小数） |
| callback | ticker_callback_t | 回调函数，签名为 `void callback(void)` |

### attach_ms

附加一个回调函数，以毫秒为单位定期调用。

```cpp
void attach_ms(uint32_t milliseconds, ticker_callback_t callback);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| milliseconds | uint32_t | 周期时间（单位：毫秒） |
| callback | ticker_callback_t | 回调函数，签名为 `void callback(void)` |

### once_ms

附加一个回调函数，只调用一次（一次性定时器）。

```cpp
void once_ms(uint32_t milliseconds, ticker_callback_t callback);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| milliseconds | uint32_t | 延迟时间（单位：毫秒） |
| callback | ticker_callback_t | 回调函数，签名为 `void callback(void)` |

### detach

分离定时器，停止定时器并清除回调函数。

```cpp
void detach();
```

### active

检查定时器是否处于活动状态。

```cpp
bool active() const;
```

**返回：** 如果定时器处于活动状态返回 `true`，否则返回 `false`。

## Ticker 示例

### Ticker 示例

此示例演示如何使用Ticker通过周期性回调控制LED：

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