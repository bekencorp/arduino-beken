# 2. Platform & Hardware

## Chip Support List

| Chip Series | Key Features | Status |
|------------|-------------|--------|
| BK7239N | Wi-Fi 6 (2.4G+5G), Bluetooth Low Energy 6.0, IEEE 802.15.4, Arm®v8-M STAR-MC1 core at up to 240 MHz | Supported |

## Chip Datasheets

| Chip | Datasheet |
|------|-----------|
| BK7239N | <a href="/_static/BK7239_Datasheet.pdf" target="_blank">BK7239N Datasheet</a> |
| M39N | <a href="/_static/M39N_Datasheet.pdf" target="_blank">M39N Datasheet</a> |

## Peripheral Support List

| Peripheral | BK7239N |
|------------|:-------:|
| GPIO | ✅ |
| ADC | ✅ |
| PWM | ✅ |
| UART | ✅ |
| I2C | ✅ |
| SPI | ✅ |
| Timer | ✅ |
| Wi-Fi | ✅ |
| BLE | ✅ |

- `✅`: Supported through the Arduino Core
- `❌`: Not supported through the Arduino Core yet.
- `➖`: Not available on the SoC

## Module / DevKit Hardware Introduction

### M39N

The M39N module is built around the highly integrated BK7239N wireless SoC, specifically engineered for next generation multi-protocol AIoT applications.

#### Module Photos

<div style="display: flex; gap: 20px; flex-wrap: wrap;">
    <a href="../_static/images/M39N.png" target="_blank"><img src="../_static/images/M39N.png" alt="BK7239N Module View" style="max-width: 700px; height: auto;"></a>
</div>

> **Note:** Due to power-up jitters, the following I/O pins are not recommended for applications sensitive to power up fluctuations, such as LEDs or motor control: GPIO0, GPIO1, GPIO10, and GPIO11.


#### Pinout

**J2**
| Pin # | Name | I/O | Type | Description |
|-------|------|-----|------|-------------|
| 1 | P0 | I/O | Digital/Analog | - GPIO0: general-purpose I/O<br>- ADC12: analog input channel |
| 2 | P1 | I/O | Digital/Analog | - GPIO1: general-purpose I/O<br>- ADC13: analog input channel |
| 3 | P7 | I/O | Digital/Analog | - GPIO7: general-purpose I/O<br>- ADC6: analog input channel<br>- PWM1: PWM channel 1 |
| 4 | P6 | I/O | Digital/Analog | - GPIO6: general-purpose I/O<br>- ADC5: analog input channel<br>- PWM0: PWM channel 0 |
| 5 | P5 | I/O | Digital/Analog | - GPIO5: general-purpose I/O<br>- ADC4: analog input channel<br>- PWM9: PWM channel 9 |
| 6 | P4 | I/O | Digital/Analog | - GPIO4: general-purpose I/O<br>- ADC3: analog input channel |
| 7 | P12 | I/O | Digital/Analog | - GPIO12: general-purpose I/O<br>- ADC14: analog input channel<br>- PWM10: PWM channel 10 |
| 8 | P13 | I/O | Digital/Analog | - GPIO13: general-purpose I/O<br>- ADC15: analog input channel<br>- PWM11: PWM channel 11 |
| 9 | P22 | I/O | Digital | - GPIO22: general-purpose I/O<br>- PWM8: PWM channel 8 |

**J3**
| Pin # | Name | I/O | Type | Description |
|-------|------|-----|------|-------------|
| 1 | P20 | I/O | Digital | - GPIO20: general-purpose I/O<br>- PWM6: PWM channel 6 |
| 2 | P10 | I/O | Digital | - GPIO10: general-purpose I/O<br>- DL_UART_RX: UART flash download receive data input |
| 3 | P11 | I/O | Digital | - GPIO11: general-purpose I/O<br>- DL_UART_TX: UART flash download transmit data output |
| 4 | P19 | I/O | Digital | - GPIO19: general-purpose I/O<br>- I2C_SDA: I2C data line<br>- PWM5: PWM channel 5 |
| 5 | P18 | I/O | Digital | - GPIO18: general-purpose I/O<br>- I2C_SCL: I2C clock line<br>- PWM4: PWM channel 4 |
| 6 | P17 | I/O | Digital | - GPIO17: general-purpose I/O<br>- SPI_MISO: SPI Master In Slave Out |
| 7 | P16 | I/O | Digital | - GPIO16: general-purpose I/O<br>- SPI_MOSI: SPI Master Out Slave In |
| 8 | P15 | I/O | Digital | - GPIO15: general-purpose I/O<br>- SPI_CS: SPI chip select |
| 9 | P14 | I/O | Digital | - GPIO14: general-purpose I/O<br>- SPI_SCK: SPI serial clock |

**H2**
| Pin # | Name | I/O | Type | Description |
|-------|------|-----|------|-------------|
| 1 | GND | - | GND | Ground |
| 2 | RX | I/O | Digital | DL_UART_RX: UART flash download receive data input |
| 3 | TX | I/O | Digital | DL_UART_TX: UART flash download transmit data output |
| 4 | VCC | - | POWER | Module power supply |
| 5 | CEN | - | Analog input | Chip enable, active high |

**H1**
| Pin # | Name | I/O | Type | Description |
|-------|------|-----|------|-------------|
| 1 | GND | - | GND | Ground |
| 2 | RX | I/O | Digital | DL_UART_RX: UART flash download receive data input |
| 3 | TX | I/O | Digital | DL_UART_TX: UART flash download transmit data output |
| 4 | VCC | - | POWER | Module power supply |

> **Note:** H1 and H2 are electrically connected, just using different interface forms to accommodate different boards.