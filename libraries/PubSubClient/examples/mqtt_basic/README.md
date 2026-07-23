# MQTT Basic Example

## Overview

Minimal MQTT client: connect to a broker, publish `hello world` to `outTopic`, subscribe to `inTopic`, print payloads, and reconnect with a blocking loop if needed.

## Features

- WiFi STA with `WSS_GOT_IP`
- Blocking reconnect
- Publish and subscribe with callback printing

## Hardware Requirements

- Beken BK7239N board
- Reachable MQTT broker on port 1883

## Configuration

```cpp
WIFI_SSID / WIFI_PASS
const char* mqtt_server = "broker.mqtt-dashboard.com";
```

## How It Works

1. Connect WiFi
2. Set server and callback
3. On disconnect, block in `reconnect()` until success
4. Call `client.loop()` continuously

## Usage

1. Edit WiFi and broker settings
2. Upload and monitor at 115200 baud

## Expected Output

```
WiFi connected
Attempting MQTT connection...connected
Message arrived [inTopic] ...
```

## Troubleshooting

- Stuck reconnecting: check broker host/DNS/firewall
- No callback: publish to `inTopic` from another client

## Related Examples

- mqtt_reconnect_nonblocking
- mqtt_auth
- mqtt_bk7239
