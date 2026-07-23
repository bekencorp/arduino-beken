# HTTPClient 最小示例

## 概述

这是 `arduino-beken` 上最小的 HTTP 客户端示例：连接 WiFi 后使用 `HTTPClient` 发起一次 HTTP GET，并打印状态码与响应正文摘要。适合作为板端验证 HTTPClient 的第一步，再运行更复杂的示例。

## 功能特性

- 连接 WiFi，等待 `WSS_GOT_IP`
- 通过 `http.begin(WiFiClient&, url)` 发起明文 HTTP GET
- 打印 HTTP 状态码与响应正文预览
- 失败时打印负错误码及 `HTTPClient::errorToString()` 说明

## 硬件要求

- Beken BK7239N 开发板
- USB 线用于烧录与串口监视
- 可访问互联网或局域网 HTTP 服务的 WiFi 网络

## 配置说明

更新源码顶部的通用 WiFi 配置段：

```cpp
// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
// ----------------------------------------------------------
```

按需修改 HTTP 目标地址（在 `setup()` 内）：

```cpp
const char *url = "http://example.com/";
```

也可在编译时通过 `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"` 传入凭据。

## 工作原理

### 连接与请求流程

1. 初始化串口（115200 波特率）
2. 调用 `WiFi.begin()`，轮询直到 `WSS_GOT_IP`
3. 使用外部 `WiFiClient` 调用 `http.begin(client, url)`
4. 调用 `http.GET()`，用 `getString()` 读取响应正文
5. 调用 `http.end()` 释放本次请求状态

### API 说明

| 项目 | arduino-beken 行为 |
|------|--------------------|
| URL 协议 | 本示例仅使用 `http://` |
| `begin()` | 第一个参数必须为 `WiFiClient&` |
| 成功返回值 | 正数 HTTP 状态码（如 200） |
| 失败返回值 | 负数 `HTTPC_ERROR_*` 错误码 |

## 使用方法

1. 修改 `WIFI_SSID` 与 `WIFI_PASS`
2. 确认 `url` 指向可访问的 HTTP 服务
3. 上传例程
4. 打开串口监视器（115200）
5. 确认 WiFi 已连接且 HTTP 状态码为正数

## 预期输出

```text
Connecting to WiFi... .....
IP address: 192.168.x.x
HTTP GET http://example.com/
HTTP code=200
HTTP body length=...
HTTP body preview=<!doctype html>...
```

## 故障排除

**一直打印 `.`（WiFi 未连接）：**
- 检查 SSID 与密码
- 使用 WiFiScan 确认网络可见

**HTTP begin failed：**
- URL 必须以 `http://` 开头
- 确认 `HTTPClient` 与 `WiFiClient` 库可用

**GET 失败（负错误码）：**
- 确认已在 `WSS_GOT_IP` 后再发起 HTTP
- 检查 DNS、防火墙，或改用局域网 HTTP 服务

**HTTP code=200 但 body 为空：**
- 服务端可能仅返回头；可换 URL 或查看完整串口日志

## 注意事项

- 发起 HTTP 前须等待 `WSS_GOT_IP`
- 本示例在 `setup()` 中只请求一次，`loop()` 空闲
- HTTPS 请使用 `BasicHttpsClient`（`WiFiClientSecure`）

## 相关示例

- `BasicHttpClient` - 周期重试式 HTTP GET
- `BasicHttpsClient` - 带根证书校验的 HTTPS GET
- `WiFiMinimal` - 最小 WiFi 连接
- `WiFiClientBasic` - 使用 `WiFiClient` 的原始 TCP HTTP 请求
