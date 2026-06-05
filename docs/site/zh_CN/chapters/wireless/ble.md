# BLE (蓝牙低功耗)

Beken Arduino 平台支持蓝牙低功耗（BLE）用于短距离无线通信。

**BLE 功能概述**

| 特性 | 描述 |
|---------|-------------|
| **蓝牙版本** | Bluetooth Low Energy 6.0 |
| **工作模式** | 外设模式 |
| **最大数据速率** | 2 Mbps |
| **功耗** | 超低功耗 |



## BLE API 参考

### begin()

初始化 BLE 并开始广播。

```cpp
bool BLE.begin();
```

**返回：** 如果初始化成功则返回 `true`，否则返回 `false`。

### end()

停止 BLE。

```cpp
void BLE.end();
```

### connected()

检查 BLE 是否连接到中心设备。

```cpp
bool BLE.connected() const;
```

**返回：** 如果已连接则返回 `true`，否则返回 `false`。

### disconnect()

断开与中心设备的连接。

```cpp
bool BLE.disconnect();
```

**返回：** 如果断开连接成功则返回 `true`，否则返回 `false`。

### advertise()

开始广播。

```cpp
bool BLE.advertise();
```

**返回：** 如果广播成功启动则返回 `true`，否则返回 `false`。

### stopAdvertise()

停止广播。

```cpp
void BLE.stopAdvertise();
```

### setLocalName()

设置本地设备名称。

```cpp
bool BLE.setLocalName(const char* localName);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `localName` | `const char*` | 本地设备名称 |

**返回：** 成功返回 `true`，失败返回 `false`。

### setDeviceName()

设置设备名称。

```cpp
void BLE.setDeviceName(const char* deviceName);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `deviceName` | `const char*` | 设备名称 |

### setAdvertisedService()

设置广播的服务。

```cpp
void BLE.setAdvertisedService(BLEService& service);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `service` | `BLEService&` | 要广播的服务 |

### setAdvertisedServiceUuid()

设置广播的服务 UUID。

```cpp
void BLE.setAdvertisedServiceUuid(const char* uuid);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `uuid` | `const char*` | 要广播的服务 UUID |

### addService()

添加服务。

```cpp
void BLE.addService(BLEService& service);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `service` | `BLEService&` | 要添加的服务 |

### setManufacturerData()

设置制造商数据。

```cpp
void BLE.setManufacturerData(const uint8_t* manufacturerData, size_t manufacturerDataLength);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `manufacturerData` | `const uint8_t*` | 制造商数据 |
| `manufacturerDataLength` | `size_t` | 制造商数据长度 |

### BLEService()

创建新服务。

```cpp
BLEService::BLEService(const char* uuid);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `uuid` | `const char*` | 服务 UUID |

### addCharacteristic()

向服务添加特性。

```cpp
void BLEService::addCharacteristic(BLECharacteristic& characteristic);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `characteristic` | `BLECharacteristic&` | 要添加的特性 |

### BLECharacteristic()

创建特性。

```cpp
BLECharacteristic::BLECharacteristic(const char* uuid, uint8_t properties, size_t valueSize, bool fixedLength = true);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `uuid` | `const char*` | 特性 UUID |
| `properties` | `uint8_t` | 特性属性 |
| `valueSize` | `size_t` | 特性值大小 |
| `fixedLength` | `bool` | 值长度是否固定 |

### value()

获取特性值。

```cpp
BLEStringValueCharacteristic BLECharacteristic::value();
```

**返回：** 特性值。

### writeValue()

向特性写入值。

```cpp
int BLECharacteristic::writeValue(const uint8_t* value, int length);
int BLECharacteristic::writeValue(uint8_t value);
int BLECharacteristic::writeValue(const char* value);
int BLECharacteristic::writeValue(const String& value);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `value` | `const uint8_t*`, `uint8_t`, `const char*` 或 `const String&` | 要写入的值 |
| `length` | `int` | 值的长度（适用时） |

**返回：** 写入的字节数。

### readValue()

将特性值读入缓冲区。

```cpp
int BLECharacteristic::readValue(uint8_t* buffer, int length) const;
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `buffer` | `uint8_t*` | 用于读取值的缓冲区 |
| `length` | `int` | 缓冲区长度 |

**返回：** 读取的字节数。

### valueSize()

获取特性值缓冲区的大小。

```cpp
int BLECharacteristic::valueSize() const;
```

**返回：** 特性值缓冲区的大小（以字节为单位）。

### valueLength()

获取特性的实际值长度。

```cpp
int BLECharacteristic::valueLength() const;
```

**返回：** 特性值的实际长度（以字节为单位）。

### properties()

获取特性的属性。

```cpp
unsigned int BLECharacteristic::properties() const;
```

**返回：** 特性的属性。

### value()

获取特性原始值指针。

```cpp
const unsigned char* BLECharacteristic::value() const;
```

**返回：** 指向特性值的指针。

### setNotifyEnabled()

启用或禁用特性的通知。

```cpp
void BLECharacteristic::setNotifyEnabled(bool enabled);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `enabled` | `bool` | 启用 (`true`) 或禁用 (`false`) 通知 |

### addDescriptor()

向特性添加描述符。

```cpp
void BLECharacteristic::addDescriptor(BLEDescriptor& descriptor);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `descriptor` | `BLEDescriptor&` | 要添加的描述符 |

### descriptorCount()

获取特性中的描述符数量。

```cpp
size_t BLECharacteristic::descriptorCount() const;
```

**返回：** 描述符数量。

### BLE 服务 API

#### BLEService()

创建服务。

```cpp
BLEService::BLEService(const char* uuid);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `uuid` | `const char*` | 服务 UUID |

