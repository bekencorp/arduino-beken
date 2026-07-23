# MQTT BK7239 Example

## Overview

Connects to WiFi, then to an MQTT broker. Publishes a counter message to `outTopic` every two seconds, subscribes to `inTopic`, prints payloads, and toggles an LED when the first payload character is `1`.

## Features

- WiFi station mode with wait for `WSS_GOT_IP`
- Blocking MQTT reconnect
- Periodic publish and subscribe callback
- Optional GPIO LED control from payload

## Hardware Requirements

- Beken BK7239N board (arduino-beken)
- USB cable for programming and serial monitoring
- Network access to an MQTT broker (default: `broker.mqtt-dashboard.com:1883`)

## Configuration

```cpp
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif

const char* mqtt_server = "broker.mqtt-dashboard.com";
// LED uses LED_BUILTIN (GPIO 24 on BK7239N)
```

## How It Works

1. Connect WiFi and wait for `WSS_GOT_IP`
2. Set MQTT server and callback
3. On connect: publish announcement and subscribe to `inTopic`
4. Every 2 seconds publish `hello world #N`
5. Callback prints messages and drives the LED

## Usage

1. Edit WiFi credentials and broker host
2. Upload the sketch
3. Open Serial Monitor at 115200 baud
4. Publish to `inTopic` from another MQTT client to test the callback (`1` / other for LED)

## Expected Output

```
Connecting to your-ssid
.....
WiFi connected
IP address: 192.168.x.x
Attempting MQTT connection...connected
Publish message: hello world #1
```

## Troubleshooting

- Never leaves dots: check SSID/password and 2.4 GHz WiFi
- MQTT `rc` failures: check broker host, port 1883, and DNS
- LED does nothing: confirm board LED is on `LED_BUILTIN` (GPIO 24) and active-low wiring

## Notes

- Wait for `WSS_GOT_IP` before opening MQTT TCP
- For non-blocking reconnect, see `mqtt_reconnect_nonblocking`

## Related Examples

- mqtt_basic
- mqtt_auth
- mqtt_reconnect_nonblocking
