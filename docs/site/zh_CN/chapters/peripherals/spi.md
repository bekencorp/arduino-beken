# SPI

SPI（串行外设接口）是嵌入式系统中广泛使用的同步串行通信协议。它工作在主从配置模式下，包含四个信号：

| 信号 | 名称 | 方向 | 描述 |
|--------|------|-----------|-------------|
| SCK | 串行时钟 | 输出（主设备） | 由主设备产生的时钟信号 |
| MOSI | 主出从入 | 输出（主设备）→ 输入（从设备） | 从主设备传输到从设备的数据 |
| MISO | 主入从出 | 输入（主设备）← 输出（从设备） | 从从设备传输到主设备的数据 |
| SS/CS | 从设备选择/片选 | 输出（主设备） | 从设备选择信号 |

SPI 使用全双工同步通信模式，数据在时钟上升沿或下降沿传输，具体取决于 SPI 模式。协议支持四种模式，由时钟极性（CPOL）和时钟相位（CPHA）定义：

| 模式 | CPOL | CPHA | 描述 |
|------|------|------|-------------|
| SPI_MODE0 | 0 | 0 | 时钟空闲低电平，上升沿采样 |
| SPI_MODE1 | 0 | 1 | 时钟空闲低电平，下降沿采样 |
| SPI_MODE2 | 1 | 0 | 时钟空闲高电平，下降沿采样 |
| SPI_MODE3 | 1 | 1 | 时钟空闲高电平，上升沿采样 |

M39N DevKit 上的默认 SPI 引脚：

| 开发板 | SCK | MISO | MOSI | SS |
|-------|-----|------|------|----|
| M39N DevKit | P14 | P17 | P16 | P15 |

## SPI API 参考

### begin()

使用默认引脚初始化 SPI 总线。

```cpp
void SPI.begin();
void SPI.begin(uint8_t SCK, uint8_t MISO, uint8_t MOSI, uint8_t SS = SS);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `SCK` | `uint8_t` | 串行时钟引脚 |
| `MISO` | `uint8_t` | 主入从出引脚 |
| `MOSI` | `uint8_t` | 主出从入引脚 |
| `SS` | `uint8_t` | 从设备选择引脚（可选） |

### end()

禁用 SPI 总线并释放引脚。

```cpp
void SPI.end();
```

### beginTransaction()

使用特定设置开始 SPI 事务。

```cpp
void SPI.beginTransaction(SPISettings settings);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `settings` | `SPISettings` | 时钟、位序和模式的配置 |

**SPISettings 配置：**

SPISettings 类配置 SPI 通信参数：

```cpp
SPISettings::SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `clock` | `uint32_t` | SPI 时钟频率（Hz） |
| `bitOrder` | `BitOrder` | 位序（`MSBFIRST` 或 `LSBFIRST`） |
| `dataMode` | `uint8_t` | SPI 模式（`SPI_MODE0` 到 `SPI_MODE3`） |


### transfer()

发送和接收数据。

```cpp
uint8_t SPI.transfer(uint8_t val);
uint16_t SPI.transfer16(uint16_t val);
void SPI.transfer(void *buf, size_t count);
void SPI.transfer(void *txBuf, void *rxBuf, size_t count);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `val` | `uint8_t` 或 `uint16_t` | 要发送的值 |
| `buf` | `void*` | 发送和接收数据的缓冲区 |
| `txBuf` | `void*` | 包含要发送数据的缓冲区 |
| `rxBuf` | `void*` | 存储接收到数据的缓冲区 |
| `count` | `size_t` | 要传输的字节数 |

**返回：** 从从设备接收的字节或 16 位值。



## SPI 示例

### SPILoopback 示例

**初始化 API：**
1. `SPI.begin()` - 使用默认引脚初始化 SPI 总线
2. `SPI.beginTransaction(settings)` - 配置 SPI 设置（时钟、位序、模式）

**通信 API（按顺序）：**
1. `SPI.transfer(data)` - 通过 MOSI 发送数据并同时从 MISO 接收数据
2. `SPI.endTransaction()` - 结束当前 SPI 事务

**Loopback 测试说明：**
- 物理连接 MOSI 引脚到 MISO 引脚
- 发送的数据会被接收回来，可验证 SPI 功能
- 使用 `SPISettings(clock, bitOrder, dataMode)` 配置所需的通信参数

```cpp
#include <SPI.h>

static const uint8_t kPattern[] = {0x55, 0xAA, 0x0F, 0xF0};

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("arduino-beken spi loopback");
    Serial.println("Connect MOSI to MISO to observe mirrored bytes.");

    SPI.begin();
}

void loop() {
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    Serial.print("TX/RX:");
    for (size_t i = 0; i < sizeof(kPattern); ++i) {
        uint8_t rx = SPI.transfer(kPattern[i]);
        Serial.print(" ");
        if (kPattern[i] < 16) {
            Serial.print("0");
        }
        Serial.print(kPattern[i], HEX);
        Serial.print("/");
        if (rx < 16) {
            Serial.print("0");
        }
        Serial.print(rx, HEX);
    }
    SPI.endTransaction();

    Serial.println();
    delay(1000);
}
```