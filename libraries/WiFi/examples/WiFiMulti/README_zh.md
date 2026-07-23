# WiFiMulti 多 AP 连接示例

## 概述

此示例演示使用 `WiFiMulti` 从多组 WiFi 凭据中自动连接当前可用的最佳接入点。注册两个 AP，连接成功后打印 SSID 与 IP，并在 `loop()` 中持续监控连接状态。

## 功能特性

- 通过 `WiFiMulti.addAP()` 注册多组凭据
- 自动选择可用网络
- `WiFiMulti.run()` 返回 `WL_CONNECTED` 表示已连接
- `loop()` 中周期性检查并重连
- 打印当前 SSID 和 IP 地址

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- USB 数据线（烧录与串口监视）
- 至少一个已配置且在范围内的 WiFi 网络（建议两个用于故障切换测试）

## 配置说明

更新代码顶部的主 AP 与备用 AP 凭据：

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

也可在编译时通过 `-DWIFI_SSID=...` 等参数覆盖。

## 工作原理

### 1. 初始连接
- 使用 `wifiMulti.addAP()` 注册两个 AP
- 在 `setup()` 中调用 `wifiMulti.run()` 直到返回 `WL_CONNECTED`
- 打印已连接 SSID 和本地 IP

### 2. 连接监控
- `loop()` 中周期性调用 `wifiMulti.run()`
- 若返回值不是 `WL_CONNECTED`，打印 `WiFi not connected!` 并等待 1 秒重试
- 主 AP 不可用时 `WiFiMulti` 自动尝试备用 AP

## 使用方法

1. 配置至少一个有效 AP；建议配置第二个用于故障切换测试
2. 上传代码到开发板
3. 打开串口监视器（115200 波特率）
4. 确认出现 `WiFi connected` 及 SSID、IP
5. 可选：关闭主 AP 验证是否切换到备用 AP

## 预期输出

```
Connecting WiFi...
WiFi connected
SSID: your-ssid
IP address: 192.168.1.100
```

连接丢失时：

```
WiFi not connected!
```

## 故障排除

**启动时无法连接：**
- 确认至少一组 SSID/密码正确且在范围内
- 使用 WiFiScan 确认网络可见

**故障切换较慢：**
- `WiFiMulti.run()` 按内部策略重试；AP 变更后需等待数秒
- 确认备用 AP 凭据正确

**连接到非预期 SSID：**
- `WiFiMulti` 按信号强度选择最佳 AP，属正常行为

## 注意事项

- 使用 `WiFiMulti.run()` 的 `WL_CONNECTED`，而非直接轮询 `WSS_GOT_IP`
- 可通过多次 `addAP()` 添加更多 AP
- 须在 `loop()` 中持续调用 `wifiMulti.run()` 以维持连接并支持故障切换

## 相关示例

- WiFiClientBasic - 单 AP `WiFiMulti` 加 TCP 客户端
- WiFiTelnetToSerial - `WiFiMulti` 加 Telnet 服务
- WiFiMinimal - 单 AP 使用 `WSS_GOT_IP` 轮询
