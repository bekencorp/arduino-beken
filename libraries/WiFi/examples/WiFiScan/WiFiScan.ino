/*
  WiFi Scan - list nearby APs
  Adapted for arduino-beken.
*/

#include <WiFi.h>

static const char *encryptionTypeStr(WF_AP_AUTH_MODE_E mode) {
  switch (mode) {
    case WAAM_OPEN:
      return "open";
    case WAAM_WEP:
      return "WEP";
    case WAAM_WPA_PSK:
      return "WPA PSK";
    case WAAM_WPA2_PSK:
      return "WPA2 PSK";
    case WAAM_WPA_WPA2_PSK:
      return "WPA+WPA2";
    case WAAM_WPA_WPA3_SAE:
      return "WPA3";
    default:
      return "unknown";
  }
}

static const char *ssidForPrint(uint8_t index) {
  const String ssid = WiFi.SSID(index);
  return ssid.length() > 0 ? ssid.c_str() : "<hidden>";
}

static void printScanTable(int count) {
  Serial.println("Nr | SSID                             | RSSI | CH  | Encryption");
  Serial.println("--- + -------------------------------- + ---- + --- + ----------");

  char line[96];
  for (int i = 0; i < count; ++i) {
    snprintf(line, sizeof(line), "%3d | %-32.32s | %4d | %3d | %s", i + 1,
             ssidForPrint(static_cast<uint8_t>(i)), WiFi.RSSI(i), WiFi.channel(i),
             encryptionTypeStr(WiFi.encryptionType(static_cast<uint8_t>(i))));
    Serial.println(line);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  Serial.println("Setup done");
}

void loop() {
  Serial.println("Scan start");

  int n = WiFi.scanNetworks();

  // Let SDK finish scan-related log output before printing the table.
  delay(300);
  Serial.println("Scan done");

  if (n == WIFI_SCAN_FAILED) {
    Serial.println("scan failed");
  } else if (n == WIFI_SCAN_RUNNING) {
    Serial.println("scan in progress");
  } else if (n == 0) {
    Serial.println("no networks found");
  } else if (n > 0) {
    Serial.print(n);
    Serial.println(" networks found");
    printScanTable(n);
  }

  Serial.println("");
  WiFi.scanDelete();
  delay(5000);
}
