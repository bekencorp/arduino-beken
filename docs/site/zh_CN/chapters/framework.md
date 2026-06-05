# 3. Arduino 框架

## Arduino 框架基础

Arduino 是一个基于易用硬件和软件的开源电子平台。它旨在让艺术家、设计师、爱好者以及任何对创建交互式对象或环境感兴趣的人都能接触到电子技术。

### Arduino 代码架构

Arduino 编程模型是事件驱动的，遵循简单的双函数结构：

- **setup()**：在启动时运行一次，用于初始化硬件、设置引脚模式、启动通信接口等。
- **loop()**：在 setup() 之后持续运行，包含主要应用逻辑。

### 主要特性

1. **简化的 C/C++ 语法**：Arduino 使用简化版的 C/C++，更容易学习，同时保持传统编程语言的强大功能。

2. **硬件抽象**：Arduino 核心提供了对底层硬件的抽象层，使编写跨不同微控制器板的代码变得容易。

3. **事件驱动编程**：loop() 函数持续运行，允许实时监控和响应传感器输入、按钮按下和其他事件。

4. **跨平台**：Arduino 代码可以编译并上传到各种开发板，从简单的 AVR 开发板到更强大的 Beken 开发板。

### setup() 函数

在启动时调用一次，用于初始化硬件和变量。

```cpp
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
}
```

### loop() 函数

在 setup() 之后持续运行，包含主要应用逻辑。

```cpp
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
```

## 数据类型和基本语法

### 支持的数据类型

| 类型 | 大小 | 范围 |
|------|------|-------|
| bool | 1 byte | true/false |
| char | 1 byte | -128 to 127 |
| int | 4 bytes | -2^31 to 2^31-1 |
| unsigned int | 4 bytes | 0 to 2^32-1 |
| float | 4 bytes | IEEE 754 单精度 |
| double | 8 bytes | IEEE 754 双精度 |