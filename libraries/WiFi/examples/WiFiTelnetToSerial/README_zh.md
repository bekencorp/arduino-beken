# WiFi Telnet 转串口桥接示例

## 概述

此示例通过 `WiFiMulti` 连接 WiFi（支持两组 AP 故障切换），在 23 端口提供 Telnet 服务，将网络数据与 UART 双向桥接。默认 `UART_BRIDGE` 宏定义为 `Serial` 用于演示；若开发板有第二 UART，可改为 `Serial1`。

## 功能特性

- `WiFiMulti` 主/备 AP 故障切换
- 23 端口 Telnet 服务器（`WiFiServer`）
- 双向桥接：Telnet 客户端 ↔ UART（`UART_BRIDGE`）
- 单客户端限制（`MAX_SRV_CLIENTS = 1`）
- `loop()` 中通过 `WiFiMulti.run()` 维持连接

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- USB 数据线（烧录与串口监视）
- 同一局域网的 PC（带 Telnet 客户端）

## 配置说明

更新 WiFi 配置段：

```cpp
// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
#ifndef WIFI_SSID2
#define WIFI_SSID2 "your-backup-ssid"
#endif
#ifndef WIFI_PASS2
#define WIFI_PASS2 "your-backup-password"
#endif
// ----------------------------------------------------------
```

UART 桥接选择（源码中）：

```cpp
#define UART_BRIDGE Serial   // 演示用 USB 串口；可用时改为 Serial1
```

## 工作原理

### 1. WiFi 连接
- 注册两个 AP 凭据
- 最多重试 10 次 `wifiMulti.run()` 直到 `WL_CONNECTED`
- 初始化 `UART_BRIDGE`（115200 波特率）
- 启动 23 端口 Telnet 服务器

### 2. Telnet 桥接
- 同时接受一个 Telnet 客户端
- Telnet 收到的数据写入 `UART_BRIDGE`
- `UART_BRIDGE` 读到的数据发回 Telnet 客户端
- WiFi 断开时关闭 Telnet 客户端并尝试重连

## 使用方法

1. 修改 WiFi 凭据
2. 上传代码到开发板
3. 打开串口监视器（115200 波特率）
4. 记录打印的 IP 和 Telnet 命令
5. 在 PC 执行：`telnet <板子IP> 23`
6. 在 Telnet 会话中输入内容，数据经 `UART_BRIDGE` 输出（演示模式下为 Serial）

## 预期输出

```
Connecting
Connecting WiFi
WiFi connected IP address: 192.168.1.100
Ready! Use 'telnet 192.168.1.100 23' to connect
New client: 0 192.168.1.50
```

## 故障排除

**WiFi connect failed：**
- 确认至少一组 AP 凭据正确
- 查看串口重试倒计时

**Telnet 连接被拒绝：**
- 确认已 `WL_CONNECTED` 且 IP 有效
- 检查 PC 防火墙是否允许 Telnet（23 端口）

**Telnet 无数据 / 串口输入变成 `cmd NOT found`：**
- `Serial.begin()` 会接管打印口 RX（对齐旧 `arduino_idk`），CLI 不再占用该口输入
- 若仍异常，确认已重新编译烧录最新 `HardwareSerial`
- 真实独立 UART 桥接仍可改为 `#define UART_BRIDGE Serial1`

**loop 中 WiFi not connected：**
- 故障切换期间属正常；重连前 Telnet 客户端会被断开

## 注意事项

- 连接判断使用 `WiFiMulti.run()` 的 `WL_CONNECTED`
- 默认 `UART_BRIDGE` 为 `Serial`，不是 `Serial1`
- 同时仅支持一个 Telnet 客户端
- `server.setNoDelay(true)` 降低 Telnet 延迟

## 相关示例

- WiFiMulti - 无 Telnet 的多 AP 连接
- SimpleWiFiServer - 80 端口 HTTP 服务器
- WiFiClientBasic - 出站 TCP 客户端
