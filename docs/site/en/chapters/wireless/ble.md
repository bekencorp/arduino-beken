# BLE (Bluetooth Low Energy)

Beken Arduino platforms support Bluetooth Low Energy for short-range wireless communication.

**BLE Capabilities Overview**

| Feature | Description |
|---------|-------------|
| **Bluetooth Version** | Bluetooth Low Energy 6.0 |
| **Operating Modes** | Peripheral |
| **Max Data Rate** | 2 Mbps |
| **Power Consumption** | Ultra-low power |



## BLE API Reference

### begin()

Initialize BLE and start advertising.

```cpp
bool BLE.begin();
```

**Return:** `true` if initialization successful, `false` otherwise.

### end()

Stop BLE.

```cpp
void BLE.end();
```

### connected()

Check if BLE is connected to a central device.

```cpp
bool BLE.connected() const;
```

**Return:** `true` if connected, `false` otherwise.

### disconnect()

Disconnect from central device.

```cpp
bool BLE.disconnect();
```

**Return:** `true` if disconnection initiated successfully, `false` otherwise.

### advertise()

Start advertising.

```cpp
bool BLE.advertise();
```

**Return:** `true` if advertising started successfully, `false` otherwise.

### stopAdvertise()

Stop advertising.

```cpp
void BLE.stopAdvertise();
```

### setLocalName()

Set local device name.

```cpp
bool BLE.setLocalName(const char* localName);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `localName` | `const char*` | Local device name |

**Returns:** `true` if successful, `false` otherwise.

### setDeviceName()

Set device name.

```cpp
void BLE.setDeviceName(const char* deviceName);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `deviceName` | `const char*` | Device name |

### setAdvertisedService()

Set advertised service.

```cpp
bool BLE.setAdvertisedService(BLEService& service);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `service` | `BLEService&` | Service to advertise |

**Return:** `true` if successful, `false` otherwise.

### setAdvertisedServiceUuid()

Set advertised service UUID.

```cpp
void BLE.setAdvertisedServiceUuid(const char* uuid);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `uuid` | `const char*` | Service UUID to advertise |

### valueSize()

Get the size of the characteristic value buffer.

```cpp
int BLECharacteristic::valueSize() const;
```

**Return:** Size of the characteristic value buffer in bytes.

### valueLength()

Get the actual length of the characteristic value.

```cpp
int BLECharacteristic::valueLength() const;
```

**Return:** Actual length of the characteristic value in bytes.

### properties()

Get the properties of the characteristic.

```cpp
unsigned int BLECharacteristic::properties() const;
```

**Return:** Properties of the characteristic.

### addService()

Add a service.

```cpp
void BLE.addService(BLEService& service);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `service` | `BLEService&` | Service to add |

### setManufacturerData()

Set manufacturer data.

```cpp
void BLE.setManufacturerData(const uint8_t* manufacturerData, size_t manufacturerDataLength);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `manufacturerData` | `const uint8_t*` | Manufacturer data |
| `manufacturerDataLength` | `size_t` | Length of manufacturer data |

### BLEService()

Create a new service.

```cpp
BLEService::BLEService(const char* uuid);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `uuid` | `const char*` | Service UUID |

### addCharacteristic()

Add a characteristic to service.

```cpp
void BLEService::addCharacteristic(BLECharacteristic& characteristic);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `characteristic` | `BLECharacteristic&` | Characteristic to add |

### BLECharacteristic()

Create a characteristic.

