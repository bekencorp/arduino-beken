# MQTT Stream Example

## Overview

Demonstrates storing inbound MQTT payloads in a `Stream`. The original sketch used an external SRAM shield; this port uses an in-RAM `MemoryStream` suitable for BK7239N.

## Features

- WiFi STA with `WSS_GOT_IP`
- `PubSubClient::setStream()` payload capture
- Prints stored payload bytes from the stream in the callback

## Hardware Requirements

- Beken BK7239N board
- MQTT broker on port 1883

## Configuration

```cpp
WIFI_SSID / WIFI_PASS
const char* mqtt_server = "broker.mqtt-dashboard.com";
MemoryStream payloadStream(1024);  // change capacity if needed
```

## How It Works

1. Allocate a RAM stream and pass it to `setStream`
2. On subscribe traffic, PubSubClient writes payload bytes into the stream
3. Callback seeks to start and prints stream contents

## Usage

1. Edit WiFi and broker
2. Upload, publish to `inTopic`, observe Serial output

## Expected Output

```
WiFi connected
MQTT connected
Message arrived [inTopic] hello
```

## Troubleshooting

- Empty prints: ensure `setStream` is called before subscribe
- Truncated payloads: increase `MemoryStream` capacity

## Notes

- No external SRAM library is required on this platform

## Related Examples

- mqtt_basic
- mqtt_publish_in_callback
