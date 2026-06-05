# I2C

I2C（Inter-Integrated Circuit），也称为 TWI（Two-Wire Interface），是一种同步串行通信协议，广泛用于连接低速外设，如传感器、EEPROM 和显示器。<br>
BK7239N 嵌入了两个 I2C 接口，支持主机和从机模式，规格包括：2个接口、主机/从机工作模式、总线速度（标准100kHz、快速400kHz、快速+ 1MHz）、7-bit 寻址（默认）以及 16 字节 TX/RX 缓冲区。M39N DevKit 上的默认 I2C 引脚为 SDA 在 P19 和 SCL 在 P18。

## I2C API 参考

### begin()

初始化 I2C 总线。

```cpp
// 主机模式（默认引脚）
void Wire.begin(void);

// 从机模式（7-bit 地址）
void Wire.begin(uint8_t address);

// 主机模式（自定义引脚和频率）
bool Wire.begin(uint8_t sda, uint8_t scl, uint32_t frequency = 100000);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `address` | `uint8_t` | 7-bit 从机地址（从机模式） |
| `sda` | `uint8_t` | SDA 引脚编号 |
| `scl` | `uint8_t` | SCL 引脚编号 |
| `frequency` | `uint32_t` | 时钟频率（单位：Hz，默认：100000） |

**返回：** 初始化成功返回 `true`，否则返回 `false`。

### end()

禁用 I2C 总线。

```cpp
void Wire.end(void);
```

### setClock()

设置 I2C 时钟频率。

```cpp
void Wire.setClock(uint32_t freq);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `freq` | `uint32_t` | 时钟频率（单位：Hz） |

### beginTransmission()

开始向从设备传输数据。

```cpp
void Wire.beginTransmission(uint8_t address);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `address` | `uint8_t` | 7-bit 从机地址 |

### write()

向 I2C 发送缓冲区写入数据。

```cpp
size_t Wire.write(uint8_t data);
size_t Wire.write(const uint8_t *data, size_t size);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `data` | `uint8_t` 或 `const uint8_t*` | 单个字节或数据缓冲区 |
| `size` | `size_t` | 要写入的字节数 |

**返回：** 写入的字节数。

### endTransmission()

结束传输并发送数据。

```cpp
uint8_t Wire.endTransmission(bool sendStop);
uint8_t Wire.endTransmission(void);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `sendStop` | `bool` | `true` = 发送停止条件（默认），`false` = 发送重启 |

**返回值：**
- `0`: 成功
- `2`: ACK 超时错误
- `4`: 其他错误
- `5`: 总线忙超时或 SCL 超时错误

### requestFrom()

从从设备请求字节。

```cpp
size_t Wire.requestFrom(uint8_t address, size_t size, bool sendStop);
size_t Wire.requestFrom(uint8_t address, size_t size);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `address` | `uint8_t` | 7-bit 从机地址 |
| `size` | `size_t` | 请求的字节数 |
| `sendStop` | `bool` | `true` = 发送停止条件（默认），`false` = 发送重启 |

**返回：** 接收到的字节数。

### read()

从接收缓冲区读取一个字节。

```cpp
int Wire.read(void);
```

**返回：** 缓冲区中的下一个字节，如果没有数据则返回 `-1`。

### peek()

查看下一个字节但不移除它。

```cpp
int Wire.peek(void);
```

**返回：** 缓冲区中的下一个字节，如果没有数据则返回 `-1`。

### available()

检查接收缓冲区中可用的字节数。

```cpp
int Wire.available(void);
```

**返回：** 可读取的字节数。

### onReceive()

注册一个回调函数，在从机模式下收到数据时调用。

```cpp
void Wire.onReceive(void (*callback)(int));
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `callback` | `void(*)(int)` | 收到数据时调用的函数。参数为收到的字节数。 |

### onRequest()

注册一个回调函数，当主机从本从机请求数据时调用。

```cpp
void Wire.onRequest(void (*callback)(void));
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `callback` | `void(*)(void)` | 主机请求数据时调用的函数。在回调中使用 `Wire.write()` 提供数据。 |


## I2C 示例

### 设置自定义时钟速度

```cpp
#include <Wire.h>

void setup() {
  Wire.begin();
  Wire.setClock(400000);  // 设置 I2C 时钟为 400kHz（快速模式）
  Serial.begin(115200);
}
```

### I2C 主机模式

**初始化 API：**
1. `Wire.begin()` - 使用默认引脚初始化 I2C 为主机

**通信 API（按顺序）：**
1. `Wire.beginTransmission(address)` - 开始向指定地址的设备传输
2. `Wire.write(data)` - 将数据字节写入发送缓冲区
3. `Wire.endTransmission()` - 发送数据并结束传输（或 `endTransmission(false)` 发送重启条件）
4. `Wire.requestFrom(address, quantity)` - 从设备请求数据
5. `Wire.available()` - 检查接收缓冲区中的可用字节数
6. `Wire.read()` - 从接收缓冲区读取下一个字节

```cpp
#include <Wire.h>

void setup() {
  Wire.begin();        // 初始化 I2C 为主机
  Serial.begin(115200);
}

void loop() {
  // Write to slave device
  Wire.beginTransmission(0x48);  // Slave address (7-bit)
  Wire.write(0x00);              // Register address to write
  Wire.write(0x55);              // Data to write
  Wire.endTransmission();        // End transmission
  
  // Read from slave device
  Wire.beginTransmission(0x48);  // Slave address
//   Wire.write(0x00);              // Register address to read from
  Wire.endTransmission(false);   // Send restart condition
  
  Wire.requestFrom(0x48, 2);     // Request 2 bytes from slave
  if (Wire.available() >= 2) {
    int high = Wire.read();
    int low = Wire.read();
    int value = (high << 8) | low;
    Serial.print("Value: ");
    Serial.println(value);
  }
  
  delay(1000);
}
```

### I2C 从机模式

**初始化 API：**
1. `Wire.begin(address)` - 使用指定地址初始化 I2C 为从机
2. `Wire.onReceive(callback)` - 注册接收数据的回调函数
3. `Wire.onRequest(callback)` - 注册数据请求的回调函数

**回调执行流程：**
- 当主机向从机发送数据时：触发 `onReceive` 回调
- 当主机向从机请求数据时：触发 `onRequest` 回调

```cpp
#include <Wire.h>

#define SLAVE_ADDRESS 0x48

void setup() {
  Wire.begin(SLAVE_ADDRESS);     // 使用指定地址加入 I2C 总线
  Wire.onReceive(receiveEvent);  // 注册接收数据的回调函数
  Wire.onRequest(requestEvent);  // 注册数据请求的回调函数
  Serial.begin(115200);
  Serial.println("I2C Slave ready");
}

void loop() {
  delay(100);  // 主循环不执行任何操作，由回调函数处理通信
}

void receiveEvent(int howMany) {
  Serial.print("Received ");
  Serial.print(howMany);
  Serial.println(" bytes:");
  
  while (Wire.available()) {
    char c = Wire.read();
    Serial.print("  0x");
    Serial.println(c, HEX);
  }
}

void requestEvent() {
  // 当主机请求数据时发送数据
  static uint8_t counter = 0;
  const char* msg = "Hello from Slave! ";
  for (int i = 0; msg[i] != '\0'; i++) {
    Wire.write((uint8_t)msg[i]);
  }
  Wire.write(counter++);
}
```