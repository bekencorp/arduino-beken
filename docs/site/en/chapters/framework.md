# 3. Arduino Framework

## Arduino Framework Basics

Arduino is an open-source electronics platform based on easy-to-use hardware and software. It is designed to make electronics accessible to artists, designers, hobbyists, and anyone interested in creating interactive objects or environments.

### Arduino Code Architecture

The Arduino programming model is event-driven and follows a simple two-function structure:

- **setup()**: Runs once at startup to initialize hardware, set pin modes, start communication interfaces, etc.
- **loop()**: Runs continuously after setup(), containing the main application logic.

### Key Features

1. **Simplified C/C++ Syntax**: Arduino uses a simplified version of C/C++ that is easier to learn while maintaining the power of traditional programming languages.

2. **Hardware Abstraction**: The Arduino core provides a layer of abstraction over the underlying hardware, making it easy to write code that works across different microcontroller boards.

3. **Event-Driven Programming**: The loop() function runs continuously, allowing for real-time monitoring and response to sensor inputs, button presses, and other events.

4. **Cross-Platform**: Arduino code can be compiled and uploaded to a wide variety of boards, from simple AVR-based boards to more powerful Beken-based boards.

### setup() Function

Called once at startup to initialize hardware and variables.

```cpp
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
}
```

### loop() Function

Runs continuously after setup(), containing the main application logic.

```cpp
void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);
}
```

## Data Types and Basic Syntax

### Supported Data Types

| Type | Size | Range |
|------|------|-------|
| bool | 1 byte | true/false |
| char | 1 byte | -128 to 127 |
| int | 4 bytes | -2^31 to 2^31-1 |
| unsigned int | 4 bytes | 0 to 2^32-1 |
| float | 4 bytes | IEEE 754 single-precision |
| double | 8 bytes | IEEE 754 double-precision |