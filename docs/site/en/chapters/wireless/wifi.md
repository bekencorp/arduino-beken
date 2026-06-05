# Wi-Fi

The BK7239N Wi-Fi module is designed to address congestion issues in high-density IoT scenarios.It supports the 802.11ax (Wi-Fi 6) standard and covers both 2.4GHz and 5GHz dual frequency bands.

**Wi-Fi Capabilities Overview**

| Feature | Description |
|---------|-------------|
| **Operating Modes** | Station (STA) |
| **Wireless Standards** | IEEE 802.11 b/g/n/ax (Wi-Fi 6) |
| **Frequency Band** | 2.4 GHz + 5 GHz (Dual Band) |
| **Security** | WEP, WPA/WPA2 Personal, WPA3 |

*Note: AP mode is under development and will be supported in future releases.*

## Wi-Fi API Reference

### begin()

Connect to a Wi-Fi network.

```cpp
bool WiFi.begin(const char* ssid);
bool WiFi.begin(const char* ssid, const char* passphrase);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `ssid` | `const char*` | Network SSID |
| `passphrase` | `const char*` | Network password (for secured networks) |

**Return:** `true` if connection initiated successfully, `false` otherwise.

### disconnect()

Disconnect from the current Wi-Fi network.

```cpp
bool WiFi.disconnect(bool wifioff = false);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `wifioff` | `bool` | Turn off Wi-Fi radio if `true` |

**Return:** `true` if disconnected successfully, `false` otherwise.

### status()

Get the current connection status.

```cpp
wl_status_t WiFi.status();
```

**Return:** Connection status (see Status Constants).

### SSID()

Get the SSID of the connected network or scanned network by index.

```cpp
String WiFi.SSID(uint8_t index = WIFI_SCAN_RUNNING);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `uint8_t` | Index of scanned network (omit for connected network) |

**Return:** SSID string.

### RSSI()

Get the signal strength (RSSI) of the connected network or scanned network by index.

```cpp
int32_t WiFi.RSSI(uint8_t index = WIFI_SCAN_RUNNING);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `uint8_t` | Index of scanned network (omit for connected network) |

**Return:** RSSI value in dBm.

### BSSID()

Get the BSSID (MAC address) of the connected network.

```cpp
uint8_t* WiFi.BSSID();
```

**Return:** Pointer to BSSID array.

### encryptionType()

Get the encryption type of the connected network or scanned network by index.

```cpp
uint8_t WiFi.encryptionType(uint8_t index = WIFI_SCAN_RUNNING);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `index` | `uint8_t` | Index of scanned network (omit for connected network) |

**Return:** Encryption type.

### scanNetworks()

Start scanning for available Wi-Fi networks.

```cpp
int8_t WiFi.scanNetworks(bool async = true, bool show_hidden = false, uint8_t channel = 0, const char* ssid = NULL);
```

| Parameter | Type | Description |
|-----------|------|-------------|
| `async` | `bool` | Perform scan asynchronously if `true` |
| `show_hidden` | `bool` | Show hidden networks if `true` |
| `channel` | `uint8_t` | Specific channel to scan (0 = all channels) |
| `ssid` | `const char*` | Specific SSID to scan for |

**Return:** Number of networks found (when sync), or scan ID (when async).

### Status Constants

| Constant | Description |
|----------|-------------|
| `WL_IDLE_STATUS` | Idle status |
| `WL_NO_SSID_AVAIL` | SSID not available |
| `WL_SCAN_COMPLETED` | Scan completed |
| `WL_CONNECTED` | Connected to Wi-Fi |
| `WL_CONNECT_FAILED` | Connection failed |
| `WL_CONNECTION_LOST` | Connection lost |
| `WL_DISCONNECTED` | Disconnected from Wi-Fi |

## Wi-Fi Examples

Connect to an existing Wi-Fi network.

### Example: Connect to Wi-Fi

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
  // Check connection status
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  }
  delay(1000);
}
```

### Example: Scan Wi-Fi Networks

```cpp
#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  
  Serial.println("Scanning for Wi-Fi networks...");
  int numNetworks = WiFi.scanNetworks();
  
  Serial.print("Found ");
  Serial.print(numNetworks);
  Serial.println(" networks");
  
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