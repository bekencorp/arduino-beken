# WiFi Scan Example

## Overview

This example demonstrates how to scan for available WiFi networks in the vicinity. It performs periodic scans and displays detailed information about each detected network, including SSID, signal strength (RSSI), channel, and encryption type. This is useful for network discovery, site surveys, and choosing the best network to connect to.

## Features

- **Network Discovery**: Scans for all available WiFi networks in range
- **Detailed Information**: Displays SSID, RSSI, channel, and encryption type in a fixed-width table
- **Periodic Scanning**: Continuously scans at regular intervals
- **Encryption Detection**: Identifies security protocols (Open, WEP, WPA, WPA2, WPA3)
- **Memory Management**: Properly cleans up scan results to free memory
- **Hidden SSID**: Empty SSID is shown as `<hidden>`

## Hardware Requirements

- Beken BK7239N or BK7236N board (arduino-beken)
- USB cable for programming and serial monitoring

## Configuration

No WiFi credentials are needed for this example. The sketch operates in station mode but doesn't connect to any network.

## How It Works

### Scan Process

1. **WiFi Initialization**: Set WiFi to station mode without connecting
2. **Network Scanning**: Call `WiFi.scanNetworks()` to scan for available networks
3. **Result Processing**: Iterate through scan results and display information
4. **Memory Cleanup**: Delete scan results to free memory
5. **Repeat**: Wait 5 seconds and scan again

### WiFi Modes

```cpp
WiFi.mode(WIFI_STA);    // Set to station mode
WiFi.disconnect();       // Ensure not connected
```

### Scanning

```cpp
int n = WiFi.scanNetworks();
```

This function:
- Blocks until scan completes (via `EVENT_WIFI_SCAN_DONE`)
- Returns number of networks found
- Returns `WIFI_SCAN_FAILED` (-2) on failure
- Returns `WIFI_SCAN_RUNNING` (-1) if scan still in progress
- Results accessible via `WiFi.SSID()`, `WiFi.RSSI()`, etc.

### Network Information Access

For each network found:

```cpp
WiFi.SSID(i)           // Network name
WiFi.RSSI(i)           // Signal strength in dBm
WiFi.channel(i)        // WiFi channel
WiFi.encryptionType(i) // Security protocol (WF_AP_AUTH_MODE_E)
```

### Memory Management

```cpp
WiFi.scanDelete();  // Free memory used by scan results
```

## Usage

1. **Upload Sketch**: Upload to your board
2. **Open Serial Monitor**: Set baud rate to 115200
3. **Observe Scans**: Watch as networks are discovered every 5 seconds

### Expected Output

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

## Understanding the Output

### RSSI (Signal Strength)

| RSSI Range | Quality | Description |
|------------|---------|-------------|
| -30 to -50 dBm | Excellent | Very strong signal |
| -50 to -60 dBm | Good | Strong, reliable connection |
| -60 to -70 dBm | Fair | Acceptable, may have issues |
| -70 to -80 dBm | Weak | Poor connection quality |
| -80 to -90 dBm | Very Weak | Unstable, frequent drops |
| Below -90 dBm | Unusable | Too weak to maintain connection |

### Encryption Types

The example recognizes these security protocols:

- **WAAM_OPEN**: No encryption (public network)
- **WAAM_WEP**: WEP (obsolete, insecure)
- **WAAM_WPA_PSK**: WPA with pre-shared key
- **WAAM_WPA2_PSK**: WPA2 (most common)
- **WAAM_WPA_WPA2_PSK**: Mixed WPA/WPA2
- **WAAM_WPA_WPA3_SAE**: WPA3 (newest, most secure)
- **unknown**: Unrecognized encryption

## Troubleshooting

### No Networks Found

- Verify WiFi antenna is properly connected
- Check if WiFi is enabled in your area
- Ensure board is not in metal enclosure (shields RF)
- Try moving to different location

### Scan Failed (`scan failed`)

- Retry after a few seconds; first scan after boot may race with WiFi init
- Ensure no other scan is already running

### Incomplete Network List

- Some networks may be hidden (not broadcasting SSID)
- Very weak signals may not be detected

## Notes

- Scanning works without connecting to any network
- Station mode is required for scanning
- A short delay after scan helps avoid SDK log output mixing with the table
- Always call `WiFi.scanDelete()` after processing results

## Related Examples

- WiFiMinimal - Minimal WiFi connect and status
- WiFiClient - TCP client and ThingSpeak HTTP
