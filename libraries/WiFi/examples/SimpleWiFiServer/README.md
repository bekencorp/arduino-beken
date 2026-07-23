# Simple WiFi Server Example

## Overview

This example connects to a WiFi network as a station (STA), starts an HTTP server on port 80, and controls the onboard LED via browser requests to `/H` (on) and `/L` (off). Use it to verify TCP server functionality after basic WiFi connectivity works.

## Features

- WiFi station mode (`WIFI_STA`)
- Non-blocking wait for `WSS_GOT_IP` before starting the server
- HTTP server on port 80 with simple HTML page
- LED control via `GET /H` and `GET /L`
- Request logging on Serial

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring
- Onboard LED connected to `LED_BUILTIN` (or adjust pin in code)

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

1. Initialize serial at 115200 baud and configure `LED_BUILTIN` as output
2. Call `WiFi.begin(ssid, password)`
3. Poll `WiFi.status()` until `WSS_GOT_IP`
4. Print local IP and call `server.begin()` on port 80

### HTTP Server

1. Accept incoming client connections in `loop()`
2. Parse the HTTP request line for `GET /H` or `GET /L`
3. Toggle the LED and respond with a simple HTML page containing control links
4. Close the client connection

## Usage

1. Update `WIFI_SSID` and `WIFI_PASS`
2. Upload the sketch
3. Open Serial Monitor at 115200 baud
4. Note the printed IP address
5. From a device on the same LAN, open `http://<board-ip>/` in a browser
6. Click the links to turn the LED on or off

## Expected Output

```
Connecting to your-ssid
.....
WiFi connected.
IP address: 192.168.1.100
New Client.
GET / HTTP/1.1
Client Disconnected.
```

## Troubleshooting

**Stuck on dots (never connects):**
- Verify SSID and password
- Check router is 2.4 GHz (if applicable)
- Use WiFiScan to confirm the network is visible

**Browser cannot reach the server:**
- Confirm the PC/phone is on the same WiFi network
- Check firewall settings on the client device
- Verify the IP address from Serial output

**LED does not respond:**
- Confirm `LED_BUILTIN` matches your board schematic
- Check Serial for incoming `GET /H` or `GET /L` requests

## Notes

- Wait for `WSS_GOT_IP`, not only link-up, before starting the HTTP server
- The server handles one client at a time in this minimal example
- HTTP responses are minimal; no CSS or JavaScript is included

## Related Examples

- WiFiMinimal - Basic STA connection and status
- WiFiAccessPoint - SoftAP mode with similar HTTP LED control
- WiFiClient - Outbound HTTP client example
