# I2C

I2C (Inter-Integrated Circuit), also known as TWI (Two-Wire Interface), is a synchronous serial communication protocol widely used for connecting low-speed peripherals like sensors, EEPROMs, and displays.<br>
The BK7239N embeds two I2C interfaces that support both master and slave modes with specifications including: 2 interfaces, Master/Slave operating modes, bus speeds of Standard (100kHz), Fast (400kHz), and Fast Plus (1MHz), 7-bit addressing (default), and 16-byte TX/RX buffers. The default I2C pins on the M39N DevKit are SDA on P19 and SCL on P18.

## I2C API Reference

### begin()

Initialize I2C bus.

```cpp
// Master mode (default pins)
void Wire.begin(void);

// Slave mode with 7-bit address
void Wire.begin(uint8_t address);

// Master mode with custom pins and frequency
bool Wire.begin(uint8_t sda, uint8_t scl, uint32_t frequency = 100000);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `uint8_t` | 7-bit slave address (for slave mode) |
| `sda` | `uint8_t` | SDA pin number |
| `scl` | `uint8_t` | SCL pin number |
| `frequency` | `uint32_t` | Clock frequency in Hz (default: 100000) |

**Return:** `true` if initialization successful, `false` otherwise.

### end()

Disable I2C bus.

```cpp
void Wire.end(void);
```

### setClock()

Set I2C clock frequency.

```cpp
void Wire.setClock(uint32_t freq);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `freq` | `uint32_t` | Clock frequency in Hz |

### beginTransmission()

Begin transmission to a slave device.

```cpp
void Wire.beginTransmission(uint8_t address);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `uint8_t` | 7-bit slave address |

### write()

Write data to the I2C transmit buffer.

```cpp
size_t Wire.write(uint8_t data);
size_t Wire.write(const uint8_t *data, size_t size);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t` or `const uint8_t*` | Single byte or data buffer |
| `size` | `size_t` | Number of bytes to write |

**Return:** Number of bytes written.

### endTransmission()

End transmission and send data.

```cpp
uint8_t Wire.endTransmission(bool sendStop);
uint8_t Wire.endTransmission(void);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `sendStop` | `bool` | `true` = send stop condition (default), `false` = send restart |

**Return Values:**
- `0`: Success
- `2`: ACK timeout error
- `4`: Other error
- `5`: Bus busy timeout or SCL timeout error

### requestFrom()

Request bytes from a slave device.

```cpp
size_t Wire.requestFrom(uint8_t address, size_t size, bool sendStop);
size_t Wire.requestFrom(uint8_t address, size_t size);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `address` | `uint8_t` | 7-bit slave address |
| `size` | `size_t` | Number of bytes to request |
| `sendStop` | `bool` | `true` = send stop condition (default), `false` = send restart |

**Return:** Number of bytes received.

### read()

Read one byte from the receive buffer.

```cpp
int Wire.read(void);
```

**Return:** Next byte from buffer, or `-1` if no data available.

### peek()

Peek at the next byte without removing it from the buffer.

```cpp
int Wire.peek(void);
```

**Return:** Next byte from buffer, or `-1` if no data available.

### available()

Check number of available bytes in receive buffer.

```cpp
int Wire.available(void);
```

**Return:** Number of bytes available to read.

### onReceive()

Register a callback function to be called when data is received in slave mode.

```cpp
void Wire.onReceive(void (*callback)(int));
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `callback` | `void(*)(int)` | Function to call when data is received. The parameter is the number of bytes received. |

### onRequest()

Register a callback function to be called when the master requests data from this slave.

```cpp
void Wire.onRequest(void (*callback)(void));
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `callback` | `void(*)(void)` | Function to call when the master requests data. Use `Wire.write()` in the callback to provide data. |


## I2C Examples

### Setting Custom Clock Speed

```cpp
#include <Wire.h>

void setup() {
  Wire.begin();
  Wire.setClock(400000);  // Set I2C clock to 400kHz (Fast mode)
  Serial.begin(115200);
}
```

### I2C Master Mode

**Initialization APIs:**
1. `Wire.begin()` - Initialize I2C as master with default pins

**Communication APIs (in order):**
1. `Wire.beginTransmission(address)` - Start transmission to device at address
2. `Wire.write(data)` - Write data byte(s) to transmit buffer
3. `Wire.endTransmission()` - Send data and stop transmission (or `endTransmission(false)` for restart condition)
4. `Wire.requestFrom(address, quantity)` - Request data from device
5. `Wire.available()` - Check available bytes in receive buffer
6. `Wire.read()` - Read next byte from receive buffer

```cpp
#include <Wire.h>

void setup() {
  Wire.begin();        // Initialize I2C as master
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

### I2C Slave Mode

**Initialization APIs:**
1. `Wire.begin(address)` - Initialize I2C as slave with specific address
2. `Wire.onReceive(callback)` - Register callback for received data
3. `Wire.onRequest(callback)` - Register callback for data requests

**Callback Execution Flow:**
- When master sends data to slave: `onReceive` callback is triggered
- When master requests data from slave: `onRequest` callback is triggered

```cpp
#include <Wire.h>

#define SLAVE_ADDRESS 0x48

void setup() {
  Wire.begin(SLAVE_ADDRESS);     // Join I2C bus with specified address
  Wire.onReceive(receiveEvent);  // Register callback for received data
  Wire.onRequest(requestEvent);  // Register callback for data requests
  Serial.begin(115200);
  Serial.println("I2C Slave ready");
}

void loop() {
  delay(100);  // Main loop does nothing, callbacks handle communication
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
  // Send data back to master when requested
  static uint8_t counter = 0;
  const char* msg = "Hello from Slave! ";
  for (int i = 0; msg[i] != '\0'; i++) {
    Wire.write((uint8_t)msg[i]);
  }
  Wire.write(counter++);
}
```