# Simple WebSocket 示例（WSS）

## 概述

通过 TLS（`wss://`）连接安全 WebSocket 回显服务器，每 5 秒发送 `hello N`，并打印服务器返回的文本。

默认目标：`wss://echo.websocket.org/`（公开回显服务）。

## 功能特性

- WiFi STA，等待 `WSS_GOT_IP`
- 使用 `WiFiClientSecure` 作为 `WebSocketClient` 的 TLS 传输
- 周期发送文本帧，并用 `parseMessage` 读取回复

## 硬件要求

- Beken BK7239N 开发板（arduino-beken）
- 可访问公网（DNS + 出站 TCP 443）

## 配置说明

```cpp
WIFI_SSID / WIFI_PASS
WS_HOST   // 默认 echo.websocket.org
WS_PORT   // 默认 443
WS_PATH   // 默认 /
WS_INSECURE  // 1=跳过证书校验（调试）；0=需 setCACert()
```

## 工作原理

1. 连接 WiFi，等待 `WSS_GOT_IP`
2. 配置 TLS（`setInsecure` 或 `setCACert`）
3. `client.begin(WS_PATH)` 在 TLS 套接字上做 HTTP Upgrade
4. 连接保持期间发送文本帧，再 `parseMessage` / `readString`

## 使用方法

1. 修改 WiFi 凭据（`your-ssid` / `your-password`）
2. 上传，串口监视器波特率 115200

## 预期输出

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

## 故障排除

- `WebSocket begin failed`：检查 DNS、防火墙、TLS（`WS_INSECURE`/证书）与路径
- 立刻 `disconnected`：服务端关闭连接，稍后会重试
- 无接收输出：可换 `echo.websocket.org` 确认是否回显

## 注意事项

- 若要测明文 `ws://`，改回 `WiFiClient` 和非 TLS 端口即可
- 正式使用请设 `WS_INSECURE` 为 `0`，并用 `setCACert` 配置根证书

## 相关示例

- BasicHttpsClient（HTTPS + WiFiClientSecure）
- WiFiClient / WiFiMinimal
