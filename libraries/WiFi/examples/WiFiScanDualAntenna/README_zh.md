# WiFi 双天线扫描示例

## 概述

此示例可选地通过 `WiFi.setDualAntennaConfig()` 配置双天线 GPIO，然后在站点模式下周期性扫描 WiFi 网络。无双天线硬件或 API 仅为 stub 时，配置可能失败，但扫描功能仍可正常使用。

## 功能特性

- 站点模式（`WIFI_STA`）
- 可选 `WiFi.setDualAntennaConfig(ANT1_GPIO, ANT2_GPIO, 0, 0)`
- 每 5 秒执行 `WiFi.scanNetworks()`
- 打印各网络的 SSID、RSSI 及加密标记

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- USB 数据线（烧录与串口监视）
- 双天线硬件（可选；完整天线切换测试才需要）

## 配置说明

若有双天线硬件，按开发板修改 GPIO：

```cpp
const uint8_t ANT1_GPIO = 0;
const uint8_t ANT2_GPIO = 1;
```

本例无需 WiFi 凭据，仅扫描不连接。

## 工作原理

### 1. 初始化
- 设置 `WiFi.mode(WIFI_STA)`
- 调用 `WiFi.setDualAntennaConfig()` 配置 ANT1/ANT2 GPIO
- 打印配置成功或失败（不支持硬件时 stub 可能返回失败）
- 扫描前调用 `WiFi.disconnect()`

### 2. 扫描循环
- 每 5 秒调用 `WiFi.scanNetworks()`
- 打印发现的网络数量
- 逐条输出：序号、SSID、RSSI、加密标记（加密网络带 `*`）

## 使用方法

1. 若有双天线硬件，修改 `ANT1_GPIO` 和 `ANT2_GPIO`
2. 上传代码到开发板
3. 打开串口监视器（115200 波特率）
4. 观察扫描结果；天线配置消息反映硬件/API 支持情况

## 预期输出

**无双天线支持（常见演示板）：**

```
Dual Antenna configuration failed (stub or unsupported hardware)!
Setup done
scan start
scan done
3 networks found
1: MyRouter (-45) *
2: GuestWiFi (-67) *
3: OpenNet (-72)
```

**有双天线支持：**

```
Dual Antenna configuration successfully done!
Setup done
scan start
...
```

## 故障排除

**Dual Antenna configuration failed：**
- 无双天线硬件或 stub 实现时属预期
- 扫描功能独立，可继续验证扫描输出

**no networks found：**
- 确认天线已连接
- 靠近 AP；确认附近有 2.4 GHz 网络
- 与 WiFiScan 例程结果对比

**扫描卡住或崩溃：**
- 降低扫描频率；检查平台扫描限制

## 注意事项

- 不使用 `WSS_GOT_IP` 或 `WL_CONNECTED`；不进行 WiFi 关联
- `setDualAntennaConfig` 可能为 stub；配置失败不代表扫描失败
- GPIO 0 和 1 为占位符，须按原理图修改
- 开放网络加密类型为 `WAAM_OPEN`（无 `*` 后缀）

## 相关示例

- WiFiScan - 无双天线配置的标准扫描
- WiFiMinimal - 扫描确认网络后连接
- WiFiClientEvents - 含 `ARDUINO_EVENT_WIFI_SCAN_DONE` 的事件 API
