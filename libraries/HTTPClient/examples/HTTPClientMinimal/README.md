# HTTPClient Minimal Example

## Overview

This is the smallest HTTP client example on `arduino-beken`: connect to WiFi, perform one HTTP GET with `HTTPClient`, and print the status code plus a short body preview. Use it as the first sketch to verify HTTPClient before running more advanced examples.

## Features

- WiFi connect with wait for `WSS_GOT_IP`
- Plain HTTP GET via `http.begin(WiFiClient&, url)`
- Prints HTTP status code and response body preview
- Error path prints negative error code and `HTTPClient::errorToString()`

## Hardware Requirements

- Beken BK7239N or BK7236N board (bk_idk Arduino platform)
- USB cable for programming and serial monitoring
- WiFi network with internet or LAN HTTP access

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

Change the HTTP target URL if needed (inside `setup()`):

```cpp
const char *url = "http://example.com/";
```

Alternatively, pass `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"` at compile time.

## How It Works

### Connection and Request

1. Initialize serial at 115200 baud
2. Call `WiFi.begin()` and wait until `WSS_GOT_IP`
3. Call `http.begin(client, url)` with an external `WiFiClient`
4. Call `http.GET()` and read the response with `getString()`
5. Call `http.end()` to release the request state

### API Notes

| Item | arduino-beken behavior |
|------|------------------------|
| URL scheme | This example uses `http://` only |
| `begin()` | Requires `WiFiClient&` as first argument |
| Success code | Positive HTTP status (e.g. 200) |
| Failure code | Negative `HTTPC_ERROR_*` value |

## Usage

1. Update `WIFI_SSID` and `WIFI_PASS`
2. Ensure `url` points to a reachable HTTP server
3. Upload the sketch
4. Open Serial Monitor at 115200 baud
5. Confirm WiFi connects and HTTP code is positive

## Expected Output

```text
Connecting to WiFi... .....
IP address: 192.168.x.x
HTTP GET http://example.com/
HTTP code=200
HTTP body length=...
HTTP body preview=<!doctype html>...
```

## Troubleshooting

**Stuck on dots (WiFi never connects):**
- Verify SSID and password
- Use WiFiScan to confirm the network is visible

**HTTP begin failed:**
- URL must start with `http://`
- Check that `HTTPClient` and `WiFiClient` libraries are available

**GET failed with negative error code:**
- WiFi may not have IP yet; confirm `WSS_GOT_IP` before HTTP
- Check DNS, firewall, or try a LAN HTTP server

**HTTP code=200 but empty body:**
- Server may return headers only; try another URL or check serial log

## Notes

- Wait for `WSS_GOT_IP` before starting HTTP
- This example runs one GET in `setup()`; `loop()` is idle
- For HTTPS, see `BasicHttpsClient` (`WiFiClientSecure`)

## Related Examples

- `BasicHttpClient` - Retry-based HTTP GET example
- `BasicHttpsClient` - HTTPS GET with root CA verification
- `WiFiMinimal` - Minimal WiFi connection
- `WiFiClientBasic` - Raw TCP HTTP request with `WiFiClient`
