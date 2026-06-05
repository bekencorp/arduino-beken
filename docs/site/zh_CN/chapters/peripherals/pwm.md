# PWM

脉宽调制（PWM）通过在 HIGH 和 LOW 状态之间快速切换来控制传递给设备的平均功率。BK7239N 提供 12 个独立的 PWM 通道（PWM0-PWM11），具有可配置的频率和分辨率。<br>
PWM 规格包括：源时钟为 26MHz、默认频率为 1kHz、分辨率为 1-15 位（默认：8 位）、占空比为 0-100%、通道数为 12 个通道（PWM0-PWM11）、最大占空比值为 (2^分辨率 - 1)。

PWM 通道到 GPIO 引脚映射：

| PWM 通道 | GPIO 引脚 |
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

## PWM API 参考

### analogWrite()

设置引脚的 PWM 输出值。

```cpp
void analogWrite(uint8_t pin, int value);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `pin` | `uint8_t` | 支持 PWM 的引脚编号 |
| `value` | `int` | PWM 占空比值（根据分辨率从 0 到 max_duty） |

**注意：** `value` 范围取决于当前分辨率设置。默认 8-bit 分辨率下，有效值为 0-255。

### analogWriteFrequency()

设置引脚的 PWM 频率。

```cpp
void analogWriteFrequency(uint8_t pin, uint32_t frequency);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `pin` | `uint8_t` | 支持 PWM 的引脚编号 |
| `frequency` | `uint32_t` | PWM 频率（单位：Hz，默认：1000） |

### analogWriteResolution()

设置引脚的 PWM 分辨率。

```cpp
void analogWriteResolution(uint8_t pin, uint8_t resolution_bits);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `pin` | `uint8_t` | 支持 PWM 的引脚编号 |
| `resolution_bits` | `uint8_t` | 分辨率位数（1-15，默认：8） |


## PWM 示例

### 基本 PWM 渐变示例

此示例演示了通过控制 PWM 占空比来渐变 LED：

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

**要点：**
1. PWM 频率和分辨率是按引脚配置的
2. 更改频率或分辨率将重新初始化 PWM 通道
3. 最大频率取决于分辨率设置和 26MHz 源时钟
4. 默认行为（不调用 `analogWriteFrequency` 或 `analogWriteResolution`）：
   - 频率：1kHz
   - 分辨率：8-bit (0-255)