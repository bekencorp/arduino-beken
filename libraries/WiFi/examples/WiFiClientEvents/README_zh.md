# WiFi 客户端事件回调示例

## 概述

此示例演示 arduino-beken 上简化的 `WiFi.onEvent()` API。注册通用与特定事件回调，连接 WiFi 并在串口打印关联与 IP 分配过程中的事件通知。

## 功能特性

- 通用处理函数 `WiFiEvent()` 接收所有 WiFi 事件
- 专用处理函数 `WiFiGotIP()` 仅注册 `ARDUINO_EVENT_WIFI_STA_GOT_IP`
- 简化事件类型：`arduino_event_id_t`
- 覆盖扫描、启停、连接、断连、GOT_IP 等事件

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- USB 数据线（烧录与串口监视）
- 2.4 GHz WiFi 路由器

## 配置说明

更新通用 WiFi 配置段：

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

## 工作原理

### 1. 事件注册
- 调用 `WiFi.disconnect(true)` 并重置状态
- `WiFi.onEvent(WiFiEvent)` 注册通用回调
- `WiFi.onEvent(WiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP)` 注册 IP 专用回调
- 调用 `WiFi.begin(ssid, password)`

### 2. 事件处理

| 事件 | 输出说明 |
|------|---------|
| `ARDUINO_EVENT_WIFI_STA_START` | WiFi 客户端已启动 |
| `ARDUINO_EVENT_WIFI_STA_CONNECTED` | 已连接接入点 |
| `ARDUINO_EVENT_WIFI_STA_GOT_IP` | 已获取 IP（含专用处理器） |
| `ARDUINO_EVENT_WIFI_STA_DISCONNECTED` | 已从 AP 断开 |
| `ARDUINO_EVENT_WIFI_SCAN_DONE` | 扫描完成（若触发） |

### 3. 主循环
- `loop()` 仅延时；连接进度全部通过回调上报

## 使用方法

1. 修改 `WIFI_SSID` 和 `WIFI_PASS`
2. 上传代码到开发板
3. 打开串口监视器（115200 波特率）
4. 观察从 STA 启动到 GOT_IP 的事件序列

## 预期输出

```
Wait for WiFi...
[WiFi-event] event: ...
WiFi client started
[WiFi-event] event: ...
Connected to access point
[WiFi-event] event: ...
Obtained IP address: 192.168.1.100
WiFi connected (GOT_IP handler)
IP address: 192.168.1.100
```

（事件数值 ID 因平台而异。）

## 故障排除

**未出现 GOT_IP 事件：**
- 检查 SSID 和密码
- CONNECTED 后 DHCP 可能需要数秒

**GOT_IP 消息重复：**
- 属预期行为：通用与专用处理器均会触发

**CONNECTED 后出现 DISCONNECTED：**
- 检查路由器设置、信号强度或密码错误

## 注意事项

- `ARDUINO_EVENT_WIFI_STA_GOT_IP` 之后可进行 TCP/UDP，等价于 `WSS_GOT_IP`
- 本例用回调代替轮询 `WiFi.status()`
- 事件 ID 以整数打印便于调试

## 相关示例

- WiFiUDPClient - GOT_IP 后启动 UDP
- WiFiMinimal - 轮询 `WSS_GOT_IP`
- WiFiScan - 仅扫描不连接
