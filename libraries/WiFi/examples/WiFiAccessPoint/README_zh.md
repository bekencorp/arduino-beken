# WiFi 接入点（SoftAP）示例

## 概述

此示例创建 WiFi 软接入点（SoftAP），在 80 端口运行 HTTP 服务器，通过浏览器访问 `/H`（开灯）和 `/L`（关灯）控制板载 LED。客户端直接连接开发板，无需外部路由器。

## 功能特性

- 通过 `WiFi.softAP()` 创建 SoftAP
- 80 端口 HTTP 服务器，提供 LED 控制链接
- 串口打印 AP IP（`WiFi.softAPIP()`）
- 支持开放 AP 或 WPA2-PSK（密码为空或不少于 8 位）

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- USB 数据线（烧录与串口监视）
- 可连接 WiFi 的手机或 PC

## 配置说明

更新代码顶部的 AP 配置段：

```cpp
// -------- AP configuration (edit before upload) --------
#ifndef AP_SSID
#define AP_SSID "yourAP"
#endif
#ifndef AP_PASS
#define AP_PASS "yourPassword"
#endif
// ----------------------------------------------------------
```

也可在编译时通过 `-DAP_SSID=\"...\" -DAP_PASS=\"...\"` 传入。

AP 密码规则：
- 空字符串表示开放 AP
- 加密 AP 密码至少 8 个字符

## 工作原理

### 1. SoftAP 启动
- 配置 `LED_BUILTIN` 为输出
- 调用 `WiFi.softAP(AP_SSID, AP_PASS)`
- 打印 AP IP（由 `softAPConfig` 设为 `192.168.0.1`）
- 在 80 端口启动 HTTP 服务器

### 2. HTTP 服务器
- `loop()` 中接受客户端连接
- 解析 `GET /H` 或 `GET /L` 切换 LED
- 返回带控制链接的 HTML 页面
- 关闭客户端连接

## 使用方法

1. 修改 `AP_SSID` 和 `AP_PASS`
2. 上传代码到开发板
3. 打开串口监视器（115200 波特率）
4. 记录 AP IP 地址
5. 手机/PC 连接该 AP 的 SSID
6. 浏览器访问 `http://<ap-ip>/` 控制 LED

## 预期输出

```
Configuring access point...
AP IP address: 192.168.0.1
Server started
New Client.
GET / HTTP/1.1
Client Disconnected.
```

## 故障排除

**Soft AP creation failed：**
- 密码须为空或至少 8 位
- 信道冲突时可尝试更换 SSID

**手机/PC 无法连接 AP：**
- 确认 SSID 和密码与配置一致
- 靠近开发板；SoftAP 覆盖范围有限

**浏览器无法打开页面：**
- 使用串口打印的 IP，不要猜测地址
- 确认客户端已连接板子 AP，而非其他网络

**LED 无响应：**
- 确认 `LED_BUILTIN` 与开发板一致
- 查看串口是否收到 `GET /H` 或 `GET /L`

## 注意事项

- SoftAP 模式不使用 `WSS_GOT_IP` 或 `WL_CONNECTED`；关联为客户端连 AP
- 默认 AP 地址由 `softAPConfig` 设为 `192.168.0.1`（`192.168.0.0/24`）
- 无需外部路由器

## 相关示例

- SimpleWiFiServer - STA 模式下类似的 HTTP LED 控制
- WiFiMinimal - 连接外部路由器的站点模式
- WiFiScan - 扫描附近网络
