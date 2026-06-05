# GPIO

GPIO（通用输入输出）引脚是与外部硬件交互的基本构建块。BK7239N 支持多种引脚模式并提供用于数字输入/输出操作及中断处理的 API。

## GPIO API 参考

### 引脚模式

| 模式 | 描述 |
|------|-------------|
| `INPUT` | 高阻抗输入 |
| `OUTPUT` | 推挽输出 |
| `INPUT_PULLUP` | 带内部上拉电阻的输入 |
| `INPUT_PULLDOWN` | 带内部下拉电阻的输入 |
| `OUTPUT_OPENDRAIN` | 开漏输出 |

### pinMode()

配置引脚模式。

```cpp
void pinMode(uint8_t pin, PinMode mode);
```

| 参数 | 类型 | 描述 |
|------|------|-------------|
| `pin` | `uint8_t` | GPIO 引脚编号 |
| `mode` | `PinMode` | 引脚模式：`INPUT`、`OUTPUT`、`INPUT_PULLUP`、`INPUT_PULLDOWN`、`OUTPUT_OPENDRAIN` |

### digitalWrite()

设置引脚输出电平。

```cpp
void digitalWrite(uint8_t pin, PinStatus value);
```

| 参数 | 类型 | 描述 |
|------|------|-------------|
| `pin` | `uint8_t` | GPIO 引脚编号 |
| `value` | `PinStatus` | 输出电平：`HIGH` 或 `LOW` |

### digitalRead()

读取引脚输入电平。

```cpp
PinStatus digitalRead(uint8_t pin);
```

| 参数 | 类型 | 描述 |
|------|------|-------------|
| `pin` | `uint8_t` | GPIO 引脚编号 |

**返回值：** 如果引脚为高电平则返回 `HIGH`，如果引脚为低电平则返回 `LOW`。

### 中断触发模式

| 模式 | 描述 |
|------|-------------|
| `LOW` | 低电平触发（硬件支持） |
| `HIGH` | 高电平触发（硬件支持） |
| `CHANGE` | 双边沿触发（**软件模拟实现**） |
| `FALLING` | 下降沿触发（硬件支持） |
| `RISING` | 上升沿触发（硬件支持） |

> **注意**：BK7239N SDK 原生不支持双边沿中断（CHANGE 模式），本实现通过在中断触发时动态切换上升沿/下降沿检测来模拟双边沿触发功能。

### attachInterrupt()

为引脚附加中断处理函数。

```cpp
void attachInterrupt(pin_size_t interruptNumber, voidFuncPtr callback, PinStatus mode);
```

| 参数 | 类型 | 描述 |
|------|------|-------------|
| `interruptNumber` | `pin_size_t` | GPIO 引脚编号 |
| `callback` | `voidFuncPtr` | 中断处理函数，签名为 `void callback(void)` |
| `mode` | `PinStatus` | 触发模式：`LOW`、`HIGH`、`CHANGE`、`FALLING`、`RISING` |

### attachInterruptParam()

为引脚附加带参数的中断处理函数。

```cpp
void attachInterruptParam(pin_size_t interruptNumber, voidFuncPtrParam callback, PinStatus mode, void* param);
```

| 参数 | 类型 | 描述 |
|------|------|-------------|
| `interruptNumber` | `pin_size_t` | GPIO 引脚编号 |
| `callback` | `voidFuncPtrParam` | 中断处理函数，签名为 `void callback(void*)` |
| `mode` | `PinStatus` | 触发模式：`LOW`、`HIGH`、`CHANGE`、`FALLING`、`RISING` |
| `param` | `void*` | 传递给回调函数的参数 |

### detachInterrupt()

从引脚分离中断。

```cpp
void detachInterrupt(pin_size_t interruptNumber);
```

| 参数 | 类型 | 描述 |
|------|------|-------------|
| `interruptNumber` | `pin_size_t` | GPIO 引脚编号 |

## GPIO 示例

### 闪烁 LED

```cpp
const int LED_PIN = 13;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
```

### CHANGE 模式中断示例

```cpp
#include <Arduino.h>


const int LED_PIN = LED_BUILTIN;
const int INTERRUPT_PIN = 9;


volatile int interruptCount = 0;
volatile bool ledState = false;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;


void handler() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime < debounceDelay) {
    return;
  }
  lastDebounceTime = currentTime;
  
  interruptCount++;
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  
  delay(100);
  attachInterrupt(INTERRUPT_PIN, handler, RISING);
  
  Serial.println("Interrupt test started (with debounce)");
}


void loop() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();
    Serial.print("Interrupt count: ");
    Serial.print(interruptCount);
    Serial.print(", LED state: ");
    Serial.println(ledState ? "ON" : "OFF");
  }
}
```