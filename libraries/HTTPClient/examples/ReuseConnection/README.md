# ReuseConnection Example

## Overview

This example shows how to reuse a TCP connection across multiple HTTP requests with `HTTPClient::setReuse(true)` (HTTP keep-alive). It issues one GET request every second in `loop()`. When the server supports persistent connections, repeated requests can avoid a full TCP handshake on every round.

## Features

- Connects to WiFi with `WiFiMulti`
- Enables connection reuse in `setup()` via `http.setReuse(true)`
- Uses global `HTTPClient` and `WiFiClient` objects so state can persist across loop iterations
- Waits for `WSS_GOT_IP` on `arduino-beken` before starting HTTP
- Streams the response body to Serial with `writeToStream()`

## Hardware Requirements

- Beken BK7239N or BK7236N board
- USB cable for programming and Serial Monitor
- A reachable WiFi network
- An HTTP server that supports keep-alive (a LAN test server is recommended)

## Configuration

Update the common WiFi configuration block near the top of the sketch:

```cpp
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
```

Change the target URL if needed:

```cpp
const char *httpUrl = "http://example.com/";  // Change this to your HTTP server URL
```

For keep-alive validation, prefer a LAN server (see `test_server/`). You can also override WiFi credentials at compile time with `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"`.

## How It Works

1. Start Serial at 115200 baud
2. Add one AP entry to `WiFiMulti`
3. Call `http.setReuse(true)` in `setup()`
4. Retry until `wifiMulti.run()` reports `WL_CONNECTED`
5. Wait for `WiFi.status() == WSS_GOT_IP`
6. Call `http.begin(client, httpUrl)` and send `GET()`
7. If the response is `HTTP_CODE_OK`, print the body with `writeToStream()`
8. Call `http.end()`; if the server supports keep-alive, the underlying TCP socket may stay open for the next request
9. Wait 1 second and repeat

## Usage

1. Set `WIFI_SSID` and `WIFI_PASS`
2. Point `httpUrl` to a reachable HTTP endpoint that supports keep-alive
3. Upload the sketch
4. Open Serial Monitor at 115200 baud
5. Watch for `[HTTP] GET... code:` once per second

## Expected Output

```text
[SETUP] WAIT 4...
[SETUP] WAIT 3...
[SETUP] WAIT 2...
[SETUP] WAIT 1...
[HTTP] GET... code: 200
<!DOCTYPE html>
...
[HTTP] GET... code: 200
...
```

## Troubleshooting

**`[HTTP] WiFi not connected` repeats**
- Verify SSID and password
- Check signal strength and whether the AP is reachable

**`[HTTP] begin failed`**
- The URL must start with `http://`
- The current example does not enable HTTPS

**`[HTTP] GET... failed, error: ...`**
- Check whether the server is reachable from the board
- Try a simple LAN HTTP server if public internet access is unstable

**Keep-alive does not seem to work**
- Some servers disable keep-alive or close the socket after each response
- Capture traffic or inspect server logs to confirm whether the same TCP session is reused

## Notes

- This example keeps the original once-per-second retry behavior to make reuse easier to observe
- The current `arduino-beken` port uses `http.begin(client, url)` instead of the older single-argument `http.begin(url)`
- Reuse depends on both the server and the network stack; if the server does not support keep-alive, each request still opens a new TCP connection

## Related Examples

- `BasicHttpClient` - Basic HTTP GET example
- `StreamHttpClient` - Read the response body in chunks from the underlying stream
- `HTTPClientMinimal` - Minimal one-shot HTTP GET example
