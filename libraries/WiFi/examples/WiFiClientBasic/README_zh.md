# WiFi 客户端基础示例

## 概述

此示例通过 `WiFiMulti` 连接单个 WiFi 接入点，然后在循环中反复建立 TCP 连接并向目标主机发送最小 HTTP GET 请求。适用于验证 WiFi 关联成功后的出站 TCP 客户端功能。

## 功能特性

- `WiFiMulti.addAP()` 注册一组凭据
- 通过 `WiFiMulti.run()` 等待 `WL_CONNECTED`
- 周期性 TCP 客户端循环，向可配置主机发送 HTTP GET
- 串口打印 HTTP 响应状态行

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- USB 数据线（烧录与串口监视）
- 可访问互联网的 WiFi 网络（默认目标为 `example.com`）

## 配置说明

更新代码顶部的 WiFi 配置段：

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

在 `loop()` 中按需修改 TCP 目标：

```cpp
const uint16_t port = 80;
const char *host = "example.com";
const char *url = "/";
```

## 工作原理

### 1. WiFi 连接
- 使用 `WiFiMulti.addAP(ssid, password)` 注册 AP
- 循环调用 `WiFiMulti.run()` 直到返回 `WL_CONNECTED`
- 再等待 `WiFi.status() == WSS_GOT_IP`
- 打印本地 IP 与网关

### 2. TCP 客户端循环
- 创建 `WiFiClient` 并连接 `host:port`
- 发送 `GET / HTTP/1.1` 请求
- 读取并打印完整 HTTP 响应
- 关闭连接，等待 5 秒后重试

## 使用方法

1. 修改 `WIFI_SSID` 和 `WIFI_PASS`
2. 上传代码到开发板
3. 打开串口监视器（115200 波特率）
4. 观察 DNS、TCP 连接与 HTTP 响应输出

## 预期输出

```
Waiting for WiFi... .....
Waiting for IP (WSS_GOT_IP)...
WiFi connected
IP address: 192.168.x.x
Gateway: 192.168.x.x
Connecting to example.com:80
Requesting URL: /
HTTP/1.1 200 OK
...
Closing connection.
Waiting 5 seconds before restarting...
```

## 故障排除

**WiFi 连接阶段一直打印 `.`：**
- 检查 SSID 和密码
- 使用 WiFiScan 确认网络可见

**Connection failed：**
- 确认当前网络允许访问互联网
- 确认 DNS 可解析 `example.com`
- 检查网络是否拦截 TCP 80 端口

**client.available() timed out：**
- 网络或目标网站可能未及时返回 HTTP 响应
- 更换网络后重试

## 注意事项

- `WiFiMulti.run()` 返回 `WL_CONNECTED` 表示已关联；IP 在 DHCP 完成后可用
- `WiFiClient` 不提供 TLS，不能直接访问仅支持 HTTPS 的网站
- 默认测试目标为 `http://example.com/`
- 每次循环间隔 5 秒重试 TCP 连接

## 相关示例

- WiFiMinimal - 使用 `WSS_GOT_IP` 轮询的基础 STA 连接
- WiFiMulti - 多 AP 故障切换
- WiFiClient - 完整 ThingSpeak HTTP 客户端
