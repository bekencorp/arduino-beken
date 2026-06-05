# UART

UART (Universal Asynchronous Receiver/Transmitter) is used for serial communication.
The BK7239N includes four UART interfaces supporting full duplex, asynchronous serial communication at a baud rate up to 10 Mbps.
Among these, 2 UARTs feature hardware flow control and 1 supports flash download functionality.

## UART API Reference

### begin()

Initialize the serial port with specified baud rate.

```cpp
void Serial.begin(unsigned long baud);
void Serial.begin(unsigned long baud, uint16_t config);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `baud` | `unsigned long` | Baud rate (e.g., 9600, 115200) |
| `config` | `uint16_t` | Configuration using predefined constants (SERIAL_8N1, SERIAL_7E1, etc.) |

Configuration constants:
- `SERIAL_8N1`: 8 data bits, no parity, 1 stop bit
- `SERIAL_8N2`: 8 data bits, no parity, 2 stop bits
- `SERIAL_8E1`: 8 data bits, even parity, 1 stop bit
- `SERIAL_8E2`: 8 data bits, even parity, 2 stop bits
- `SERIAL_8O1`: 8 data bits, odd parity, 1 stop bit
- `SERIAL_8O2`: 8 data bits, odd parity, 2 stop bits
- And more configurations with 5, 6, or 7 data bits

### end()

Disable the serial port.

```cpp
void Serial.end();
```

### write()

Write raw data to the serial port.

```cpp
size_t Serial.write(uint8_t data);
size_t Serial.write(const uint8_t *buf, size_t size);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `uint8_t` | Single byte to write |
| `buf` | `const uint8_t*` | Buffer to write |
| `size` | `size_t` | Number of bytes to write |

**Return:** Number of bytes written.

### print()

Print data to the serial port without a newline character.

```cpp
size_t Serial.print(any data);
size_t Serial.print(any data, int base);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | Various | Data to print (string, number, etc.) |
| `base` | `int` | Number base for integers (DEC, HEX, OCT, BIN) |

**Return:** Number of bytes printed.

### println()

Print data to the serial port followed by a newline character.

```cpp
size_t Serial.println(any data);
size_t Serial.println(any data, int base);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | Various | Data to print (string, number, etc.) |
| `base` | `int` | Number base for integers (DEC, HEX, OCT, BIN) |

**Return:** Number of bytes printed.

### available()

Get the number of bytes available for reading from the serial port.

```cpp
int Serial.available();
```

**Return:** Number of bytes available for reading.

### read()

Read the next byte from the serial port.

```cpp
int Serial.read();
```

**Return:** Next byte in the serial buffer, or -1 if no data is available.





### flush()

Wait for the transmission of outgoing serial data to complete.

```cpp
void Serial.flush();
```

### availableForWrite()

Get the number of bytes available for writing to the serial port.

```cpp
int Serial.availableForWrite();
```

**Return:** Number of bytes available for writing (always 1 if initialized, 0 if not).



## UART Examples

### Serial Echo Example

This example demonstrates echoing characters received via the serial interface:

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