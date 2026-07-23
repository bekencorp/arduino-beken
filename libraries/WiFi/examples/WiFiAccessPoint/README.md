# WiFi Access Point Example

## Overview

This example creates a WiFi soft access point (SoftAP) and runs an HTTP server on port 80 to control the onboard LED via browser requests to `/H` (on) and `/L` (off). Clients connect directly to the board without an external router.

## Features

- SoftAP mode via `WiFi.softAP()`
- HTTP server on port 80 with LED control links
- AP IP displayed on Serial (`WiFi.softAPIP()`)
- Open or WPA2-PSK AP (password empty or ≥ 8 characters)

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring
- WiFi-capable phone or PC to connect to the AP

## Configuration

Update the AP configuration block at the top of the sketch:

```cpp
// -------- AP configuration (edit before upload) --------
#ifndef AP_SSID
#define AP_SSID "yourAP"
#endif
#ifndef AP_PASS
#define AP_PASS "yourPassword"
#endif
// ----------------------------------------------------------
```

Alternatively, pass `-DAP_SSID=\"...\" -DAP_PASS=\"...\"` at compile time.

AP password rules:
- Empty string for open AP
- At least 8 characters for secured AP

## How It Works

### SoftAP Setup

1. Configure `LED_BUILTIN` as output
2. Call `WiFi.softAP(AP_SSID, AP_PASS)`
3. Print AP IP address (`192.168.0.1` from `softAPConfig`)

4. Start HTTP server on port 80

### HTTP Server

1. Accept client connections in `loop()`
2. Parse request for `GET /H` or `GET /L` to toggle LED
3. Respond with HTML page containing control links
4. Close client connection

## Usage

1. Update `AP_SSID` and `AP_PASS`
2. Upload the sketch
3. Open Serial Monitor at 115200 baud
4. Note the AP IP address
5. Connect your phone/PC to the AP SSID
6. Open `http://<ap-ip>/` in a browser and control the LED

## Expected Output

```
Configuring access point...
AP IP address: 192.168.0.1
Server started
New Client.
GET / HTTP/1.1
Client Disconnected.
```

## Troubleshooting

**Soft AP creation failed:**
- Password must be empty or at least 8 characters
- Try a different SSID if channel conflict occurs

**Cannot connect to AP from phone/PC:**
- Confirm SSID and password match configuration
- Move closer to the board; SoftAP range is limited

**Browser cannot load page:**
- Use the IP from Serial output, not a guessed address
- Ensure the client is connected to the board's AP, not another network

**LED does not respond:**
- Verify `LED_BUILTIN` matches your board
- Check Serial for `GET /H` or `GET /L` in the request log

## Notes

- SoftAP mode does not use `WSS_GOT_IP` or `WL_CONNECTED`; association is client-to-AP
- Default AP address is set by `softAPConfig` to `192.168.0.1` (`192.168.0.0/24`)
- Only one SoftAP is created; no external router required

## Related Examples

- SimpleWiFiServer - STA mode with similar HTTP LED control
- WiFiMinimal - Station mode connecting to external router
- WiFiScan - Scan nearby networks without hosting AP
