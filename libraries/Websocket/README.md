# Websocket for arduino-beken

Arduino HTTP / WebSocket client based on ArduinoHttpClient, packaged for
Beken **arduino-beken** (`BK7239N`).

## Platform notes

- Pass a `WiFiClient` for plain `ws://`, or `WiFiClientSecure` for `wss://`.
- WiFi examples wait for `WSS_GOT_IP` (arduino-beken), not ESP `WL_CONNECTED`.
- Default SimpleWebSocket example uses public echo `wss://echo.websocket.org:443/`.

## Examples

| Example | Purpose |
|---------|---------|
| SimpleWebSocket | Secure WebSocket echo client (TLS via WiFiClientSecure) |

## License

Apache License 2.0 (upstream Arduino / MCQN HttpClient).
Port packaging for arduino-beken: 2026-07-20.
