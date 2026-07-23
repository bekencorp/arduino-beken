# WiFi 客户端 - 静态 IP 示例

## 概述

此示例演示如何在 WiFi 站点模式（STA）下使用静态 IP 地址连接路由器，并通过 `WiFiClient` 向 HTTP 服务器发起 GET 请求。适用于需要固定局域网 IP 的场景，例如端口转发或固定设备寻址。

## 功能特性

- 使用 `WiFi.config()` 配置静态 IP、网关、子网掩码和 DNS
- 连接 WiFi 并等待 `WSS_GOT_IP`
- 打印 IP、MAC、子网掩码、网关和 DNS 信息
- 周期性向 `example.com` 发起 HTTP GET 请求
- 连接超时保护

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- 用于编程和串口监视的 USB 数据线
- 2.4 GHz WiFi 路由器
- 可访问互联网的网络环境（用于访问 `example.com`）

## 配置说明

在程序开头修改通用 WiFi 配置段：

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

静态 IP 相关参数需与路由器局域网网段一致：

```cpp
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);     // optional
IPAddress secondaryDNS(8, 8, 4, 4);   // optional
```

HTTP 目标可按需修改：

```cpp
const char* host = "example.com";
const char* url  = "/";
```

编译时覆盖 WiFi 凭据：

```bash
-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"
```

## 工作原理

### 1. 静态 IP 配置
- 初始化串口（115200 波特率）
- 调用 `WiFi.config()` 设置静态 IP、网关、子网掩码和 DNS
- 配置失败时打印 `STA Failed to configure`

### 2. WiFi 连接
- 调用 `WiFi.begin()` 连接路由器
- 轮询 `WiFi.status()` 直到 `WSS_GOT_IP`
- 打印 IP、MAC、子网掩码、网关和 DNS

### 3. HTTP 请求
- 每 5 秒解析 `example.com`，打印解析出的服务器 IP
- 创建 `WiFiClient` 并连接服务器 IP 的 80 端口
- 发送 HTTP GET 请求
- 读取并打印服务器响应
- 5 秒无响应则超时断开

## 代码结构

```cpp
void setup() {
    // 配置静态 IP
    // 连接 WiFi
    // 打印网络信息
}

void loop() {
    // 连接 HTTP 服务器
    // 发送 GET 请求
    // 读取并打印响应
}
```

## 使用方法

1. 根据路由器网段修改静态 IP、网关和子网掩码
2. 更新 WiFi 凭据（`WIFI_SSID` 和 `WIFI_PASS`）
3. 确认静态 IP 未被其他设备占用
4. 上传代码到开发板
5. 打开串口监视器（115200 波特率）
6. 确认打印的 IP 与配置的静态 IP 一致
7. 观察 HTTP 请求响应

## 预期输出

```
Connecting to your-ssid
.....
WiFi connected
IP address: 192.168.1.100
MAC Address: AA:BB:CC:DD:EE:FF
Subnet Mask: 255.255.255.0
Gateway IP: 192.168.1.1
DNS: 8.8.8.8
connecting to example.com
DNS in use: 8.8.8.8
Resolving example.com... <服务器IP>
Connecting to <服务器IP>:80
Requesting URL: /
HTTP/1.1 200 OK
...
closing connection
```

## 故障排除

**`STA Failed to configure`：**
- 检查 IP、网关、子网掩码格式是否正确
- 确认静态 IP 与路由器网段匹配

**一直打印 `.` 后无法连接：**
- 验证 WiFi 凭据
- 确认路由器已开启 2.4 GHz 频段
- 使用 WiFiScan 确认网络可见

**IP 地址与配置不一致：**
- 确认 `local_IP` / `gateway` / `subnet` 与当前路由器网段一致（例如现场是 `10.37.45.x` 就不能再用 `192.168.1.x`）
- 确认该静态 IP 未被其它设备占用
- 连接成功后串口打印的 `IP address:` 应等于 `WiFi.config()` 中的 `local_IP`；若仍看到 DHCP 分配的其它地址，请确认已烧录包含静态 IP 修复的固件
- `secondaryDNS` 仅保留 API 兼容，底层只使用 `primaryDNS`

**`Resolving ... failed`（域名解析失败）：**
- 确认 `primaryDNS` 指向真正提供 DNS 服务的地址（推荐 8.8.8.8 或 114.114.114.114）
- 网关不一定提供 DNS 转发，填网关地址可能导致解析超时
- 观察 loop 中打印的 `DNS in use:`，确认实际生效的 DNS 服务器

**`connection failed` 或 HTTP 超时：**
- 确认开发板已获取 IP 且能访问互联网
- 尝试将 `host` 改为局域网内可访问的服务器

**setup 中打印 `DNS: 0.0.0.0`：**
- 属正常现象：静态配置由 SDK 在连接事件后才真正下发，例程打印时可能尚未生效
- 以 loop 中的 `DNS in use:` 输出为准

## 注意事项

- 静态 IP 必须在路由器局域网网段内且未被占用
- 本示例使用 Beken `WSS_*` 状态码，连接成功需等待 `WSS_GOT_IP`
- `WiFi.config()` 应在 `WiFi.begin()` 之前调用；库会在关联前调用 `bk_netif_static_ip()`，并在断开后重新置位，避免再次走 DHCP
- `secondaryDNS` 仅 API 兼容，底层只有一个 DNS 槽（使用 `primaryDNS`）
- 访问外网需正确配置网关和 DNS
- 默认每 5 秒发起一次 HTTP 请求

## 相关示例

- WiFiMinimal - 最小 WiFi 连接与状态
- WiFiClientConnect - STA 连接与断开
- WiFiClient - ThingSpeak HTTP 客户端
- WiFiScan - 扫描附近网络
