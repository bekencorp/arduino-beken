# MQTT Large Message Example

## Overview

Shows how to publish arbitrarily large MQTT payloads with `beginPublish` / `print` / `endPublish`. After connect, the board self-publishes lyrics for `DEMO_BOTTLE_COUNT` bottles so you can test with only one development board. It also still responds if `greenBottles/<number>` is published externally.

## Features

- WiFi STA with `WSS_GOT_IP`
- Streaming publish API for long payloads
- Wildcard subscribe `greenBottles/#`
- Single-board self-test (no external MQTT client required)

## Hardware Requirements

- Beken BK7239N board
- MQTT broker reachable on port 1883

## Configuration

```cpp
WIFI_SSID / WIFI_PASS
const char* mqtt_server = "broker.mqtt-dashboard.com";
#define DEMO_BOTTLE_COUNT 3   // keep small for public brokers
```

## How It Works

1. Connect WiFi and MQTT
2. Subscribe to `greenBottles/#`
3. Once after connect, call `publishLyrics(DEMO_BOTTLE_COUNT)`
4. Optionally, if a `greenBottles/<number>` message arrives, publish lyrics again

## Usage

1. Update `WIFI_SSID` / `WIFI_PASS`
2. Upload the sketch
3. Open Serial Monitor at 115200 baud
4. Wait for self-test lines after `connected`

## Expected Output

```
WiFi connected
Attempting MQTT connection...connected
Subscribed to greenBottles/#
Self-test: publishing lyrics for 3 bottles
beginPublish greenBottles/lyrics, expectedLen=...
endPublish OK (large message sent)
```

## Troubleshooting

- Broker disconnect on large publish: lower `DEMO_BOTTLE_COUNT`
- `endPublish failed`: check broker connectivity / try again

## Related Examples

- mqtt_basic
- mqtt_publish_in_callback
