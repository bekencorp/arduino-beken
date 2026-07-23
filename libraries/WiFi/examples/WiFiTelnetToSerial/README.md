# WiFi Telnet to Serial Example

## Overview

This example connects to WiFi using `WiFiMulti` with two AP credentials, then exposes a telnet server on port 23 that bridges network traffic to a UART. By default, `UART_BRIDGE` is defined as `Serial` for demo purposes; replace with `Serial1` when a second UART is available on your board.

## Features

- `WiFiMulti` with primary and backup AP failover
- Telnet server on port 23 (`WiFiServer`)
- Bidirectional bridge: telnet client ↔ UART (`UART_BRIDGE`)
- Single client limit (`MAX_SRV_CLIENTS = 1`)
- Connection maintained via `WiFiMulti.run()` in `loop()`

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring
- PC with telnet client on the same LAN

## Configuration

Update the WiFi configuration block:

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

UART bridge selection (in sketch):

```cpp
#define UART_BRIDGE Serial   // Demo: USB serial; use Serial1 when available
```

## How It Works

### Connection Process

1. Register two APs with `wifiMulti.addAP()`
2. Retry `wifiMulti.run()` up to 10 times until `WL_CONNECTED`
3. Initialize `UART_BRIDGE` at 115200 baud
4. Start telnet server on port 23 and print connection instructions

### Telnet Bridge

1. Accept one telnet client at a time
2. Data from telnet → written to `UART_BRIDGE`
3. Data from `UART_BRIDGE` → sent to connected telnet client
4. If WiFi drops, disconnect telnet clients and retry association

## Usage

1. Update WiFi credentials
2. Upload the sketch
3. Open Serial Monitor at 115200 baud
4. Note the printed IP and telnet command
5. From PC: `telnet <board-ip> 23`
6. Type in the telnet session; output appears on `UART_BRIDGE` (Serial in demo mode)

## Expected Output

```
Connecting
Connecting WiFi
WiFi connected IP address: 192.168.1.100
Ready! Use 'telnet 192.168.1.100 23' to connect
New client: 0 192.168.1.50
```

## Troubleshooting

**WiFi connect failed:**
- Verify at least one AP credential is correct
- Check Serial for retry countdown

**Telnet connection refused:**
- Confirm `WL_CONNECTED` and valid IP before telnet
- Check PC firewall for outbound telnet (port 23)

**No data in telnet session:**
- In demo mode, `UART_BRIDGE` is `Serial`; USB Serial Monitor may conflict
- For real UART bridging, change `#define UART_BRIDGE Serial1` and wire external UART

**WiFi not connected in loop:**
- Normal during failover; telnet clients are stopped until reconnected

## Notes

- Uses `WL_CONNECTED` from `WiFiMulti.run()`
- Default `UART_BRIDGE` is `Serial`, not `Serial1`, for boards without a second UART
- Only one telnet client is supported at a time
- `server.setNoDelay(true)` reduces telnet latency

## Related Examples

- WiFiMulti - AP failover without telnet
- SimpleWiFiServer - HTTP server on port 80
- WiFiClientBasic - Outbound TCP client
