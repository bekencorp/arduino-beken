# BasicHttpsClient Example

## Overview

This example performs an HTTPS GET request with `HTTPClient` and `WiFiClientSecure`. It connects to WiFi through `WiFiMulti`, waits for an IP address, configures a root CA (or optional insecure mode), then prints the HTTP status code and response body.

## Features

- Connects to WiFi with `WiFiMulti`
- Waits for `WSS_GOT_IP` before starting HTTPS
- Uses `WiFiClientSecure` for TLS
- Calls `http.begin(WiFiClientSecure&, httpsUrl)`
- Supports `setCACert()` verification or optional `setInsecure()` debug mode
- Prints success and failure logs to Serial

## Hardware Requirements

- Beken BK7239N or BK7236N board
- USB cable for programming and Serial Monitor
- A reachable WiFi network with Internet access (or a LAN HTTPS server)

## Configuration

Update the WiFi credentials:

```cpp
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
```

Update the HTTPS target if needed:

```cpp
#define HTTPS_URL "https://valid-isrgrootx1.letsencrypt.org/"
```

The sketch ships with **ISRG Root X1**, which matches the default Let's Encrypt test URL. If you change the URL to a site signed by another CA, replace `rootCACertificate` with that CA's PEM.

For TLS bring-up only, you can temporarily disable verification:

```cpp
#define HTTPS_INSECURE 1
```

Do not leave `HTTPS_INSECURE` enabled in production.

## How It Works

1. Start Serial at 115200 baud
2. Add one AP entry to `WiFiMulti`
3. Wait until `wifiMulti.run()` reports `WL_CONNECTED`
4. Wait for `WiFi.status() == WSS_GOT_IP`
5. Create `WiFiClientSecure` and call `setCACert()` (or `setInsecure()`)
6. Call `https.begin(*client, HTTPS_URL)` and `https.GET()`
7. Print the status code and body when the response is `200` or `301`
8. Destroy `HTTPClient` before deleting the secure client, then retry after 10 seconds

## Usage

1. Set `WIFI_SSID` and `WIFI_PASS`
2. Confirm `HTTPS_URL` and the root CA match
3. Upload the sketch
4. Open Serial Monitor at 115200 baud
5. Wait for `[HTTPS] GET... code:` or a readable error line

## Expected Output

```text
[SETUP] WAIT 4...
[SETUP] WAIT 3...
[SETUP] WAIT 2...
[SETUP] WAIT 1...
[HTTPS] begin...
[HTTPS] GET...
[HTTPS] GET... code: 200
...
Waiting 10s before the next round...
```

## Troubleshooting

**`[HTTPS] WiFi not connected` repeats**
- Verify SSID and password
- Check whether the AP is reachable

**`[HTTPS] Unable to connect` or begin fails**
- Confirm the URL starts with `https://`
- Confirm `WiFiClientSecure` is available on the platform

**`[HTTPS] GET... failed, error: ...`**
- Check DNS and Internet access from the board
- If certificate verification fails, confirm the root CA matches the server chain
- For bring-up only, try `#define HTTPS_INSECURE 1`

**Low memory / TLS handshake failures**
- HTTPS uses more RAM than plain HTTP
- Prefer a short response page for first validation

## Notes

- This example keeps the original loop-based retry behavior
- Certificate verification is enabled by default
- `HTTPS_INSECURE` is for debugging only

## Related Examples

- `BasicHttpClient` - Plain HTTP GET example
- `Authorization` - HTTP Basic Auth example
- `ReuseConnection` - HTTP keep-alive reuse example
