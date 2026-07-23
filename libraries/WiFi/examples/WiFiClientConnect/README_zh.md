# WiFi 客户端连接 - STA 连接与断开示例

## 概述

此示例演示 WiFi 站点模式（STA）的连接与断开操作。上电后连接路由器并打印连接状态与 IP 地址；在 `loop()` 中按下 BOOT 键（GPIO0）可主动断开 WiFi。

建议在运行 WiFiClient 或其他 HTTP 例程之前，用本示例验证连接状态机与断开流程。

## 功能特性

- 以站点模式（`WIFI_STA`）连接 WiFi 网络
- 按 `WSS_*` 状态码打印连接过程日志
- 连接超时管理（20 次 × 500ms）
- GPIO0（BOOT 键）触发 `WiFi.disconnect()`
- 处理连接失败（密码错误、未找到 AP、DHCP 失败）

## 硬件要求

- Beken BK7239N 或 BK7236N 开发板（arduino-beken）
- 用于编程和串口监视的 USB 数据线
- 2.4 GHz WiFi 路由器

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

编译时覆盖：

```bash
-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"
```

## 工作原理

### 1. WiFi 连接
- 以 115200 波特率初始化串口
- 设置 GPIO0 为输入（BOOT 键）
- 调用 `WiFi.mode(WIFI_STA)` 后执行 `WiFi.begin()`
- 轮询 `WiFi.status()`，按 `WSS_*` 状态打印日志
- 等待 `WSS_GOT_IP`（成功）或 20 次重试后超时

### 2. 断开连接
- 在 `loop()` 中监测 BOOT 键（GPIO0 为 LOW）
- 调用 `WiFi.disconnect(true, false)` 关闭 WiFi 但保留已存储配置
- 显示断开状态

### 3. 状态码

| 值 | 常量 | 含义 |
|----|------|------|
| 0 | WSS_IDLE | 空闲 / 已断开 |
| 1 | WSS_CONNECTING | 连接中 |
| 2 | WSS_PASSWD_WRONG | 密码错误 |
| 3 | WSS_NO_AP_FOUND | 未找到 AP |
| 4 | WSS_CONN_FAIL | 连接失败 |
| 5 | WSS_CONN_SUCCESS | 链路已关联（DHCP 进行中） |
| 6 | WSS_GOT_IP | 已获取 IP（连接成功） |
| 7 | WSS_DHCP_FAIL | DHCP 失败 |

> 本例程使用 Beken `WSS_*` 状态，而非 ESP 风格的 `WL_*`。

## 代码结构

```cpp
void setup() {
    // 初始化串口通信
    // 配置 BOOT 键 GPIO
    // 连接 WiFi
    // 轮询状态直至连接成功或超时
}

void loop() {
    // 读取 BOOT 键状态
    // 按键按下时断开 WiFi
}
```

## 使用方法

1. 更新 WiFi 凭据（`WIFI_SSID` 和 `WIFI_PASS`）
2. 上传代码到开发板
3. 打开串口监视器（115200 波特率）
4. 确认出现 `[WiFi] WiFi is connected!` 及非零 IP
5. 按下 BOOT 键，确认出现 `[WiFi] Disconnected from WiFi!`

## 预期输出

```
[WiFi] Connecting to your-ssid
[WiFi] WiFi Status: 1
...
[WiFi] WiFi is connected!
[WiFi] IP address: 192.168.2.33
[WiFi] Disconnecting from WiFi!
[WiFi] Disconnected from WiFi!
```

## 故障排除

**一直打印状态 1（连接中）后超时：**
- 验证 WiFi 凭据
- 使用 WiFiScan 确认网络可见

**未找到 SSID：**
- 检查 SSID 拼写或确认路由器已开启 2.4 GHz 频段

**密码错误：**
- 验证 WPA 密码

**按 BOOT 键无反应：**
- 确认开发板 BOOT 键接在 GPIO0
- 部分板卡需 `INPUT_PULLUP`，可按硬件手册调整

## 注意事项

- 默认开启自动重连；可使用 `WiFi.setAutoReconnect(false)` 关闭
- `WiFi.disconnect(true, false)` 会关闭 WiFi 但保留 NVS 中的 WiFi 配置
- 连接超时为 20 次 × 500ms（约 10 秒）
- 本示例使用 Beken `WSS_*` 状态码，而非 ESP 风格的 `WL_*`

## 相关示例

- WiFiMinimal - 最小 WiFi 连接与状态
- WiFiScan - 扫描附近网络
- WiFiClient - ThingSpeak HTTP 客户端
