# BasicHttpClient Example

## Overview

This example shows a basic HTTP GET flow with `HTTPClient` after connecting to WiFi through `WiFiMulti`. It keeps the same learning goal throughout the sketch: connect, send one GET request, print the status code, and dump the response body when the server replies with `200 OK`.

## Features

- Connects to WiFi with `WiFiMulti`
- Performs a plain HTTP GET request with `HTTPClient`
- Uses `http.begin(WiFiClient&, url)` as required by the current `arduino-beken` API
- Prints success and failure logs to Serial
- Uses `HTTPClient::errorToString()` for readable error output

## Hardware Requirements

- Beken BK7239N or BK7236N board
- USB cable for programming and Serial Monitor
- A reachable WiFi network
- A reachable plain HTTP server

## Configuration

Update the common WiFi configuration block near the top of the sketch:

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

Change the target URL if needed:

```cpp
const char *httpUrl = "http://example.com/";
```

You can also override the WiFi credentials at compile time with `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"`.

## How It Works

1. Start Serial at 115200 baud
2. Add one AP entry to `WiFiMulti`
3. Retry until `wifiMulti.run()` reports `WL_CONNECTED`
4. Wait for `WiFi.status() == WSS_GOT_IP`
5. Create `HTTPClient` and call `http.begin(client, httpUrl)`
6. Call `http.GET()` to send the request
7. Print the HTTP status code
8. If the response is `HTTP_CODE_OK`, read and print the response body with `getString()`
9. Call `http.end()` and try again after 5 seconds

## Usage

1. Set `WIFI_SSID` and `WIFI_PASS`
2. Point `httpUrl` to a reachable HTTP endpoint
3. Upload the sketch
4. Open Serial Monitor at 115200 baud
5. Wait for `[HTTP] GET... code:` or a readable error line

## Expected Output

```text
[SETUP] WAIT 4...
[SETUP] WAIT 3...
[SETUP] WAIT 2...
[SETUP] WAIT 1...
[HTTP] begin...
[HTTP] GET...
[HTTP] GET... code: 200
<!doctype html>
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
- If using a hostname, verify DNS works on your network

**No body is printed even with code 200**
- Some servers may return an empty body
- Try another test URL or a local HTTP test server

## Notes

- This example keeps the loop-based retry behavior from the original sketch
- HTTPS is left as a commented reference only; use `BasicHttpsClient` for TLS
- The current `arduino-beken` port uses `http.begin(client, url)` instead of the older single-argument `http.begin(url)`

## Related Examples

- `HTTPClientMinimal` - Minimal one-shot HTTP GET example
- `BasicHttpsClient` - HTTPS GET with root CA verification
- `WiFiMulti` - Connect to the best AP from a list
