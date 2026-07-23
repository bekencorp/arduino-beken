# WiFi Client Basic Example

## Overview

This example connects to a WiFi network using `WiFiMulti` with a single access point, then repeatedly opens a TCP connection and sends a minimal HTTP GET request. Use it to verify outbound TCP client behavior after WiFi association.

## Features

- `WiFiMulti.addAP()` with one credential set
- Connection wait via `WiFiMulti.run()` until `WL_CONNECTED`
- Extra wait for `WSS_GOT_IP` before starting TCP/HTTP
- Periodic TCP client loop with HTTP GET to a configurable host
- Full HTTP response printed on Serial

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring
- WiFi network with Internet access (default target is `example.com`)

## Configuration

Update the WiFi configuration block at the top of the sketch:

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

In `loop()`, adjust the TCP target if needed:

```cpp
const uint16_t port = 80;
const char *host = "example.com";
const char *url = "/";
```

## How It Works

### Connection Process

1. Register AP credentials with `WiFiMulti.addAP(ssid, password)`
2. Call `WiFiMulti.run()` in a loop until it returns `WL_CONNECTED`
3. Wait until `WiFi.status() == WSS_GOT_IP`
4. Print local IP and gateway

### TCP Client Loop

1. Create a `WiFiClient` and connect to `host:port`
2. Send `GET / HTTP/1.1` with `Host` / `User-Agent` / `Connection: close`
3. Read and print the HTTP response
4. Close the connection and wait 5 seconds before retrying

## Usage

1. Update `WIFI_SSID` and `WIFI_PASS`
2. Keep the default `example.com` target, or change `host` / `url`
3. Upload the sketch
4. Open Serial Monitor at 115200 baud
5. Observe DNS, TCP connect, and HTTP response output

## Expected Output

```text
Waiting for WiFi... .....
Waiting for IP (WSS_GOT_IP)...
WiFi connected
IP address: 192.168.x.x
Gateway: 192.168.x.x
Connecting to example.com:80
Requesting URL: /
HTTP/1.1 200 OK
...
Closing connection.
Waiting 5 seconds before restarting...
```

## Troubleshooting

**Stuck on dots during WiFi connect:**
- Verify SSID and password
- Use WiFiScan to confirm the network is visible

**Connection failed:**
- Confirm Internet access and DNS
- Try a LAN HTTP server if public Internet is blocked

**Timeout waiting for response:**
- Check that port 80 is reachable
- Confirm the host responds to HTTP GET

## Notes

- `WL_CONNECTED` from `WiFiMulti.run()` only means association; wait for `WSS_GOT_IP` before TCP
- This example uses plain `WiFiClient` HTTP, not `HTTPClient`

## Related Examples

- `HTTPClientMinimal` / `BasicHttpClient` - Higher-level HTTPClient examples
- `WiFiClient` - ThingSpeak HTTP API example
- `WiFiMinimal` - Minimal WiFi connection
