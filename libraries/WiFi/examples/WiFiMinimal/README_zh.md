# WiFi 最小示例

## 概述

这是最小的 WiFi 站点模式示例：连接网络、打印分配的 IP 地址，并在循环中定期报告连接状态。建议在运行 WiFiScan 或 WiFiClient 之前，先用此示例验证开发板 WiFi 是否正常。

## 功能特性

- WiFi 站点模式（`WIFI_STA`）
- 非阻塞 `WiFi.begin()`，等待 `WSS_GOT_IP`
- 通过 `WiFi.localIP()` 显示本地 IP
- 在 `loop()` 中定期输出状态与 IP

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- 用于编程和串口监视的 USB 数据线

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

或在编译时传入 `-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"`。

## 工作原理

### 连接流程

1. 以 115200 波特率初始化串口
2. 设置站点模式并调用 `WiFi.begin(ssid, password)`
3. 轮询 `WiFi.status()` 直到 `WSS_GOT_IP`（DHCP 完成且 IP 有效）
4. 连接成功后打印 IP 地址
5. 在 `loop()` 中每 5 秒打印状态码和 IP

### 状态码（`WF_STATION_STAT_E`）

| 值 | 常量 | 含义 |
|----|------|------|
| 0 | WSS_IDLE | 空闲 / 未连接 |
| 1 | WSS_CONNECTING | 连接中 |
| 6 | WSS_GOT_IP | 已连接并获取 IP |

## 使用方法

1. 修改 `WIFI_SSID` 和 `WIFI_PASS`
2. 上传程序
3. 打开串口监视器，波特率 115200
4. 确认出现 `WiFi connected` 且 IP 非零（不是 `0.0.0.0`）

## 预期输出

```
Connecting to your-ssid
.....
WiFi connected
IP address: 192.168.2.33
status=6 ip=192.168.2.33
status=6 ip=192.168.2.33
```

## 故障排除

**一直打印点号（无法连接）：**
- 检查 SSID 和密码
- 确认路由器为 2.4 GHz（如适用）
- 使用 WiFiScan 确认网络可见

**已连接但 IP 为 0.0.0.0：**
- DHCP 可能仍在进行；多等一会或检查路由器 DHCP 池

**WPA3 关联失败后再成功：**
- 在 WPA2/WPA3 混合 AP 上较常见；SDK 会降级重试 WPA2

## 注意事项

- 使用 TCP/HTTP 前应等待 `WSS_GOT_IP`，不能只看链路已关联
- 本示例不使用 `WiFiClient`；HTTP 请参考 WiFiClient

## 相关示例

- WiFiScan - 不连接，扫描附近网络
- WiFiClient - ThingSpeak HTTP 客户端
