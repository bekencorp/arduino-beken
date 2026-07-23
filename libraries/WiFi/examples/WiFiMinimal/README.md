# WiFi Minimal Example

## Overview

This is the smallest WiFi station example: connect to a network, print the assigned IP address, and periodically report connection status in the loop. Use it as the first sketch to verify WiFi on your board before running WiFiScan or WiFiClient.

## Features

- WiFi station mode (`WIFI_STA`)
- Non-blocking `WiFi.begin()` with wait for `WSS_GOT_IP`
- Display local IP via `WiFi.localIP()`
- Periodic status and IP in `loop()`

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring

## Configuration

Update the common WiFi configuration block at the top of the sketch:

```cpp
// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
// ----------------------------------------------------------
```

Alternatively, pass `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"` at compile time.

## How It Works

### Connection Process

1. Initialize serial at 115200 baud
2. Set station mode and call `WiFi.begin(ssid, password)`
3. Poll `WiFi.status()` until `WSS_GOT_IP` (DHCP completed with valid IP)
4. Print IP address once connected
5. In `loop()`, print status code and IP every 5 seconds

### Status Codes (`WF_STATION_STAT_E`)

| Value | Constant | Meaning |
|-------|----------|---------|
| 0 | WSS_IDLE | Idle / disconnected |
| 1 | WSS_CONNECTING | Connecting |
| 6 | WSS_GOT_IP | Connected with IP |

## Usage

1. Update `WIFI_SSID` and `WIFI_PASS`
2. Upload the sketch
3. Open Serial Monitor at 115200 baud
4. Confirm `WiFi connected` and a non-zero IP (not `0.0.0.0`)

## Expected Output

```
Connecting to your-ssid
.....
WiFi connected
IP address: 192.168.2.33
status=6 ip=192.168.2.33
status=6 ip=192.168.2.33
```

## Troubleshooting

**Stuck on dots (never connects):**
- Verify SSID and password
- Check router is 2.4 GHz (if applicable)
- Use WiFiScan to confirm the network is visible

**WiFi connected but IP is 0.0.0.0:**
- DHCP may still be running; wait longer or check router DHCP pool

**WPA3 association fails then succeeds:**
- Normal on mixed WPA2/WPA3 APs; SDK retries with WPA2

## Notes

- Wait for `WSS_GOT_IP`, not only link-up, before using TCP/HTTP
- This example does not use `WiFiClient`; see WiFiClient for HTTP

## Related Examples

- WiFiScan - Scan nearby networks without connecting
- WiFiClient - ThingSpeak HTTP client
