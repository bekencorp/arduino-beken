# HardwareTimer

HardwareTimer类用于配置和控制硬件定时器，支持毫秒和微秒精度的中断回调。BK7239N内置6个32位通用定时器。

## HardwareTimer API 参考

### HardwareTimer()

构造函数，用于创建HardwareTimer实例。

```cpp
HardwareTimer(uint8_t timer_id = 4);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| timer_id | uint8_t | 硬件定时器ID（0-5，默认为4）|

### ~HardwareTimer()

析构函数，清理HardwareTimer实例。

```cpp
~HardwareTimer();
```

### begin

初始化定时器硬件。初始化成功后定时器将自动启动。

```cpp
bool begin();
```

**返回：** 成功返回 `true`，否则返回 `false`。

### attachInterrupt

设置定时器中断回调函数。

```cpp
void attachInterrupt(hardware_timer_callback_t callback);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| callback | hardware_timer_callback_t | 中断回调函数，签名为 `void callback(void)` |

### start

以毫秒为周期启动定时器。

```cpp
bool start(uint32_t period_ms);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| period_ms | uint32_t | 定时器周期（单位：毫秒） |

**返回：** 成功返回 `true`，否则返回 `false`。

### startMicroseconds

以微秒为周期启动定时器。

```cpp
bool startMicroseconds(uint64_t period_us);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| period_us | uint64_t | 定时器周期（单位：微秒） |

**返回：** 成功返回 `true`，否则返回 `false`。

### stop

停止定时器计数。

```cpp
void stop();
```

### read

获取当前定时器计数值。

```cpp
uint32_t read() const;
```

**返回：** 当前计数值。

### period

获取配置的定时器周期（毫秒）。

```cpp
uint32_t period() const;
```

**返回：** 毫秒为单位的定时器周期。

### running

检查定时器是否正在运行。

```cpp
bool running() const;
```

**返回：** 如果定时器正在运行返回 `true`，否则返回 `false`。

### id

获取定时器ID。

```cpp
uint8_t id() const;
```

**返回：** 定时器ID。

### invoke

手动触发定时器回调函数。

```cpp
void invoke() const;
```

## HardwareTimer 示例

### 硬件定时器示例

此示例演示如何使用硬件定时器通过中断回调控制LED：

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