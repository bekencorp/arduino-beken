# MQTT Auth Example

## Overview

Connects to WiFi and an MQTT broker using username/password authentication, then publishes to `outTopic` and subscribes to `inTopic`.

Default test target is the public authenticated listener on [test.mosquitto.org](https://test.mosquitto.org/) (port 1884). No signup is required.

## Features

- WiFi STA with `WSS_GOT_IP`
- MQTT CONNECT with username and password
- One-shot publish and subscribe after connect

## Hardware Requirements

- Beken BK7239N board
- Internet access to `test.mosquitto.org:1884` (or your own auth broker)

## Configuration

```cpp
WIFI_SSID / WIFI_PASS

const char* mqtt_server = "test.mosquitto.org";
const uint16_t mqtt_port = 1884;
const char* mqtt_user = "rw";
const char* mqtt_pass = "readwrite";
```

Other public Mosquitto accounts: `ro`/`readonly`, `wo`/`writeonly`.

## How It Works

1. Connect WiFi and wait for IP
2. `client.connect(clientId, user, pass)`
3. Publish announcement and subscribe
4. `client.loop()` keeps the session alive

## Usage

1. Edit WiFi credentials
2. Upload and open Serial Monitor at 115200
3. Confirm `MQTT connected (auth OK)`

## Expected Output

```
mqtt_auth starting
Broker test.mosquitto.org:1884 user=rw
WiFi connected
IP address: 192.168.x.x
MQTT connected (auth OK)
Published announcement to outTopic
Subscribed to inTopic
```

## Troubleshooting

- Auth failures: confirm port **1884** (not 1883) and `rw`/`readwrite`
- Buffer overflow on long credentials: call `client.setBufferSize(255)`
- Public broker may disconnect later (`rc=-4`); reconnect is not part of this example

## Related Examples

- mqtt_basic
- mqtt_bk7239
