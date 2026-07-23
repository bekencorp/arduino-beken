# StreamHttpClient Example

## Overview

This example demonstrates HTTP response streaming with `HTTPClient`. After connecting to WiFi with `WiFiMulti`, it sends one HTTP GET request, reads the response body from `getStreamPtr()` in small chunks, and prints the streamed data to Serial.

## Features

- Connects to WiFi with `WiFiMulti`
- Waits for `WSS_GOT_IP` before starting HTTP on `arduino-beken`
- Performs a plain HTTP GET request
- Prints the reported `Content-Length`
- Reads the response body from the raw TCP stream in 128-byte chunks
- Prints the total number of streamed bytes

## Hardware Requirements

- Beken BK7239N or BK7236N board
- USB cable for programming and Serial Monitor
- A reachable WiFi network
- A reachable HTTP server with a stable response body

## Configuration

Update the common WiFi configuration block:

```cpp
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
```

Change the test URL if needed:

```cpp
const char *httpUrl = "http://example.com/";
```

For predictable validation, use a fixed-length response from a LAN HTTP server when available.

## How It Works

1. Start Serial at 115200 baud
2. Add one AP entry to `WiFiMulti`
3. Retry until `wifiMulti.run()` reports `WL_CONNECTED`
4. Wait for `WiFi.status() == WSS_GOT_IP`
5. Call `http.begin(client, httpUrl)` and send `GET()`
6. Read `http.getSize()` as the expected response length
7. Use `http.getStreamPtr()` to read the body in 128-byte chunks
8. Print the streamed data and the final total byte count

## Usage

1. Set `WIFI_SSID` and `WIFI_PASS`
2. Point `httpUrl` to a reachable HTTP endpoint
3. Upload the sketch
4. Open Serial Monitor at 115200 baud
5. Confirm the HTTP code, streamed response, and total byte count

## Expected Output

```text
[SETUP] WAIT 4...
[SETUP] WAIT 3...
[SETUP] WAIT 2...
[SETUP] WAIT 1...
[HTTP] WiFi connected, waiting for IP...
[HTTP] begin...
[HTTP] GET...
[HTTP] GET... code: 200
[HTTP] Content-Length: -1
<!doctype html>...
[HTTP] stream bytes read: ...
[HTTP] connection closed or file end.
```

Note: `example.com` often omits `Content-Length` (reports `-1`, chunked). A fixed-length LAN page makes the byte count easier to verify.


## Troubleshooting

**`[HTTP] begin failed`**
- The URL must start with `http://`
- HTTPS is not enabled in this example

**`GET... failed` with a negative error**
- Check WiFi, DNS, firewall, and HTTP server reachability
- Try a simple LAN HTTP server first

**`stream pointer is null`**
- The TCP stream was not available after the HTTP headers
- Check whether the request already failed earlier

**The byte count does not match `Content-Length`**
- The server may have closed the connection early
- Try a smaller fixed file from a local HTTP server

## Notes

- This example demonstrates streaming instead of loading the full body into a `String`
- A server without `Content-Length` may report `-1`; the loop still reads until the connection ends
- Compared with `getString()`, streaming reduces temporary memory pressure

## Related Examples

- `HTTPClientMinimal` - Minimal one-shot GET validation
- `BasicHttpClient` - Classic retry-based GET example
