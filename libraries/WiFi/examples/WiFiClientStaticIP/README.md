# WiFi Client - Static IP Example

## Overview

This example demonstrates how to connect to a WiFi network in station mode (STA) using a static IP address, then perform HTTP GET requests to a remote server via `WiFiClient`. It is useful when you need a fixed LAN address for port forwarding or device discovery.

## Features

- Configure static IP, gateway, subnet mask, and DNS with `WiFi.config()`
- Connect to WiFi and wait for `WSS_GOT_IP`
- Print IP, MAC, subnet mask, gateway, and DNS
- Periodically send HTTP GET requests to `example.com`
- Client connection timeout protection

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring
- 2.4 GHz WiFi router
- Internet access (for resolving and reaching `example.com`)

## Configuration

Update the common WiFi configuration block at the top of the sketch:

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

Static IP settings must match your router's LAN subnet:

```cpp
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);     // optional
IPAddress secondaryDNS(8, 8, 4, 4);   // optional
```

HTTP target (optional):

```cpp
const char* host = "example.com";
const char* url  = "/";
```

Compile-time credential override:

```bash
-DWIFI_SSID=\"...\" -DWIFI_PASS=\"...\"
```

## How It Works

### 1. Static IP Configuration
- Initialize serial at 115200 baud
- Call `WiFi.config()` before `WiFi.begin()`
- Print `STA Failed to configure` if configuration fails

### 2. WiFi Connection
- Call `WiFi.begin()` to join the router
- Poll `WiFi.status()` until `WSS_GOT_IP`
- Print IP, MAC, subnet mask, gateway, and DNS

### 3. HTTP Requests
- Every 5 seconds, create a `WiFiClient` and connect to `host:80`
- Send an HTTP GET request
- Read and print the server response
- Timeout after 5 seconds if no data arrives

## Usage

1. Set static IP, gateway, and subnet to match your router
2. Update `WIFI_SSID` and `WIFI_PASS`
3. Ensure the static IP is not already in use
4. Upload the sketch to your board
5. Open Serial Monitor at 115200 baud
6. Confirm the printed IP matches your configured static IP
7. Observe HTTP request responses

## Expected Output

```
Connecting to your-ssid
.....
WiFi connected
IP address: 192.168.1.100
MAC Address: AA:BB:CC:DD:EE:FF
Subnet Mask: 255.255.255.0
Gateway IP: 192.168.1.1
DNS: 8.8.8.8
connecting to example.com
Requesting URL: /
HTTP/1.1 200 OK
...
closing connection
```

## Troubleshooting

**`STA Failed to configure`:**
- Verify IP, gateway, and subnet mask format
- Ensure the static IP is within the router subnet

**Stuck printing `.` without connecting:**
- Check WiFi credentials
- Confirm 2.4 GHz is enabled on the router
- Use WiFiScan to verify the network is visible

**IP address does not match configuration:**
- Confirm `local_IP` / `gateway` / `subnet` match the current router LAN (e.g. do not keep `192.168.1.x` on a `10.37.45.x` network)
- Confirm the static IP is not already in use
- After a successful connect, the printed `IP address:` must equal `WiFi.config()`'s `local_IP`; if you still see a DHCP-assigned address, rebuild/flash with the static-IP fix
- `secondaryDNS` is API-compatible only; only `primaryDNS` is applied

**`connection failed` or HTTP timeout:**
- Confirm internet access and DNS resolution
- Try changing `host` to a LAN HTTP server for isolation testing

## Notes

- Static IP must be unused and within the LAN subnet
- This example uses Beken `WSS_*` status codes; wait for `WSS_GOT_IP`, not `WL_CONNECTED`
- Call `WiFi.config()` before `WiFi.begin()`; the library calls `bk_netif_static_ip()` before association and re-arms it after disconnect so DHCP is not restarted
- `secondaryDNS` is API-compatible only; only `primaryDNS` is applied
- Gateway and DNS must be correct for outbound HTTP
- HTTP requests repeat every 5 seconds by default

## Related Examples

- WiFiMinimal - Minimal WiFi connection and status
- WiFiClientConnect - STA connect and disconnect
- WiFiClient - ThingSpeak HTTP client
- WiFiScan - Scan nearby networks
