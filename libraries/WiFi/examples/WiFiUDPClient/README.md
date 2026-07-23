# WiFi UDP Client Example

## Overview

This example connects to a WiFi network and sends a UDP packet every second containing the uptime in seconds. It uses `WiFi.onEvent()` to detect `ARDUINO_EVENT_WIFI_STA_GOT_IP` before starting UDP transmission. A companion `udp_server.py` script on your PC can receive and print the packets.

## Features

- WiFi station mode with event-driven connection handling
- `WiFi.onEvent()` callback for GOT_IP and disconnect
- Periodic UDP unicast/broadcast on port 3333
- Payload: `Seconds since boot: <n>`

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring
- PC on the same LAN to run `udp_server.py` (optional but recommended)

## Configuration

Update the WiFi configuration block and UDP target:

```cpp
// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
// ----------------------------------------------------------

const char *udpAddress = "192.168.1.255";  // LAN broadcast or host IP
const int udpPort = 3333;
```

Set `udpAddress` to your LAN broadcast address (e.g. `192.168.1.255`) or a specific host IP running the UDP receiver.

## How It Works

### Connection Process

1. Register `onWiFiEvent` via `WiFi.onEvent()`
2. Call `WiFi.begin(ssid, password)`
3. On `ARDUINO_EVENT_WIFI_STA_GOT_IP`, call `udp.begin()` and set `connected = true`
4. On disconnect event, set `connected = false`

### UDP Transmission

1. Every 1 second in `loop()`, if connected, send a UDP packet
2. Packet contains `Seconds since boot: ` followed by `millis() / 1000`
3. Target address and port are defined by `udpAddress` and `udpPort`

### UDP Receiver (PC)

Run the helper script from the example directory:

```powershell
python udp_server.py
```

It listens on UDP port 3333 and prints received messages.

## Usage

1. Update `WIFI_SSID`, `WIFI_PASS`, and `udpAddress`
2. Upload the sketch
3. Open Serial Monitor at 115200 baud
4. On your PC, run `python udp_server.py`
5. Confirm UDP packets appear every second

## Expected Output

**Serial:**

```
Connecting to WiFi network: your-ssid
Waiting for WiFi connection...
WiFi connected! IP address: 192.168.1.100
```

**udp_server.py:**

```
Server listening
Seconds since boot: 5
Seconds since boot: 6
Seconds since boot: 7
```

## Troubleshooting

**No UDP packets received:**
- Verify `udpAddress` matches your LAN subnet broadcast or receiver IP
- Confirm PC firewall allows UDP port 3333
- Ensure PC and board are on the same network

**WiFi connects but no UDP output:**
- Wait for `WiFi connected! IP address:` on Serial (GOT_IP event)
- Check that `connected` flag is set before packets are sent

**Disconnect messages:**
- Normal if router drops the association; the sketch stops sending until GOT_IP again

## Notes

- Connection readiness is signaled by `ARDUINO_EVENT_WIFI_STA_GOT_IP`, equivalent to `WSS_GOT_IP`
- `udp.begin()` is called with the local IP and port after GOT_IP
- Default target is broadcast; use a unicast IP to send to a specific host

## Related Examples

- WiFiMinimal - Basic STA connection without UDP
- WiFiClientEvents - More WiFi event callback examples
- WiFiClientBasic - TCP client after WiFiMulti connection
