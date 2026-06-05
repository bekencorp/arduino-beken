# UART

UART（通用异步收发器）用于串行通信。
BK7239N芯片包含四个UART接口，支持全双工、异步串行通信，波特率最高可达10Mbps。
其中，2个UART支持硬件流控制，1个支持Flash下载功能。

## UART API 参考

### begin()

使用指定的波特率初始化串行端口。

```cpp
void Serial.begin(unsigned long baud);
void Serial.begin(unsigned long baud, uint16_t config);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `baud` | `unsigned long` | 波特率（例如：9600, 115200） |
| `config` | `uint16_t` | 使用预定义常量的配置（SERIAL_8N1, SERIAL_7E1等） |

配置常量：
- `SERIAL_8N1`: 8个数据位，无奇偶校验，1个停止位
- `SERIAL_8N2`: 8个数据位，无奇偶校验，2个停止位
- `SERIAL_8E1`: 8个数据位，偶校验，1个停止位
- `SERIAL_8E2`: 8个数据位，偶校验，2个停止位
- `SERIAL_8O1`: 8个数据位，奇校验，1个停止位
- `SERIAL_8O2`: 8个数据位，奇校验，2个停止位
- 以及其他使用5、6或7个数据位的配置

### end()

禁用串行端口。

```cpp
void Serial.end();
```

### write()

向串行端口写入原始数据。

```cpp
size_t Serial.write(uint8_t data);
size_t Serial.write(const uint8_t *buf, size_t size);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `data` | `uint8_t` | 要写入的单个字节 |
| `buf` | `const uint8_t*` | 要写入的缓冲区 |
| `size` | `size_t` | 要写入的字节数 |

**返回：** 写入的字节数。

### print()

向串行端口打印数据，不包含换行符。

```cpp
size_t Serial.print(any data);
size_t Serial.print(any data, int base);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `data` | 多种类型 | 要打印的数据（字符串、数字等） |
| `base` | `int` | 整数的进制（DEC, HEX, OCT, BIN） |

**返回：** 打印的字节数。

### println()

向串行端口打印数据并添加换行符。

```cpp
size_t Serial.println(any data);
size_t Serial.println(any data, int base);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `data` | 多种类型 | 要打印的数据（字符串、数字等） |
| `base` | `int` | 整数的进制（DEC, HEX, OCT, BIN） |

**返回：** 打印的字节数。

### available()

获取串行端口可读取的字节数。

```cpp
int Serial.available();
```

**返回：** 可读取的字节数。

### read()

从串行端口读取下一个字节。

```cpp
int Serial.read();
```

**返回：** 串行缓冲区中的下一个字节，如果无数据则返回-1。





### flush()

等待传出串行数据传输完成。

```cpp
void Serial.flush();
```

### availableForWrite()

获取可用于写入串行端口的字节数。

```cpp
int Serial.availableForWrite();
```

**返回：** 可用于写入的字节数（如果已初始化则始终为1，否则为0）。



## UART 示例

### 串行回显示例

此示例演示通过串行接口回显接收到的字符：

```cpp
static const unsigned long kBaudRate = 115200;

void setup() {
    Serial.begin(kBaudRate);
    Serial.println();
    Serial.println("arduino-beken serial echo");
    Serial.println("Type any character to echo it back.");
}

void loop() {
    if (Serial.available() > 0) {
        int value = Serial.read();
        Serial.print("RX: 0x");
        if (value < 16) {
            Serial.print("0");
        }
        Serial.print(value, HEX);
        Serial.print(" '");
        if (value >= 32 && value <= 126) {
            Serial.write((uint8_t)value);
        } else {
            Serial.print(".");
        }
        Serial.println("'");
    }
}
```