```cpp
BLECharacteristic::BLECharacteristic(const char* uuid, uint8_t properties, size_t valueSize, bool fixedLength = true);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `uuid` | `const char*` | Characteristic UUID |
| `properties` | `uint8_t` | Characteristic properties |
| `valueSize` | `size_t` | Size of characteristic value |
| `fixedLength` | `bool` | Whether value length is fixed |

### value()

Get characteristic value.

```cpp
BLEStringValueCharacteristic BLECharacteristic::value();
```

**Return:** Characteristic value.

### writeValue()

Write value to characteristic.

```cpp
int BLECharacteristic::writeValue(const uint8_t* value, int length);
int BLECharacteristic::writeValue(uint8_t value);
int BLECharacteristic::writeValue(const char* value);
int BLECharacteristic::writeValue(const String& value);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const uint8_t*`, `uint8_t`, `const char*` or `const String&` | Value to write |
| `length` | `int` | Length of value (when applicable) |

**Return:** Number of bytes written.

### read()

Initiate a read operation on the characteristic.

```cpp
bool BLECharacteristic::read();
```

**Return:** `true` if read operation initiated successfully, `false` otherwise.

### readValue()

Read characteristic value into a buffer.

```cpp
int BLECharacteristic::readValue(uint8_t* buffer, int length) const;
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | `uint8_t*` | Buffer to read value into |
| `length` | `int` | Length of buffer |

**Return:** Number of bytes read.





### value()

Get characteristic raw value pointer.

```cpp
const unsigned char* BLECharacteristic::value() const;
```

**Return:** Pointer to characteristic value.

### setNotifyEnabled()

Enable or disable notification for the characteristic.

```cpp
void BLECharacteristic::setNotifyEnabled(bool enabled);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `enabled` | `bool` | Enable (`true`) or disable (`false`) notifications |

### addDescriptor()

Add a descriptor to the characteristic.

```cpp
void BLECharacteristic::addDescriptor(BLEDescriptor& descriptor);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `descriptor` | `BLEDescriptor&` | Descriptor to add |

### descriptorCount()

Get the number of descriptors in the characteristic.

```cpp
size_t BLECharacteristic::descriptorCount() const;
```

**Return:** Number of descriptors.

### BLE Service API

#### BLEService()

Create a service.

```cpp
BLEService::BLEService(const char* uuid);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `uuid` | `const char*` | Service UUID |

#### uuid()

Get service UUID.

```cpp
const char* BLEService::uuid() const;
```

**Return:** Service UUID.



#### addCharacteristic()

Add a characteristic to service.

```cpp
void BLEService::addCharacteristic(BLECharacteristic& characteristic);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `characteristic` | `BLECharacteristic&` | Characteristic to add |

#### characteristics()

Get the list of characteristics in the service.

```cpp
const std::vector<BLECharacteristic*>& BLEService::characteristics() const;
```

**Return:** List of characteristics in the service.

### BLE Descriptor API

#### BLEDescriptor()

Create a descriptor.

```cpp
BLEDescriptor::BLEDescriptor(const char* uuid);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `uuid` | `const char*` | Descriptor UUID |

#### uuid()

Get descriptor UUID.

```cpp
const char* BLEDescriptor::uuid() const;
```

**Return:** Descriptor UUID.

### Characteristic Properties

| Property | Description |
|----------|-------------|
| `BLERead` | Allow read access |
| `BLEWrite` | Allow write access |
| `BLEWriteWithoutResponse` | Allow write without response |
| `BLENotify` | Allow notifications |
| `BLEIndicate` | Allow indications |
| `BLEBroadcast` | Allow broadcasting |
| `BLEAuthSignedWrite` | Allow authenticated signed writes |
| `BLEExtProp` | Extended properties |

## BLE Examples

### Example: BLE Advertise

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

### Example: BLE Write Test

This example demonstrates bidirectional communication between the central device and the peripheral device. It creates a service with two characteristics that can be written to and read from.

```cpp
/*
  BLEWriteTest - verify App -> device GATT write

  Service UUID:  fff0
  Byte char:     fff1  (Read + Write, 1 byte)
  Text char:     fff2  (Read + Write, up to 20 bytes)

  Test with nRF Connect / LightBlue:
  1. Connect to "ble-write-test"
  2. Open service fff0
  3. Write to fff1 (e.g. 0x55) or fff2 (e.g. "hello")
  4. Tap Read on the same characteristic - value should match
  5. Serial Monitor (115200) prints when the device receives a write
*/

#include <ArduinoBLE.h>

BLEService g_service("fff0");

// App writes a single byte here (0x00 - 0xFF)
BLEByteCharacteristic g_cmd("fff1", BLERead | BLEWrite);

// App writes a short text here
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