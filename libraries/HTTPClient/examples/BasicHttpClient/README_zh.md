# BasicHttpClient 示例

## 概述

这个示例演示最基础的 HTTP GET 流程：先通过 `WiFiMulti` 连接 WiFi，再使用 `HTTPClient` 发起一次明文 HTTP 请求。它保留了“连接网络、发送 GET、打印状态码、在 `200 OK` 时输出响应正文”的教学目的，适合作为 HTTPClient 的基础入门示例。

## 功能特性

- 使用 `WiFiMulti` 连接 WiFi
- 通过 `HTTPClient` 发起明文 HTTP GET 请求
- 按当前 `arduino-beken` API 使用 `http.begin(WiFiClient&, url)`
- 通过串口输出成功与失败日志
- 失败时使用 `HTTPClient::errorToString()` 输出可读错误文本

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板
- 用于烧录和串口监视的 USB 线
- 可访问的 WiFi 网络
- 可访问的明文 HTTP 服务

## 配置说明

先修改示例顶部的通用 WiFi 配置段：

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

按需修改目标 URL：

```cpp
const char *httpUrl = "http://example.com/";
```

也可以在编译时通过 `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"` 覆盖 WiFi 凭据。

## 工作原理

1. 串口初始化为 115200 波特率
2. 向 `WiFiMulti` 添加一个 AP 配置
3. 在 `loop()` 中持续调用 `wifiMulti.run()`，直到返回 `WL_CONNECTED`
4. 等待 `WiFi.status() == WSS_GOT_IP`
5. 创建 `HTTPClient`，并调用 `http.begin(client, httpUrl)`
6. 使用 `http.GET()` 发送请求
7. 打印 HTTP 状态码
8. 若返回 `HTTP_CODE_OK`，调用 `getString()` 读取并打印响应正文
9. 调用 `http.end()` 释放本次请求状态，5 秒后再次尝试

## 使用方法

1. 修改 `WIFI_SSID` 与 `WIFI_PASS`
2. 将 `httpUrl` 改为可访问的 HTTP 地址
3. 上传示例到开发板
4. 打开 115200 波特率串口监视器
5. 等待看到 `[HTTP] GET... code:` 或可读错误信息

## 预期输出

```text
[SETUP] WAIT 4...
[SETUP] WAIT 3...
[SETUP] WAIT 2...
[SETUP] WAIT 1...
[HTTP] begin...
[HTTP] GET...
[HTTP] GET... code: 200
<!doctype html>
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
- 若使用域名，请确认当前网络的 DNS 可用

**返回 200 但没有打印正文**
- 目标服务可能返回空 body
- 可更换测试 URL，或改用本地 HTTP 测试服务

## 注意事项

- 这个示例保留了原始例程在 `loop()` 中周期性重试的行为
- HTTPS 代码仅保留为注释参考；TLS 请使用 `BasicHttpsClient`
- 当前 `arduino-beken` 平台使用 `http.begin(client, url)`，而不是旧式单参数 `http.begin(url)`

## 相关示例

- `HTTPClientMinimal` - 最小化的一次性 HTTP GET 示例
- `BasicHttpsClient` - 带根证书校验的 HTTPS GET
- `WiFiMulti` - 多 AP 连接示例