#### uuid()

获取服务 UUID。

```cpp
const char* BLEService::uuid() const;
```

**返回：** 服务 UUID。



#### addCharacteristic()

向服务添加特性。

```cpp
void BLEService::addCharacteristic(BLECharacteristic& characteristic);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `characteristic` | `BLECharacteristic&` | 要添加的特性 |

#### characteristics()

获取服务中的特性列表。

```cpp
const std::vector<BLECharacteristic*>& BLEService::characteristics() const;
```

**返回：** 服务中的特性列表。

### BLE 描述符 API

#### BLEDescriptor()

创建描述符。

```cpp
BLEDescriptor::BLEDescriptor(const char* uuid);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `uuid` | `const char*` | 描述符 UUID |

#### uuid()

获取描述符 UUID。

```cpp
const char* BLEDescriptor::uuid() const;
```

**返回：** 描述符 UUID。



### 特性属性

| 属性 | 描述 |
|----------|-------------|
| `BLERead` | 允许读取访问 |
| `BLEWrite` | 允许写入访问 |
| `BLEWriteWithoutResponse` | 允许无需响应的写入 |
| `BLENotify` | 允许通知 |
| `BLEIndicate` | 允许指示 |
| `BLEBroadcast` | 允许广播 |
| `BLEAuthSignedWrite` | 允许认证签名写入 |
| `BLEExtProp` | 扩展属性 |

## BLE 示例

### 示例：BLE 广播

```cpp
#include <ArduinoBLE.h>

static const uint8_t kManufacturerData[] = {0x34, 0x12, 0xBE, 0x4B, 0x01};
BLEService g_service("180F");
BLEByteCharacteristic g_level("2A19", BLERead | BLENotify);

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("arduino-beken ble advertise");

    if (!BLE.begin()) {
        Serial.println("BLE.begin() failed");
        return;
    }

    BLE.setDeviceName("arduino-beken");
    BLE.setLocalName("arduino-beken");
    BLE.setManufacturerData(kManufacturerData, sizeof(kManufacturerData));
    g_service.addCharacteristic(g_level);
    BLE.addService(g_service);
    g_level.writeValue(static_cast<uint8_t>(42));

    if (BLE.advertise()) {
        Serial.println("BLE advertising started");
    } else {
        Serial.println("BLE advertising failed");
    }
}

void loop() {
    static uint8_t level = 42;
    g_level.writeValue(level++);
    Serial.print("Advertising, battery level=");
    Serial.println(level);
    delay(2000);
}
```

### 示例：BLE 写入测试

此示例演示中心设备和外围设备之间的双向通信。它创建一个具有两个可写可读特性的服务。

```cpp
/*
  BLEWriteTest - 验证 App -> 设备 GATT 写入

  服务 UUID:  fff0
  字节特性:     fff1  (读 + 写, 1 字节)
  文本特性:     fff2  (读 + 写, 最多 20 字节)

  使用 nRF Connect / LightBlue 测试:
  1. 连接到 "ble-write-test"
  2. 打开服务 fff0
  3. 向 fff1 写入 (例如 0x55) 或向 fff2 写入 (例如 "hello")
  4. 点击同一特性上的读取 - 值应该匹配
  5. 串口监视器 (115200) 在设备接收到写入时打印
*/

#include <ArduinoBLE.h>

BLEService g_service("fff0");

// App 在这里写入单个字节 (0x00 - 0xFF)
BLEByteCharacteristic g_cmd("fff1", BLERead | BLEWrite);

// App 在这里写入短文本
BLEStringCharacteristic g_text("fff2", BLERead | BLEWrite, 20);

static uint8_t s_last_cmd = 0xFF;
static String s_last_text;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("BLEWriteTest starting...");

  g_service.addCharacteristic(g_cmd);
  g_service.addCharacteristic(g_text);
  BLE.addService(g_service);

  BLE.setDeviceName("ble-write-test");
  BLE.setLocalName("ble-write-test");
  BLE.setAdvertisedService(g_service);

  g_cmd.writeValue(static_cast<uint8_t>(0));
  g_text.writeValue("");

  if (!BLE.begin()) {
    Serial.println("BLE.begin() failed");
    return;
  }

  if (!BLE.advertise()) {
    Serial.println("BLE.advertise() failed");
    return;
  }

  Serial.println("Advertising as \"ble-write-test\"");
  Serial.println("Service fff0:");
  Serial.println("  fff1 = byte (Write then Read to verify)");
  Serial.println("  fff2 = text (Write then Read to verify)");
  Serial.println();
}

void loop() {
  const uint8_t cmd = g_cmd.value();
  if (cmd != s_last_cmd) {
    s_last_cmd = cmd;
    Serial.print("[fff1] App wrote byte: 0x");
    if (cmd < 0x10) {
      Serial.print('0');
    }
    Serial.print(cmd, HEX);
    Serial.print(" (");
    Serial.print(cmd);
    Serial.println(')');
  }

  const String text = g_text.value();
  if (text != s_last_text) {
    s_last_text = text;
    Serial.print("[fff2] App wrote text: \"");
    Serial.print(text);
    Serial.println('"');
  }

  if (BLE.connected()) {
    static unsigned long last_hint_ms = 0;
    const unsigned long now = millis();
    if (now - last_hint_ms >= 10000UL) {
      last_hint_ms = now;
      Serial.println("(connected - use Write then Read on fff1/fff2)");
    }
  }

  delay(50);
}
```