# WiFi Client Events Example

## Overview

This example demonstrates the simplified `WiFi.onEvent()` API on arduino-beken. It registers general and event-specific callbacks for WiFi station events, connects to a network, and prints event notifications on Serial as association and IP assignment progress.

## Features

- General handler `WiFiEvent()` for all WiFi events
- Specific handler `WiFiGotIP()` registered for `ARDUINO_EVENT_WIFI_STA_GOT_IP` only
- Simplified event type: `arduino_event_id_t`
- Covers scan, start/stop, connect, disconnect, and GOT_IP events

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring
- 2.4 GHz WiFi router

## Configuration

Update the common WiFi configuration block:

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

## How It Works

### Event Registration

1. Call `WiFi.disconnect(true)` and short delay to reset state
2. Register `WiFi.onEvent(WiFiEvent)` for all events
3. Register `WiFi.onEvent(WiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP)` for IP-specific handler
4. Call `WiFi.begin(ssid, password)`

### Event Handlers

| Event | Handler Output |
|-------|----------------|
| `ARDUINO_EVENT_WIFI_STA_START` | WiFi client started |
| `ARDUINO_EVENT_WIFI_STA_CONNECTED` | Connected to access point |
| `ARDUINO_EVENT_WIFI_STA_GOT_IP` | Obtained IP address (+ dedicated handler) |
| `ARDUINO_EVENT_WIFI_STA_DISCONNECTED` | Disconnected from AP |
| `ARDUINO_EVENT_WIFI_SCAN_DONE` | Completed scan (if triggered) |

### Main Loop

- `loop()` only delays; all connection progress is reported via callbacks

## Usage

1. Update `WIFI_SSID` and `WIFI_PASS`
2. Upload the sketch
3. Open Serial Monitor at 115200 baud
4. Observe event sequence from STA start through GOT_IP

## Expected Output

```
Wait for WiFi...
[WiFi-event] event: ...
WiFi client started
[WiFi-event] event: ...
Connected to access point
[WiFi-event] event: ...
Obtained IP address: 192.168.1.100
WiFi connected (GOT_IP handler)
IP address: 192.168.1.100
```

(Event numeric IDs vary by platform.)

## Troubleshooting

**No GOT_IP event:**
- Verify SSID and password
- Wait longer; DHCP may take a few seconds after CONNECTED

**Duplicate GOT_IP messages:**
- Expected: both general and specific handlers fire for the same event

**DISCONNECTED after CONNECTED:**
- Check router settings, signal strength, or wrong password

## Notes

- IP is ready for TCP/UDP use after `ARDUINO_EVENT_WIFI_STA_GOT_IP`, equivalent to `WSS_GOT_IP`
- This example uses callbacks instead of polling `WiFi.status()`
- Event IDs are printed as integers for debugging

## Related Examples

- WiFiUDPClient - Uses `onEvent` to start UDP after GOT_IP
- WiFiMinimal - Polls `WSS_GOT_IP` instead of events
- WiFiScan - Network scan without connection
