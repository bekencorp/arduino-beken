# BasicHttpsClient 示例

## 概述

这个示例演示如何使用 `HTTPClient` 与 `WiFiClientSecure` 发起 HTTPS GET 请求。它通过 `WiFiMulti` 连接 WiFi，等待获取 IP，配置根证书（或可选的 insecure 调试模式），然后打印 HTTP 状态码和响应正文。

## 功能特性

- 使用 `WiFiMulti` 连接 WiFi
- 在 `arduino-beken` 上等待 `WSS_GOT_IP` 后再发起 HTTPS
- 使用 `WiFiClientSecure` 建立 TLS 连接
- 调用 `http.begin(WiFiClientSecure&, httpsUrl)`
- 支持 `setCACert()` 证书校验，或可选的 `setInsecure()` 调试模式
- 通过串口输出成功与失败日志

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板
- 用于烧录和串口监视的 USB 线
- 可访问的 WiFi 网络（默认目标需要外网，也可改用局域网 HTTPS 服务）

## 配置说明

先修改 WiFi 凭据：

```cpp
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
```

按需修改 HTTPS 目标：

```cpp
#define HTTPS_URL "https://valid-isrgrootx1.letsencrypt.org/"
```

示例内置 **ISRG Root X1**，与默认 Let's Encrypt 测试页匹配。若更换为其他 CA 签发的站点，请同步替换 `rootCACertificate`。

仅用于打通 TLS 链路时，可临时关闭证书校验：

```cpp
#define HTTPS_INSECURE 1
```

正式使用时不要保持 `HTTPS_INSECURE` 开启。

## 工作原理

1. 串口初始化为 115200 波特率
2. 向 `WiFiMulti` 添加一个 AP 配置
3. 持续调用 `wifiMulti.run()`，直到返回 `WL_CONNECTED`
4. 等待 `WiFi.status() == WSS_GOT_IP`
5. 创建 `WiFiClientSecure`，并调用 `setCACert()`（或 `setInsecure()`）
6. 调用 `https.begin(*client, HTTPS_URL)` 与 `https.GET()`
7. 若返回 `200` 或 `301`，打印响应正文
8. 先销毁 `HTTPClient`，再删除 secure client；10 秒后重试

## 使用方法

1. 修改 `WIFI_SSID` 与 `WIFI_PASS`
2. 确认 `HTTPS_URL` 与根证书匹配
3. 上传示例到开发板
4. 打开 115200 波特率串口监视器
5. 等待看到 `[HTTPS] GET... code:` 或可读错误信息

## 预期输出

```text
[SETUP] WAIT 4...
[SETUP] WAIT 3...
[SETUP] WAIT 2...
[SETUP] WAIT 1...
[HTTPS] begin...
[HTTPS] GET...
[HTTPS] GET... code: 200
...
Waiting 10s before the next round...
```

## 故障排除

**反复打印 `[HTTPS] WiFi not connected`**
- 检查 SSID 和密码是否正确
- 检查热点是否可用

**`[HTTPS] Unable to connect` 或 begin 失败**
- URL 必须以 `https://` 开头
- 确认平台已包含 `WiFiClientSecure`

**板子反复重启 / `Assert at: xQueueSemaphoreTake`**
- 明文 HTTP 例程正常、仅 HTTPS 崩溃时，多为 mbedTLS 线程锁路径问题（`MBEDTLS_THREADING_C`）
- 平台默认 `bk_threading_mutex_*` 使用 `xSemaphoreCreateMutex`，在本机 FreeRTOS 上可能触发 `xQueueSemaphoreTake:1543`
- `WiFiClientSecure` 已改为二进制信号量实现 mbedTLS 锁，并确保 `mbedtls_threading_set_alt()` 在首次 TLS 前执行
- 请重新编译烧录；若用 Arduino15 平台目录，需同步 `WiFiClientSecure` 库

**`[HTTPS] GET... failed, error: connection refused`**
- HTTPClient 把所有 `connect()` 失败都报成 `connection refused`，不一定是 TCP RST，也可能是 DNS/TLS 握手失败
- 先看新增日志：`[HTTPS] DNS host -> ip` 是否解析成功；失败则检查路由器 DNS
- 看 `[HTTPS] TLS detail (...)`：有内容则为 TLS/证书问题；无内容且 DNS 正常，多为 TCP 连不上（公司网封 443、需代理等）
- 临时设 `#define HTTPS_INSECURE 1` 区分“网络不通”与“证书校验失败”
- 若明文 `BasicHttpClient` 能通而 HTTPS 不通，基本可判定为出站 443 被拦或需改用内网 HTTPS 服务

**`TLS detail (-29184): SSL - An invalid SSL record was received`**
- 根因多为预编译 `libpsa_mbedtls` 的 `MBEDTLS_SSL_IN_CONTENT_LEN=4096` 过小，无法接收 Let's Encrypt 等站点的整段证书链 TLS 记录（常 >4KB）
- 串口若见 `bio_recv_to ret=4101` 且 `b0=11`，即为此类缓冲区溢出后的错位解析
- 需在 Linux/WSL 重编 vendor：`./dist/rebuild_vendor_mbedtls_16k.sh`，将新 `vendor-bk7239n-0.0.1.zip` 安装到 Arduino15 后重编示例
- 验证埋点应打印 `ssl_in_len val=16384`，且出现 `after_ssl_handshake`

**内存不足或握手失败**
- HTTPS 比明文 HTTP 更占 RAM
- 首次验证建议使用短响应页面

## 注意事项

- 本示例保留原始例程的周期性重试行为
- 默认启用证书校验
- `HTTPS_INSECURE` 仅用于调试

## 相关示例

- `BasicHttpClient` - 明文 HTTP GET 示例
- `Authorization` - HTTP Basic Auth 示例
- `ReuseConnection` - HTTP 连接复用示例
