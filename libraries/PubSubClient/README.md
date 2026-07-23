# PubSubClient for arduino-beken (MQTT)

Arduino MQTT client based on Nick O'Leary's [PubSubClient](https://pubsubclient.knolleary.net),
ported for Beken **arduino-beken** (`BK7239N`).

## Platform notes

- Use `WiFiClient` (or another `Client`) with this library.
- WiFi examples wait for `WSS_GOT_IP` (bk_idk status codes), not ESP `WL_CONNECTED`.
- These examples use plain MQTT over TCP (ports 1883 / 1884). They do **not** use HTTPS/MQTTS or TLS certificates.
- Edit `WIFI_SSID` / `WIFI_PASS` (and broker settings) in each example before upload.
- FQBN: `beken:bk7239n:bk7239n:UploadSpeed=2000000`.

## Examples

Under **File → Examples → PubSubClient**:

| Example | Purpose |
|---------|---------|
| mqtt_bk7239 | BK7239N-oriented publish/subscribe demo |
| mqtt_basic | Basic publish + subscribe with blocking reconnect |
| mqtt_auth | Username/password broker auth |
| mqtt_reconnect_nonblocking | Non-blocking reconnect |
| mqtt_publish_in_callback | Publish from the message callback |
| mqtt_stream | Stream-backed receive path |
| mqtt_large_message | Large payload via beginPublish/endPublish |

Full upstream API documentation: https://pubsubclient.knolleary.net

## Limitations

 - It can only publish QoS 0 messages. It can subscribe at QoS 0 or QoS 1.
 - The maximum message size, including header, is **256 bytes** by default. This
   is configurable via `MQTT_MAX_PACKET_SIZE` in `PubSubClient.h` or can be changed
   by calling `PubSubClient::setBufferSize(size)`.
 - The keepalive interval is set to 15 seconds by default. This is configurable
   via `MQTT_KEEPALIVE` in `PubSubClient.h` or can be changed by calling
   `PubSubClient::setKeepAlive(keepAlive)`.
 - The client uses MQTT 3.1.1 by default. It can be changed to use MQTT 3.1 by
   changing value of `MQTT_VERSION` in `PubSubClient.h`.

## Compatible Hardware (this package)

 - Beken BK7239N via `beken:bk7239n:bk7239n`
 - Any board providing an Arduino `Client` API (same as upstream)

Upstream also documents Ethernet, ESP8266/ESP32, and other Client-compatible hardware;
those targets are outside this arduino-beken package.

## License

This code is released under the MIT License. See `LICENSE.txt`.
Upstream copyright: Nicholas O'Leary (2008–2020).
Port packaging for arduino-beken: 2026-07-20.
