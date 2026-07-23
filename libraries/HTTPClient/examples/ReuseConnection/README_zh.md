# ReuseConnection 示例

## 概述

这个示例演示如何通过 `HTTPClient::setReuse(true)` 在多次 HTTP 请求之间复用 TCP 连接（HTTP keep-alive）。它在 `loop()` 中每秒发起一次 GET 请求，当服务端支持持久连接时，可避免每次请求都重新建立 TCP 握手。

## 功能特性

- 使用 `WiFiMulti` 连接 WiFi
- 在 `setup()` 中调用 `http.setReuse(true)` 启用连接复用
- 使用全局 `HTTPClient` 与 `WiFiClient`，便于跨循环保持连接状态
- 在 `arduino-beken` 上等待 `WSS_GOT_IP` 后再发起 HTTP
- 通过 `writeToStream()` 将响应正文直接写入串口

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板
- 用于烧录和串口监视的 USB 线
- 可访问的 WiFi 网络
- 支持 HTTP keep-alive 的 HTTP 服务（建议局域网测试服务器）

## 配置说明

先修改示例顶部的通用 WiFi 配置段：

```cpp
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
```

按需修改目标 URL：

```cpp
const char *httpUrl = "http://example.com/";  // Change this to your HTTP server URL
```

验证 keep-alive 时建议改用局域网服务（见 `test_server/`）。也可以在编译时通过 `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"` 覆盖 WiFi 凭据。

## 工作原理

1. 串口初始化为 115200 波特率
2. 向 `WiFiMulti` 添加一个 AP 配置
3. 在 `setup()` 中调用 `http.setReuse(true)`
4. 在 `loop()` 中持续调用 `wifiMulti.run()`，直到返回 `WL_CONNECTED`
5. 等待 `WiFi.status() == WSS_GOT_IP`
6. 调用 `http.begin(client, httpUrl)` 并发送 `GET()`
7. 若返回 `HTTP_CODE_OK`，使用 `writeToStream()` 输出响应正文
8. 调用 `http.end()`；若服务端支持 keep-alive，底层 TCP 连接可能保持打开以供下次请求复用
9. 等待 1 秒后重复上述流程

## 使用方法

1. 修改 `WIFI_SSID` 与 `WIFI_PASS`
2. 将 `httpUrl` 改为可访问且支持 keep-alive 的 HTTP 地址
3. 上传示例到开发板
4. 打开 115200 波特率串口监视器
5. 观察每秒一次的 `[HTTP] GET... code:` 输出

## 预期输出

```text
[SETUP] WAIT 4...
[SETUP] WAIT 3...
[SETUP] WAIT 2...
[SETUP] WAIT 1...
[HTTP] GET... code: 200
<!DOCTYPE html>
...
[HTTP] GET... code: 200
...
```

## 故障排除

**反复打印 `[HTTP] WiFi not connected`**
- 检查 SSID 和密码是否正确
- 检查热点信号和网络是否可用

**`[HTTP] begin failed`**
- URL 必须以 `http://` 开头
- 当前示例未启用 HTTPS

**`[HTTP] GET... failed, error: ...`**
- 检查开发板是否能访问目标服务器
- 若公网访问不稳定，优先改用局域网 HTTP 服务做验证

**连接复用不明显**
- 部分服务端默认关闭 keep-alive 或每次响应后主动断开
- 可在服务端抓包或查看连接日志，确认是否复用同一 TCP 会话

## 注意事项

- 本示例保留原始例程每秒重试的行为，便于观察多次请求下的连接复用
- 当前平台使用 `http.begin(client, url)`，而不是旧式单参数 `http.begin(url)`
- 连接复用依赖服务端与网络栈双方支持；若服务端不支持 keep-alive，每次仍会新建 TCP 连接

## 相关示例

- `BasicHttpClient` - 基础 HTTP GET 示例
- `StreamHttpClient` - 通过底层流分块读取响应正文
- `HTTPClientMinimal` - 最小化的一次性 HTTP GET 示例
