# WiFi UDP 客户端示例

## 概述

此示例连接 WiFi 网络后，每秒发送一条 UDP 数据包，内容为自启动以来的秒数。通过 `WiFi.onEvent()` 监听 `ARDUINO_EVENT_WIFI_STA_GOT_IP` 事件后再开始发送。可在 PC 上运行同目录下的 `udp_server.py` 接收并打印数据。

## 功能特性

- WiFi 站点模式，事件驱动连接管理
- `WiFi.onEvent()` 回调处理 GOT_IP 与断连
- 每 1 秒向 3333 端口发送 UDP 单播/广播
- 载荷格式：`Seconds since boot: <秒数>`

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- USB 数据线（烧录与串口监视）
- 同一局域网的 PC（用于运行 `udp_server.py`，推荐）

## 配置说明

更新 WiFi 配置段和 UDP 目标地址：

```cpp
// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
// ----------------------------------------------------------

const char *udpAddress = "192.168.1.255";  // 局域网广播或主机 IP
const int udpPort = 3333;
```

将 `udpAddress` 设为局域网广播地址（如 `192.168.1.255`）或运行接收程序的主机 IP。

## 工作原理

### 1. WiFi 连接
- 通过 `WiFi.onEvent()` 注册 `onWiFiEvent`
- 调用 `WiFi.begin(ssid, password)`
- 收到 `ARDUINO_EVENT_WIFI_STA_GOT_IP` 时调用 `udp.begin()` 并置 `connected = true`
- 断连事件时将 `connected` 置为 `false`

### 2. UDP 发送
- `loop()` 中每 1 秒发送一条 UDP 包
- 内容为 `Seconds since boot: ` 加 `millis() / 1000`
- 目标由 `udpAddress` 和 `udpPort` 定义

### 3. PC 端接收

在示例目录运行：

```powershell
python udp_server.py
```

脚本监听 UDP 3333 端口并打印收到的消息。

## 使用方法

1. 修改 `WIFI_SSID`、`WIFI_PASS` 和 `udpAddress`
2. 上传代码到开发板
3. 打开串口监视器（115200 波特率）
4. 在 PC 上运行 `python udp_server.py`
5. 确认每秒收到一条 UDP 消息

## 预期输出

**串口：**

```
Connecting to WiFi network: your-ssid
Waiting for WiFi connection...
WiFi connected! IP address: 192.168.1.100
```

**udp_server.py：**

```
Server listening
Seconds since boot: 5
Seconds since boot: 6
Seconds since boot: 7
```

## 故障排除

**收不到 UDP 包：**
- 确认 `udpAddress` 与局域网网段匹配
- 检查 PC 防火墙是否放行 UDP 3333
- 确认 PC 与开发板在同一网络

**WiFi 已连但无 UDP 输出：**
- 等待串口出现 `WiFi connected! IP address:`（GOT_IP 事件）
- 确认 `connected` 标志为 true 后才开始发送

**出现断连消息：**
- 路由器断开关联时属正常行为，需重新 GOT_IP 后才恢复发送

## 注意事项

- 连接就绪以 `ARDUINO_EVENT_WIFI_STA_GOT_IP` 为准，等价于 `WSS_GOT_IP`
- `udp.begin()` 在 GOT_IP 后以本地 IP 和端口初始化
- 默认目标为广播地址；改为单播 IP 可定向发送到指定主机

## 相关示例

- WiFiMinimal - 不含 UDP 的基础 STA 连接
- WiFiClientEvents - 更多 WiFi 事件回调示例
- WiFiClientBasic - WiFiMulti 连接后的 TCP 客户端
