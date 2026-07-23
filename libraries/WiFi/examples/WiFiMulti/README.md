# WiFiMulti Example

## Overview

This example demonstrates connecting to the best available access point from a list of credentials using `WiFiMulti`. It registers two APs and connects to whichever is reachable, then monitors the connection in `loop()`.

## Features

- Multiple AP credentials via `WiFiMulti.addAP()`
- Automatic selection of available network
- Connection status via `WiFiMulti.run()` returning `WL_CONNECTED`
- Periodic reconnection check in `loop()`
- Prints connected SSID and IP address

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring
- At least one configured WiFi network in range (two recommended for failover test)

## Configuration

Update the WiFi configuration block with primary and backup AP credentials:

```cpp
// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
#ifndef WIFI_SSID2
#define WIFI_SSID2 "your-backup-ssid"
#endif
#ifndef WIFI_PASS2
#define WIFI_PASS2 "your-backup-password"
#endif
// ----------------------------------------------------------
```

Alternatively, pass `-DWIFI_SSID=...` etc. at compile time.

## How It Works

### Connection Process

1. Register two APs with `wifiMulti.addAP()`
2. Call `wifiMulti.run()` in `setup()` until it returns `WL_CONNECTED`
3. Print connected SSID and local IP

### Connection Monitoring

1. In `loop()`, call `wifiMulti.run()` periodically
2. If not `WL_CONNECTED`, print `WiFi not connected!` and retry after 1 second
3. `WiFiMulti` automatically attempts failover to the other AP when needed

## Usage

1. Configure at least one valid AP; configure a second for failover testing
2. Upload the sketch
3. Open Serial Monitor at 115200 baud
4. Confirm `WiFi connected` with SSID and IP
5. Optionally disable the primary AP to verify failover to the backup

## Expected Output

```
Connecting WiFi...
WiFi connected
SSID: your-ssid
IP address: 192.168.1.100
```

If connection is lost:

```
WiFi not connected!
```

## Troubleshooting

**WiFi not connected on startup:**
- Verify at least one AP SSID/password is correct and in range
- Use WiFiScan to confirm networks are visible

**Fails over slowly:**
- `WiFiMulti.run()` retries on a schedule; wait several seconds after AP change
- Ensure backup AP credentials are correct

**Wrong SSID connected:**
- `WiFiMulti` selects the best available AP by signal/strength; this is expected behavior

## Notes

- Uses `WL_CONNECTED` from `WiFiMulti.run()`, not direct `WSS_GOT_IP` polling
- Add more APs with additional `addAP()` calls if needed
- Keep `wifiMulti.run()` in `loop()` to maintain connection and enable failover

## Related Examples

- WiFiClientBasic - Single AP with `WiFiMulti` plus TCP client
- WiFiTelnetToSerial - `WiFiMulti` with telnet server
- WiFiMinimal - Single AP with `WSS_GOT_IP` polling
