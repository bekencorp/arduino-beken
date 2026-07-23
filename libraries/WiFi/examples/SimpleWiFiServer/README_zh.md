# Simple WiFi Server - HTTP LED 控制示例

## 概述

此示例以 WiFi 站点模式（STA）连接路由器，在 80 端口启动 HTTP 服务器，并通过浏览器访问 `/H`（开灯）和 `/L`（关灯）控制板载 LED。适用于在基础 WiFi 连接验证通过后，测试 TCP 服务端功能。

## 功能特性

- WiFi 站点模式（`WIFI_STA`）
- 等待 `WSS_GOT_IP` 后再启动服务器
- 80 端口 HTTP 服务器，提供简单 HTML 控制页
- 通过 `GET /H` 和 `GET /L` 控制 LED
- 串口打印 HTTP 请求内容

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- USB 数据线（烧录与串口监视）
- 板载 LED 连接到 `LED_BUILTIN`（或按开发板修改引脚）

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

也可在编译时通过 `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"` 传入。

## 工作原理

### 1. WiFi 连接
- 调用 `WiFi.begin(ssid, password)`
- 轮询 `WiFi.status()` 直到 `WSS_GOT_IP`
- 打印本地 IP 地址

### 2. HTTP 服务器
- 在 `loop()` 中接受客户端连接
- 解析请求行中的 `GET /H` 或 `GET /L`
- 切换 LED 状态并返回带控制链接的 HTML 页面
- 关闭客户端连接

## 使用方法

1. 修改 `WIFI_SSID` 和 `WIFI_PASS`
2. 上传代码到开发板
3. 打开串口监视器（115200 波特率）
4. 记录打印的 IP 地址
5. 在同一局域网的设备浏览器中访问 `http://<板子IP>/`
6. 点击页面链接控制 LED 开关

## 预期输出

```
Connecting to your-ssid
.....
WiFi connected.
IP address: 192.168.1.100
New Client.
GET / HTTP/1.1
Client Disconnected.
```

## 故障排除

**一直打印 `.` 无法连接：**
- 检查 SSID 和密码
- 确认路由器 2.4 GHz 已开启
- 使用 WiFiScan 确认网络可见

**浏览器无法访问服务器：**
- 确认客户端与开发板在同一 WiFi 网络
- 检查客户端防火墙设置
- 核对串口输出的 IP 地址

**LED 无响应：**
- 确认 `LED_BUILTIN` 与开发板原理图一致
- 查看串口是否收到 `GET /H` 或 `GET /L` 请求

## 注意事项

- 启动 HTTP 服务器前须等待 `WSS_GOT_IP`，不能仅依赖链路层连接
- 本示例每次处理一个客户端连接
- HTTP 响应为最小实现，不含样式或脚本

## 相关示例

- WiFiMinimal - 最小 WiFi 连接与状态
- WiFiAccessPoint - SoftAP 模式下的类似 HTTP LED 控制
- WiFiClient - 出站 HTTP 客户端示例
