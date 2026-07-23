# WiFi Client Connect - STA Connect and Disconnect Example

## Overview

This example demonstrates WiFi station (STA) connect and disconnect operations. On boot it connects to a router and prints connection status and IP address; in `loop()` pressing the BOOT button (GPIO0) disconnects WiFi.

Use this sketch to verify the connection state machine and disconnect flow before running WiFiClient or other HTTP examples.

## Features

- Connect to WiFi network in station mode (`WIFI_STA`)
- Log connection progress with `WSS_*` status codes
- Connection timeout management (20 × 500ms)
- GPIO0 (BOOT button) triggers `WiFi.disconnect()`
- Handle connection failures (wrong password, AP not found, DHCP failed)

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitor
- 2.4 GHz WiFi router

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

Compile-time override:

```bash
-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"
```

## How It Works

### 1. WiFi Connection
- Initializes serial at 115200 baud
- Sets GPIO0 as input for BOOT button
- Sets `WiFi.mode(WIFI_STA)` and calls `WiFi.begin()`
- Polls `WiFi.status()` and logs each `WSS_*` state
- Waits for `WSS_GOT_IP` (success) or timeout after 20 retries

### 2. Disconnect
- Monitors BOOT button (GPIO0 LOW) in `loop()`
- Calls `WiFi.disconnect(true, false)` to turn off WiFi while keeping stored config
- Displays disconnect status

### 3. Status Codes

| Value | Constant | Meaning |
|-------|----------|---------|
| 0 | WSS_IDLE | Idle / disconnected |
| 1 | WSS_CONNECTING | Connecting |
| 2 | WSS_PASSWD_WRONG | Wrong password |
| 3 | WSS_NO_AP_FOUND | AP not found |
| 4 | WSS_CONN_FAIL | Connection failed |
| 5 | WSS_CONN_SUCCESS | Link up (DHCP in progress) |
| 6 | WSS_GOT_IP | IP obtained (success) |
| 7 | WSS_DHCP_FAIL | DHCP failed |

> This sketch uses Beken `WSS_*` statuses, not ESP-style `WL_*`.

## Code Structure

```cpp
void setup() {
    // Initialize serial communication
    // Configure BOOT button GPIO
    // Connect to WiFi
    // Poll status until connected or timeout
}

void loop() {
    // Read BOOT button state
    // Disconnect WiFi when button pressed
}
```

## Usage

1. Update WiFi credentials (`WIFI_SSID` and `WIFI_PASS`)
2. Upload code to board
3. Open Serial Monitor (115200 baud)
4. Confirm `[WiFi] WiFi is connected!` and a non-zero IP
5. Press BOOT button and confirm `[WiFi] Disconnected from WiFi!`

## Expected Output

```
[WiFi] Connecting to your-ssid
[WiFi] WiFi Status: 1
...
[WiFi] WiFi is connected!
[WiFi] IP address: 192.168.2.33
[WiFi] Disconnecting from WiFi!
[WiFi] Disconnected from WiFi!
```

## Troubleshooting

**Stuck on status 1 then timeout:**
- Verify WiFi credentials
- Use WiFiScan to confirm the network is visible

**SSID not found:**
- Check SSID spelling or ensure router 2.4 GHz band is enabled

**Wrong password:**
- Verify WPA passphrase

**BOOT button has no effect:**
- Confirm BOOT is wired to GPIO0
- Some boards need `INPUT_PULLUP`; adjust per your hardware guide

## Notes

- Auto reconnect is enabled by default; use `WiFi.setAutoReconnect(false)` to disable
- `WiFi.disconnect(true, false)` turns off WiFi but keeps NVS WiFi configuration
- Connection timeout is 20 × 500ms (about 10 seconds)
- This example uses Beken `WSS_*` status codes, not ESP-style `WL_*`

## Related Examples

- WiFiMinimal - Minimal WiFi connect and status
- WiFiScan - Scan nearby networks
- WiFiClient - HTTP client with ThingSpeak
