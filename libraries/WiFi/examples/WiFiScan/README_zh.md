# WiFi 扫描示例

## 概述

本示例演示如何扫描附近的可用 WiFi 网络。它执行定期扫描并显示有关每个检测到的网络的详细信息，包括 SSID、信号强度（RSSI）、信道和加密类型。这对于网络发现、站点调查和选择最佳连接网络非常有用。

## 功能特性

- **网络发现**：扫描范围内的所有可用 WiFi 网络
- **详细信息**：以固定列宽表格显示 SSID、RSSI、信道和加密类型
- **定期扫描**：定期连续扫描
- **加密检测**：识别安全协议（开放、WEP、WPA、WPA2、WPA3）
- **内存管理**：正确清理扫描结果以释放内存
- **隐藏 SSID**：空 SSID 显示为 `<hidden>`

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- 用于编程和串口监视的 USB 数据线

## 配置说明

本示例不需要 WiFi 凭据。程序在站点模式下运行但不连接到任何网络。

## 工作原理

### 扫描过程

1. **WiFi 初始化**：将 WiFi 设置为站点模式但不连接
2. **网络扫描**：调用 `WiFi.scanNetworks()` 扫描可用网络
3. **结果处理**：遍历扫描结果并显示信息
4. **内存清理**：删除扫描结果以释放内存
5. **重复**：等待 5 秒后再次扫描

### WiFi 模式

```cpp
WiFi.mode(WIFI_STA);    // 设置为站点模式
WiFi.disconnect();       // 确保未连接
```

### 扫描

```cpp
int n = WiFi.scanNetworks();
```

此函数：
- 阻塞直到扫描完成（通过 `EVENT_WIFI_SCAN_DONE`）
- 返回找到的网络数量
- 失败时返回 `WIFI_SCAN_FAILED` (-2)
- 扫描进行中返回 `WIFI_SCAN_RUNNING` (-1)
- 通过 `WiFi.SSID()`、`WiFi.RSSI()` 等访问结果

### 网络信息访问

对于找到的每个网络：

```cpp
WiFi.SSID(i)           // 网络名称
WiFi.RSSI(i)           // 信号强度（dBm）
WiFi.channel(i)        // WiFi 信道
WiFi.encryptionType(i) // 安全协议（WF_AP_AUTH_MODE_E）
```

### 内存管理

```cpp
WiFi.scanDelete();  // 释放扫描结果使用的内存
```

## 使用方法

1. **上传程序**：上传到开发板
2. **打开串口监视器**：设置波特率为 115200
3. **观察扫描**：每 5 秒查看发现的网络

### 预期输出

```
Setup done
Scan start
Scan done
5 networks found
Nr | SSID                             | RSSI | CH  | Encryption
--- + -------------------------------- + ---- + --- + ----------
  1 | HomeNetwork                      |  -45 |   6 | WPA2 PSK
  2 | OfficeWiFi                       |  -52 |  11 | WPA+WPA2
  3 | GuestNetwork                     |  -67 |   1 | WPA2 PSK
  4 | <hidden>                         |  -75 |   6 | open
  5 | Neighbor_2.4G                    |  -82 |   3 | WPA2 PSK

Scan start
...
```

## 理解输出

### RSSI（信号强度）

| RSSI 范围 | 质量 | 描述 |
|----------|-----|------|
| -30 到 -50 dBm | 优秀 | 非常强的信号 |
| -50 到 -60 dBm | 良好 | 强信号，可靠连接 |
| -60 到 -70 dBm | 一般 | 可接受，可能有问题 |
| -70 到 -80 dBm | 弱 | 连接质量差 |
| -80 到 -90 dBm | 非常弱 | 不稳定，频繁掉线 |
| 低于 -90 dBm | 不可用 | 太弱无法维持连接 |

### 加密类型

示例识别这些安全协议：

- **WAAM_OPEN**：无加密（公共网络）
- **WAAM_WEP**：WEP（过时，不安全）
- **WAAM_WPA_PSK**：WPA 预共享密钥
- **WAAM_WPA2_PSK**：WPA2（最常见）
- **WAAM_WPA_WPA2_PSK**：混合 WPA/WPA2
- **WAAM_WPA_WPA3_SAE**：WPA3（最新，最安全）
- **unknown**：未识别的加密

## 故障排除

### 未找到网络

- 验证 WiFi 天线是否正确连接
- 检查你所在区域是否启用 WiFi
- 确保开发板未在金属外壳中（屏蔽射频）
- 尝试移动到不同位置

### 扫描失败（`scan failed`）

- 几秒后重试；上电后首次扫描可能与 WiFi 初始化冲突
- 确保没有其他扫描正在进行

### 网络列表不完整

- 某些网络可能是隐藏的（不广播 SSID）
- 可能检测不到非常弱的信号

## 注意事项

- 扫描无需连接到任何网络即可工作
- 扫描需要站点模式
- 扫描后短暂延迟可避免 SDK 日志与表格输出交错
- 处理完结果后务必调用 `WiFi.scanDelete()`

## 相关示例

- WiFiMinimal - 最小 WiFi 连接与状态
- WiFiClient - TCP 客户端与 ThingSpeak HTTP
