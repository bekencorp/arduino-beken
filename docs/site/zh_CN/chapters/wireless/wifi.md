# Wi-Fi

BK7239N Wi-Fi 模块旨在解决高密度 IoT 场景下的拥堵问题。它支持 802.11ax (Wi-Fi 6) 标准，并覆盖 2.4GHz 和 5GHz 双频段。

**Wi-Fi 功能概述**

| 特性 | 描述 |
|---------|-------------|
| **工作模式** | Station (STA) - AP模式正在开发中，将在后续版本中支持 |
| **无线标准** | IEEE 802.11 b/g/n/ax (Wi-Fi 6) |
| **频段** | 2.4 GHz + 5 GHz (双频) |
| **安全** | WEP, WPA/WPA2 Personal, WPA3 |

*注：AP模式正在开发中，将在后续版本中支持。*

## Wi-Fi API 参考

### begin()

连接到 Wi-Fi 网络。

```cpp
bool WiFi.begin(const char* ssid);
bool WiFi.begin(const char* ssid, const char* passphrase);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `ssid` | `const char*` | 网络 SSID |
| `passphrase` | `const char*` | 网络密码（用于安全网络） |

**返回：** 如果连接成功启动则返回 `true`，否则返回 `false`。

### disconnect()

断开当前 Wi-Fi 网络连接。

```cpp
bool WiFi.disconnect(bool wifioff = false);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `wifioff` | `bool` | 如果为 `true` 则关闭 Wi-Fi 无线电 |

**返回：** 如果成功断开连接则返回 `true`，否则返回 `false`。

### status()

获取当前连接状态。

```cpp
wl_status_t WiFi.status();
```

**返回：** 连接状态（参见状态常量）。

### SSID()

获取已连接网络的 SSID 或按索引获取扫描到的网络的 SSID。

```cpp
String WiFi.SSID(uint8_t index = WIFI_SCAN_RUNNING);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `index` | `uint8_t` | 扫描到的网络的索引（对于已连接的网络可以省略） |

**返回：** SSID 字符串。

### RSSI()

获取已连接网络的信号强度（RSSI）或按索引获取扫描到的网络的 RSSI。

```cpp
int32_t WiFi.RSSI(uint8_t index = WIFI_SCAN_RUNNING);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `index` | `uint8_t` | 扫描到的网络的索引（对于已连接的网络可以省略） |

**返回：** RSSI 值（单位：dBm）。

### BSSID()

获取已连接网络的 BSSID（MAC 地址）。

```cpp
uint8_t* WiFi.BSSID();
```

**返回：** 指向 BSSID 数组的指针。

### encryptionType()

获取已连接网络的加密类型或按索引获取扫描到的网络的加密类型。

```cpp
uint8_t WiFi.encryptionType(uint8_t index = WIFI_SCAN_RUNNING);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `index` | `uint8_t` | 扫描到的网络的索引（对于已连接的网络可以省略） |

**返回：** 加密类型。

### scanNetworks()

开始扫描可用的 Wi-Fi 网络。

```cpp
int8_t WiFi.scanNetworks(bool async = true, bool show_hidden = false, uint8_t channel = 0, const char* ssid = NULL);
```

| 参数 | 类型 | 描述 |
|-----------|------|-------------|
| `async` | `bool` | 如果为 `true` 则异步执行扫描 |
| `show_hidden` | `bool` | 如果为 `true` 则显示隐藏网络 |
| `channel` | `uint8_t` | 要扫描的特定信道（0 = 所有信道） |
| `ssid` | `const char*` | 要扫描的特定 SSID |

**返回：** 找到的网络数量（同步时），或扫描 ID（异步时）。

### localIP()

获取本地 IP 地址。

```cpp
IPAddress WiFi.localIP();
```

**返回：** 本地 IP 地址。

### 状态常量

| 常量 | 描述 |
|----------|-------------|
| `WL_IDLE_STATUS` | 空闲状态 |
| `WL_NO_SSID_AVAIL` | SSID 不可用 |
| `WL_SCAN_COMPLETED` | 扫描完成 |
| `WL_CONNECTED` | 已连接到 Wi-Fi |
| `WL_CONNECT_FAILED` | 连接失败 |
| `WL_CONNECTION_LOST` | 连接丢失 |
| `WL_DISCONNECTED` | 从 Wi-Fi 断开连接 |

## Wi-Fi 示例

连接到现有 Wi-Fi 网络。

### 示例：连接 Wi-Fi

```cpp
#include <WiFi.h>

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // 检查连接状态
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  }
  delay(1000);
}
```

### 示例：扫描 Wi-Fi 网络

```cpp
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  
  Serial.println("正在扫描 Wi-Fi 网络...");
  int numNetworks = WiFi.scanNetworks();
  
  Serial.print("发现 ");
  Serial.print(numNetworks);
  Serial.println(" 个网络");
  
  for (int i = 0; i < numNetworks; i++) {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.println(" dBm)");
  }
}

void loop() {
  delay(1000);
}
```