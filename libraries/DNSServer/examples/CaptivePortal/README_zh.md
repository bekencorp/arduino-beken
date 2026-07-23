# Captive Portal（强制门户）示例

## 概述

此示例使用 DNSServer 库创建 WiFi 强制门户（Captive Portal）。当客户端连接到 SoftAP 后，所有 DNS 查询都会被重定向到开发板的 IP 地址（192.168.4.1），导致任何网页请求都会显示强制门户页面。这演示了"DNS 劫持"的核心概念，常用于公共 WiFi 的认证场景。

## 功能特性

- 创建 WiFi SoftAP（接入点）
- DNS 服务器运行在 53 端口：将所有域名查询重定向到 AP IP
- HTTP 服务器运行在 80 端口：提供强制门户 HTML 页面
- 串口打印 AP IP 和客户端连接信息
- 可配置的 AP SSID 和密码

## 硬件要求

- Beken BK7239N 开发板（arduino-beken）
- USB 数据线（用于烧录和串口监视）
- 支持 WiFi 的设备（手机、PC 或平板）

## 配置说明

更新代码顶部的 AP 配置段：

```cpp
// -------- AP configuration (edit before upload) --------
#ifndef AP_SSID
#define AP_SSID "DNSServer"
#endif
#ifndef AP_PASS
#define AP_PASS ""
#endif
// ----------------------------------------------------------
```

也可在编译时传入：`-DAP_SSID="MyPortal" -DAP_PASS="password"`

AP 密码规则：
- 空字符串表示开放 AP（无密码）
- 加密 AP 密码至少 8 个字符

## 工作原理

### 1. SoftAP 启动
- 配置 WiFi 为 AP 模式，使用固定 IP（192.168.4.1）
- 创建 SoftAP，使用指定的 SSID 和密码
- 打印 AP IP 到串口监视器

### 2. DNS 服务器
- 在 53 端口（标准 DNS 端口）启动 DNS 服务器
- `dnsServer.start(DNS_PORT, "*", apIP)` 将所有域名查询重定向到 apIP
- 当客户端解析任何域名时，返回开发板的 IP

### 3. HTTP 服务器
- 在 80 端口监听 HTTP 连接
- 当客户端连接时，读取 HTTP 请求
- 返回简单的 HTML 强制门户页面

## 使用方法

1. 修改 `AP_SSID` 和 `AP_PASS`（如需）
2. 上传代码到开发板
3. 打开串口监视器（115200 波特率）
4. 使用设备连接到 AP 的 SSID
5. 在浏览器中打开任意网站，将看到强制门户页面

## 预期输出

```
Starting Captive Portal...
AP IP address: 192.168.4.1
DNS server started on port 53
HTTP server started
New HTTP client connected
HTTP client disconnected
```

## 故障排除

**Soft AP creation failed：**
- 密码须为空或至少 8 位
- 信道冲突时可尝试更换 SSID

**无法连接 AP：**
- 确认 SSID 和密码与配置一致
- 靠近开发板；SoftAP 覆盖范围有限

**DNS 重定向不生效 / 打开任意网址没有门户页：**
- SoftAP DHCP 原先会占用 53 端口并返回 REFUSED；当前固件已禁用该行为，改由 DNSServer 劫持。
- 手机请“忘记网络”后重新连接（以拿到 DNS=AP IP）。
- 串口确认有 `DNS server started on port 53`（不是 start failed）。
- 先访问 `http://192.168.4.1`：能开门户而其它网址不行，说明客户端 DNS 仍不对。
- 可用：`nslookup google.com 192.168.4.1`

**浏览器显示连接被拒绝：**
- 检查串口输出确认服务器已启动
- 确认 HTTP 服务器在 80 端口运行

## 注意事项

- 强制门户依赖客户端使用 AP 的 DNS 服务器
- 部分移动设备可能显示内置的强制门户检测页面
- `dnsServer.start()` 中的通配符 `"*"` 匹配所有域名
- 无需外部路由器，客户端直接连接到开发板

## 相关示例

- WiFiAccessPoint - 基础 SoftAP + HTTP 服务器
- WiFiClient - 连接外部 WiFi 网络
- WiFiServer - STA 模式下的 TCP 服务器