# MQTT Non-blocking Reconnect Example

## Overview

Keeps an MQTT session alive without blocking the main loop. If disconnected, reconnect attempts happen at most once every 5 seconds.

## Features

- WiFi STA with `WSS_GOT_IP`
- Non-blocking reconnect timer
- Publish announcement and subscribe on success

## Hardware Requirements

- Beken BK7239N board
- MQTT broker on port 1883

## Configuration

```cpp
WIFI_SSID / WIFI_PASS
const char* mqtt_server = "broker.mqtt-dashboard.com";
```

## How It Works

1. Connect WiFi once in `setup()`
2. In `loop()`, if disconnected and 5 s elapsed, call `reconnect()`
3. When connected, call `client.loop()`

## Usage

1. Edit WiFi and broker
2. Upload and watch reconnect behavior after broker restarts

## Expected Output

```
mqtt_reconnect_nonblocking starting
Connecting to your-ssid
.....
WiFi connected
IP address: 192.168.x.x
Entering loop: non-blocking reconnect every 5s when disconnected
MQTT reconnect attempt... broker=broker.mqtt-dashboard.com:1883
MQTT connected
Published announcement to outTopic
Subscribed to inTopic
```

If the broker is unreachable you should see `MQTT connect failed, rc=...` then `Will retry MQTT reconnect in 5 seconds`. After a drop: `MQTT disconnected, rc=...` and another reconnect attempt.

## Troubleshooting

- Never reconnects: verify broker reachability and client ID conflicts
- Loop feels stuck: ensure you are not using the blocking reconnect example

## Related Examples

- mqtt_basic
- mqtt_bk7239
