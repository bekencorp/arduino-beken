# WiFi Scan Dual Antenna Example

## Overview

This example optionally configures dual-antenna GPIO pins via `WiFi.setDualAntennaConfig()`, then performs periodic WiFi network scans in station mode. On boards without dual-antenna hardware or stub API support, configuration may fail while scanning still works.

## Features

- Station mode (`WIFI_STA`)
- Optional `WiFi.setDualAntennaConfig(ANT1_GPIO, ANT2_GPIO, 0, 0)`
- Periodic `WiFi.scanNetworks()` every 5 seconds
- Prints SSID, RSSI, and encryption indicator for each network

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring
- Dual-antenna hardware (optional; required only for full antenna-switch test)

## Configuration

Adjust GPIO pins to match your board if dual-antenna hardware is present:

```cpp
const uint8_t ANT1_GPIO = 0;
const uint8_t ANT2_GPIO = 1;
```

No WiFi credentials are needed; this example scans only and does not connect.

## How It Works

### Setup

1. Set `WiFi.mode(WIFI_STA)`
2. Call `WiFi.setDualAntennaConfig()` with ANT1/ANT2 GPIO pins
3. Print success or failure message (stub may return failure on unsupported hardware)
4. Call `WiFi.disconnect()` before scanning

### Scan Loop

1. Every 5 seconds, call `WiFi.scanNetworks()`
2. Print count of networks found
3. For each network: index, SSID, RSSI, encryption marker (`*` if secured)

## Usage

1. Adjust `ANT1_GPIO` and `ANT2_GPIO` if you have dual-antenna hardware
2. Upload the sketch
3. Open Serial Monitor at 115200 baud
4. Observe scan results; antenna config message indicates hardware/API support

## Expected Output

**Without dual-antenna support (typical on demo boards):**

```
Dual Antenna configuration failed (stub or unsupported hardware)!
Setup done
scan start
scan done
3 networks found
1: MyRouter (-45) *
2: GuestWiFi (-67) *
3: OpenNet (-72)
```

**With dual-antenna support:**

```
Dual Antenna configuration successfully done!
Setup done
scan start
...
```

## Troubleshooting

**Dual Antenna configuration failed:**
- Expected on boards without dual-antenna hardware or stub implementation
- Scan functionality is independent; continue testing scan output

**no networks found:**
- Ensure WiFi antennas are connected
- Move closer to APs; verify 2.4 GHz networks exist nearby
- Compare with WiFiScan example results

**Scan hangs or crashes:**
- Reduce scan frequency; check for platform-specific scan limits

## Notes

- Does not use `WSS_GOT_IP` or `WL_CONNECTED`; no WiFi association occurs
- `setDualAntennaConfig` may be a stub; failure is not a scan failure
- GPIO pins `0` and `1` are placeholders; change for your schematic
- Encryption type `WAAM_OPEN` indicates open networks (no `*` suffix)

## Related Examples

- WiFiScan - Standard scan without dual-antenna configuration
- WiFiMinimal - Connect to a network after verifying scan results
- WiFiClientEvents - Event API including `ARDUINO_EVENT_WIFI_SCAN_DONE`
