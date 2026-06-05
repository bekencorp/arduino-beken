# Examples

Language: English | [简体中文](README_CN.md)

This directory contains reference Arduino sketches for `arduino-beken`.



## Peripheral Support Matrix

This matrix describes Arduino Core support as exposed to sketches.

| Peripheral | Status |
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


- `✅`: Supported through the Arduino Core
- `❌`: Not supported through the Arduino Core yet.
- `➖`: Not available on the SoC

## Example List

- `ADCReadSerial`: reads `A0` with `analogRead()` and prints raw average/min/max values over serial.
- `BLEAdvertise`: starts a minimal BLE advertiser.
- `BLEWriteTest`: demonstrates BLE write characteristic functionality.
- `Blink`: toggles `LED_BUILTIN`.
- `MultiInoBlink`: shows a sketch split across multiple `.ino` files.
- `MultiSourceBlink`: shows a sketch directory with mixed `.ino` and `.cpp` sources.
- `gpioInterrupt`: demonstrates GPIO interrupt handling.
- `PWMFade`: demonstrates BK PWM output with a fading duty cycle.
- `SPILoopback`: transfers bytes over SPI and prints TX/RX pairs. Connect `MOSI` to `MISO` for loopback.
- `HardwareTimer`: demonstrates BK hardware timer callbacks with `bk_timer_*`.
- `TickerBlink`: blinks with the Arduino-style `Ticker` wrapper.
- `SerialEcho`: echoes bytes from the serial console.
- `WiFiConnect`: connects to a Wi-Fi AP.
- `WiFiScan`: scans for nearby Wi-Fi networks.
- `WireMaster`: acts as I2C master device.
- `WireScanner`: scans the I2C bus for devices.
- `WireSlave`: acts as I2C slave device.