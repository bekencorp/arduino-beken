# Simple WebSocket Example (WSS)

## Overview

Connects to a secure WebSocket echo server over TLS (`wss://`), sends a text `hello N` message every 5 seconds, and prints any text reply.

Default target: `wss://echo.websocket.org/` (public echo, Ably-sponsored).

## Features

- WiFi STA with `WSS_GOT_IP`
- `WiFiClientSecure` TLS transport under `WebSocketClient`
- Periodic text send and receive via `parseMessage`

## Hardware Requirements

- Beken BK7239N board (arduino-beken)
- Internet access (DNS + outbound TCP 443)

## Configuration

```cpp
WIFI_SSID / WIFI_PASS
WS_HOST   // default: echo.websocket.org
WS_PORT   // default: 443
WS_PATH   // default: /
WS_INSECURE  // 1 = skip cert verify (debug); 0 = call setCACert()
```

## How It Works

1. Connect WiFi and wait for `WSS_GOT_IP`
2. Configure TLS (`setInsecure` or `setCACert`)
3. `client.begin(WS_PATH)` performs HTTP Upgrade over the TLS socket
4. While connected: send text frames, then `parseMessage` / `readString`

## Usage

1. Edit WiFi credentials (`your-ssid` / `your-password`)
2. Upload and open Serial Monitor at 115200 baud

## Expected Output

```
Connecting to ...
SSID: ...
IP Address: ...
WARNING: TLS certificate verification disabled (WS_INSECURE=1)
starting WebSocket client wss://echo.websocket.org:443/
WebSocket connected
Sending hello 0
Received a message:
hello 0
```

## Troubleshooting

- `WebSocket begin failed`: check DNS, firewall, TLS (`WS_INSECURE` / CA), and path
- Immediate `disconnected`: server closed the session; retry after delay
- No receive lines: some servers may not echo; try `echo.websocket.org`

## Notes

- Plain `ws://` is still possible by switching back to `WiFiClient` and a non-TLS port
- For production, set `WS_INSECURE` to `0` and provide a root CA PEM via `setCACert`

## Related Examples

- BasicHttpsClient (HTTPS + WiFiClientSecure)
- WiFiClient / WiFiMinimal
