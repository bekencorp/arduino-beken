# 示例

语言: [English](README.md) | 简体中文

此目录包含 `arduino-beken` 的参考 Arduino sketches。



## 外设支持矩阵

这个表反映的是 sketch 可直接使用的 Arduino Core 支持情况。

| 外设 | 状态 |
| --- | --- |
| GPIO | ✅ |
| UART | ✅ |
| ADC | ✅ |
| SPI | ✅ |
| I2C | ✅ |
| Wi-Fi | ✅ |
| BLE | ✅ |
| Ticker / Soft Timer | ✅ |
| Hardware Timer | ✅ |
| PWM | ✅ |


- `✅`：Supported through the Arduino Core
- `❌`：Not supported through the Arduino Core yet.
- `➖`：Not available on the SoC

## 示例列表

- `ADCReadSerial`：使用 `analogRead()` 读取 `A0`，并通过串口打印原始平均值、最小值、最大值。
- `BLEAdvertise`：启动一个最小 BLE 广播示例。
- `BLEWriteTest`：演示 BLE 特性写入功能。
- `Blink`：翻转 `LED_BUILTIN`。
- `MultiInoBlink`：演示一个由多个 `.ino` 文件组成的 sketch。
- `MultiSourceBlink`：演示一个同时包含 `.ino` 和 `.cpp` 源文件的 sketch 目录。
- `gpioInterrupt`：演示 GPIO 中断处理。
- `PWMFade`：演示 BK PWM 输出以及占空比渐变。
- `SPILoopback`：通过 SPI 发送数据并打印 TX/RX 对。做回环测试时请把 `MOSI` 接到 `MISO`。
- `HardwareTimer`：演示使用 `bk_timer_*` 的 BK 硬件定时器回调。
- `TickerBlink`：演示 Arduino 风格的 `Ticker` 包装层。
- `SerialEcho`：串口回显输入字节。
- `WiFiConnect`：连接到 Wi-Fi AP。
- `WiFiScan`：扫描附近的 Wi-Fi 网络。
- `WireMaster`：作为 I2C 主设备。
- `WireScanner`：扫描 I2C 总线上的设备。
- `WireSlave`：作为 I2C 从设备。