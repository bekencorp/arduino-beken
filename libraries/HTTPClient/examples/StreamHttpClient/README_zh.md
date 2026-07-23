# StreamHttpClient 示例

## 概述

这个示例演示如何使用 `HTTPClient` 进行响应流式读取。它通过 `WiFiMulti` 连接 WiFi，发起一次 HTTP GET 请求，然后从 `getStreamPtr()` 返回的底层流中按小块读取响应正文，并打印到串口。

## 功能特性

- 使用 `WiFiMulti` 连接 WiFi
- 在 `arduino-beken` 上等待 `WSS_GOT_IP` 后再发起 HTTP
- 执行一次明文 HTTP GET 请求
- 打印服务端返回的 `Content-Length`
- 通过原始 TCP 流以 128 字节分块读取正文
- 打印累计读取的总字节数

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板
- 用于烧录和串口监视的 USB 线
- 可访问的 WiFi 网络
- 可访问且响应稳定的 HTTP 服务

## 配置说明

先修改通用 WiFi 配置段：

```cpp
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
```

按需修改测试 URL：

```cpp
const char *httpUrl = "http://example.com/";
```

如果条件允许，优先使用局域网固定长度响应页面，便于核对字节数。

## 工作原理

1. 串口初始化为 115200 波特率
2. 向 `WiFiMulti` 添加一个 AP 配置
3. 持续调用 `wifiMulti.run()`，直到返回 `WL_CONNECTED`
4. 等待 `WiFi.status() == WSS_GOT_IP`
5. 调用 `http.begin(client, httpUrl)` 并发送 `GET()`
6. 用 `http.getSize()` 读取预期响应长度
7. 用 `http.getStreamPtr()` 从底层流中按 128 字节分块读取正文
8. 打印读取内容和最终总字节数

## 使用方法

1. 设置 `WIFI_SSID` 和 `WIFI_PASS`
2. 将 `httpUrl` 改为可访问的 HTTP 地址
3. 上传示例
4. 打开 115200 波特率串口监视器
5. 确认 HTTP 状态码、流式输出内容和总字节数

## 预期输出

```text
[SETUP] WAIT 4...
[SETUP] WAIT 3...
[SETUP] WAIT 2...
[SETUP] WAIT 1...
[HTTP] WiFi connected, waiting for IP...
[HTTP] begin...
[HTTP] GET...
[HTTP] GET... code: 200
[HTTP] Content-Length: -1
<!doctype html>...
[HTTP] stream bytes read: ...
[HTTP] connection closed or file end.
```

说明：`example.com` 常不带 `Content-Length`（显示 `-1`，chunked）。若要用固定长度核对字节数，建议改用局域网测试页。


## 故障排除

**`[HTTP] begin failed`**
- URL 必须以 `http://` 开头
- 当前示例未启用 HTTPS

**`GET... failed` 且返回负错误码**
- 检查 WiFi、DNS、防火墙和 HTTP 服务可达性
- 优先使用局域网简单 HTTP 服务做验证

**`stream pointer is null`**
- 说明响应头处理完成后没有可继续读取的 TCP 流
- 先检查前面是否已经出现请求失败

**总字节数与 `Content-Length` 不一致**
- 服务端可能提前关闭连接
- 可改用体积更小、长度固定的本地页面重新验证

## 注意事项

- 这个示例用于演示流式读取，而不是把整个正文读入 `String`
- 如果服务端不返回 `Content-Length`，`getSize()` 可能为 `-1`，示例会继续读到连接关闭
- 与 `getString()` 相比，流式读取可减少临时内存占用

## 相关示例

- `HTTPClientMinimal` - 最小化的一次性 GET 验证示例
- `BasicHttpClient` - 经典重试式 GET 示例
