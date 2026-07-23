# Authorization Example

## Overview

This example shows how to use HTTP Basic Authorization with `HTTPClient` on `arduino-beken`. After connecting to WiFi with `WiFiMulti`, it sends:

1. One GET with `http.setAuthorization(user, password)`
2. One GET without an `Authorization` header

By default it uses the public httpbin Basic Auth endpoint: correct credentials return `200`, and no credentials return `401`.

## Features

- Connects to WiFi with `WiFiMulti`
- Waits for `WSS_GOT_IP` before starting HTTP
- Uses `http.begin(WiFiClient&, url)`
- Demonstrates `setAuthorization(user, password)`
- Prints both success and failure paths

## Hardware Requirements

- Beken BK7239N board
- USB cable for programming and Serial Monitor
- A reachable WiFi network with Internet access (for the default httpbin URL)

## Configuration

Update the common WiFi configuration block near the top of the sketch:

```cpp
// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"  // Change this to your WiFi SSID
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"  // Change this to your WiFi password
#endif
// ----------------------------------------------------------
```

The default Basic Authorization endpoint is already set to an online test URL:

```cpp
#ifndef AUTH_HTTP_URL
// Online Basic Auth test endpoint (user/passwd must match URL path)
#define AUTH_HTTP_URL "http://httpbin.org/basic-auth/user/passwd"
#endif
#ifndef AUTH_USER
#define AUTH_USER "user"  // Change this to the Basic Auth username
#endif
#ifndef AUTH_PASS
#define AUTH_PASS "passwd"  // Change this to the Basic Auth password
#endif
```

Notes:

- `AUTH_HTTP_URL` must start with `http://`
- This example does not enable HTTPS
- Username/password must match the httpbin URL path (`/basic-auth/<user>/<passwd>`)
- The endpoint must use HTTP Basic Auth, not a web form login page
- You can replace httpbin with a local Basic Auth server if needed
- You can also override WiFi credentials at compile time with `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"`

## How It Works

1. Start Serial at 115200 baud
2. Connect with `WiFiMulti`
3. Wait until `WiFi.status() == WSS_GOT_IP`
4. Call `http.begin(client, AUTH_HTTP_URL)`
5. Call `http.setAuthorization(AUTH_USER, AUTH_PASS)` and send `GET()`
6. Print the HTTP status code and body when available
7. Send a second `GET()` without credentials to show the rejected path
8. Repeat after 10 seconds

## Usage

1. Set `WIFI_SSID` and `WIFI_PASS`
2. Keep the default httpbin URL/credentials, or change `AUTH_HTTP_URL` / `AUTH_USER` / `AUTH_PASS`
3. Upload the sketch
4. Open Serial Monitor at 115200 baud
5. Confirm the authorized request returns `200` and the unauthorized request returns `401`

## Expected Output

```text
[SETUP] WAIT 4...
[SETUP] WAIT 3...
[SETUP] WAIT 2...
[SETUP] WAIT 1...
[SETUP] URL: http://httpbin.org/basic-auth/user/passwd
[SETUP] user: user
[HTTP] begin with correct auth
[HTTP] setAuthorization(user, ...)
[HTTP] GET...
[HTTP] GET... code: 200
{
  "authenticated": true,
  "user": "user"
}
[HTTP] begin without auth
[HTTP] no Authorization header
[HTTP] GET...
[HTTP] GET... code: 401
```

## Troubleshooting

**`[HTTP] begin failed`**
- Check that `AUTH_HTTP_URL` starts with `http://`
- Confirm the board can reach the Internet (or your local server)

**Authorized request is still `401`**
- Recheck username and password
- Confirm they match the httpbin URL path
- Confirm the endpoint really uses HTTP Basic Auth, not a web form login page

**Unauthorized request returns `200`**
- The URL is likely a public page
- Point the sketch to a Basic Auth protected path instead

## Notes

- On `arduino-beken`, prefer `http.begin(client, url)` plus `setAuthorization(user, password)`
- URL-embedded credentials such as `http://user:passwd@host/...` are kept only as a commented reference

## Related Examples

- `BasicHttpClient` - Plain HTTP GET without auth
- `BasicHttpsClient` - HTTPS GET with root CA verification
- `StreamHttpClient` - Streamed response body reading
