# GPIO

GPIO (General Purpose Input/Output) pins are the basic building blocks for interacting with external hardware. The BK7239N supports various pin modes and provides APIs for digital input/output operations as well as interrupt handling.

## GPIO API Reference

### Pin Modes

| Mode | Description |
|------|-------------|
| `INPUT` | High-impedance input |
| `OUTPUT` | Push-pull output |
| `INPUT_PULLUP` | Input with internal pull-up resistor |
| `INPUT_PULLDOWN` | Input with internal pull-down resistor |
| `OUTPUT_OPENDRAIN` | Open-drain output |

### pinMode()

Configure the pin mode.

```cpp
void pinMode(uint8_t pin, PinMode mode);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pin` | `uint8_t` | GPIO pin number |
| `mode` | `PinMode` | Pin mode: `INPUT`, `OUTPUT`, `INPUT_PULLUP`, `INPUT_PULLDOWN`, `OUTPUT_OPENDRAIN` |

### digitalWrite()

Set the output level of a pin.

```cpp
void digitalWrite(uint8_t pin, PinStatus value);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pin` | `uint8_t` | GPIO pin number |
| `value` | `PinStatus` | Output level: `HIGH` or `LOW` |

### digitalRead()

Read the input level of a pin.

```cpp
PinStatus digitalRead(uint8_t pin);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `pin` | `uint8_t` | GPIO pin number |

**Return:** `HIGH` if the pin is high, `LOW` if the pin is low.

### Interrupt Trigger Modes

| Mode | Description |
|------|-------------|
| `LOW` | Trigger on low level (hardware supported) |
| `HIGH` | Trigger on high level (hardware supported) |
| `CHANGE` | Trigger on both rising and falling edges (**software emulated**) |
| `FALLING` | Trigger on falling edge (hardware supported) |
| `RISING` | Trigger on rising edge (hardware supported) |

> **Note**: The BK7239N SDK does not natively support both-edge interrupt (CHANGE mode). This implementation simulates both-edge triggering by dynamically switching between rising and falling edge detection when an interrupt occurs.

### attachInterrupt()

Attach an interrupt handler to a pin.

```cpp
void attachInterrupt(pin_size_t interruptNumber, voidFuncPtr callback, PinStatus mode);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `interruptNumber` | `pin_size_t` | GPIO pin number |
| `callback` | `voidFuncPtr` | Interrupt handler function with signature `void callback(void)` |
| `mode` | `PinStatus` | Trigger mode: `LOW`, `HIGH`, `CHANGE`, `FALLING`, `RISING` |

### attachInterruptParam()

Attach an interrupt handler with a parameter to a pin.

```cpp
void attachInterruptParam(pin_size_t interruptNumber, voidFuncPtrParam callback, PinStatus mode, void* param);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `interruptNumber` | `pin_size_t` | GPIO pin number |
| `callback` | `voidFuncPtrParam` | Interrupt handler function with signature `void callback(void*)` |
| `mode` | `PinStatus` | Trigger mode: `LOW`, `HIGH`, `CHANGE`, `FALLING`, `RISING` |
| `param` | `void*` | Parameter to pass to the callback function |

### detachInterrupt()

Detach the interrupt from a pin.

```cpp
void detachInterrupt(pin_size_t interruptNumber);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `interruptNumber` | `pin_size_t` | GPIO pin number |

## GPIO Examples

### Blink LED

```cpp
const int LED_PIN = 13;

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
  delay(500);
}
```

### Interrupt with CHANGE Mode

```cpp
#include <Arduino.h>


const int LED_PIN = LED_BUILTIN;
const int INTERRUPT_PIN = 9;


volatile int interruptCount = 0;
volatile bool ledState = false;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;


void handler() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime < debounceDelay) {
    return;
  }
  lastDebounceTime = currentTime;
  
  interruptCount++;
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}


void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  
  delay(100);
  attachInterrupt(INTERRUPT_PIN, handler, RISING);
  
  Serial.println("Interrupt test started (with debounce)");
}


void loop() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();
    Serial.print("Interrupt count: ");
    Serial.print(interruptCount);
    Serial.print(", LED state: ");
    Serial.println(ledState ? "ON" : "OFF");
  }
}
```