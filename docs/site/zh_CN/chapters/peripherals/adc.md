# ADC

BK7239N 配备高性能的 AUX ADC 用于模拟信号采集，具有 14 位分辨率 (0-16383)。ADC 支持最多 8 路外部模拟输入通道 (ADC3/4/5/6/12/13/14/15)，同时还提供用于 VBAT 监控、温度传感和调试目的的内部通道。

## ADC API 参考

### analogRead(pin)

从指定引脚读取模拟值。

```cpp
int value = analogRead(uint8_t pin);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `pin` | `uint8_t` | 要读取的模拟引脚 |

**返回：** 14位分辨率下 0 到 16383 之间的模拟值，如果引脚不是模拟引脚则返回 -1。

## ADC 示例

### 基本 ADC 使用

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