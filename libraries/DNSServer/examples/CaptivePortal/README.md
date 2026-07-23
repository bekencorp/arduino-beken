# Captive Portal Example

## Overview

This example creates a WiFi captive portal using the DNSServer library. When clients connect to the SoftAP, all DNS queries are redirected to the board's IP address (192.168.4.1), causing any web request to show the captive portal page. This demonstrates the core concept of "DNS hijacking" for captive portal implementations.

## Features

- Creates WiFi SoftAP (Access Point)
- DNS server on port 53: redirects all domain queries to the AP IP
- HTTP server on port 80: serves captive portal HTML page
- Serial logging of AP IP and client connections
- Configurable AP SSID and password

## Hardware Requirements

- Beken BK7239N development board (arduino-beken)
- USB cable (for programming and serial monitoring)
- WiFi-enabled device (smartphone, PC, or tablet)

## Configuration

Update the AP configuration section at the top of the code:

```cpp
// -------- AP configuration (edit before upload) --------
#ifndef AP_SSID
#define AP_SSID "DNSServer"
#endif
#ifndef AP_PASS
#define AP_PASS ""
#endif
// ----------------------------------------------------------
```

Or pass values at compile time: `-DAP_SSID="MyPortal" -DAP_PASS="password"`

AP password rules:
- Empty string for open (no password)
- At least 8 characters for WPA2-PSK encryption

## How It Works

### 1. SoftAP Setup
- Configures WiFi in AP mode with fixed IP (192.168.4.1)
- Creates SoftAP with specified SSID and password
- Prints AP IP to serial monitor

### 2. DNS Server
- Starts DNS server on port 53 (standard DNS port)
- `dnsServer.start(DNS_PORT, "*", apIP)` redirects all domain queries to apIP
- When clients resolve any domain, they get the board's IP instead

### 3. HTTP Server
- Listens on port 80 for incoming HTTP connections
- When a client connects, reads the HTTP request
- Responds with a simple HTML captive portal page

## Usage

1. Modify `AP_SSID` and `AP_PASS` if needed
2. Upload the sketch to the development board
3. Open serial monitor (115200 baud)
4. Connect your device to the AP SSID
5. Open any website in a browser - you will see the captive portal page

## Expected Output

```
Starting Captive Portal...
AP IP address: 192.168.4.1
DNS server started on port 53
HTTP server started
New HTTP client connected
HTTP client disconnected
```

## Troubleshooting

**Soft AP creation failed:**
- Password must be empty or at least 8 characters
- Try changing SSID if there's channel conflict

**Cannot connect to AP:**
- Verify SSID and password match configuration
- Move closer to the board; SoftAP range is limited

**DNS redirect not working / any URL does not show portal:**
- SoftAP DHCP used to bind port 53 and reply REFUSED; this build disables that so DNSServer can hijack.
- Forget the WiFi network on the phone and reconnect (pick up new DHCP DNS = AP IP).
- Confirm serial shows `DNS server started on port 53` (not start failed).
- First try `http://192.168.4.1` — if that works but other URLs do not, DNS is still wrong on the client.
- Use: `nslookup google.com 192.168.4.1`

**Browser shows connection refused:**
- Check serial output for server startup message
- Verify HTTP server is running on port 80

## Notes

- Captive portals rely on clients using the AP's DNS server
- Some mobile devices may show their built-in captive portal detection
- The wildcard `"*"` in `dnsServer.start()` matches all domain names
- No external router required - clients connect directly to the board

## Related Examples

- WiFiAccessPoint - Basic SoftAP with HTTP server
- WiFiClient - Connect to external WiFi networks
- WiFiServer - TCP server in STA mode