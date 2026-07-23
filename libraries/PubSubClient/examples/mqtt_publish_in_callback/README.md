# MQTT Publish In Callback Example

## Overview

Shows how to republish a received MQTT payload from inside the callback. The payload must be copied first because the original buffer is reused while building the outbound PUBLISH packet.

## Features

- WiFi STA with `WSS_GOT_IP`
- Subscribe to `inTopic`
- Callback copies payload and publishes to `outTopic`

## Hardware Requirements

- Beken BK7239N board
- MQTT broker on port 1883

## Configuration

```cpp
WIFI_SSID / WIFI_PASS
const char* mqtt_server = "broker.mqtt-dashboard.com";
```

## How It Works

1. Declare callback before constructing `PubSubClient`
2. On message, `malloc` + `memcpy` payload
3. `client.publish("outTopic", p, length)` then `free`

## Usage

1. Edit WiFi and broker
2. Upload, then publish to `inTopic` and watch `outTopic`

## Expected Output

```
mqtt_publish_in_callback starting
WiFi connected
MQTT connected
Published announcement to outTopic
Subscribed to inTopic
Message arrived [inTopic] len=5 payload=hello
Republished payload to outTopic
```

Use another MQTT client: publish to `inTopic`, and optionally subscribe to `outTopic` to see the echoed payload.

## Troubleshooting

- No republish: ensure callback order/declaration and heap availability
- Disconnects: keep callback work short

## Related Examples

- mqtt_basic
- mqtt_large_message
