/*
  WiFi Scan Dual Antenna - optional dual-antenna config then scan
  Adapted for arduino-beken. Requires dual-antenna hardware and setDualAntennaConfig support.
*/

#include <WiFi.h>

// Change GPIO pins to match your board (optional dual-antenna hardware)
const uint8_t ANT1_GPIO = 0;
const uint8_t ANT2_GPIO = 1;

void setup() {
  Serial.begin(115200);
  delay(500);

  WiFi.mode(WIFI_STA);

  bool err = WiFi.setDualAntennaConfig(ANT1_GPIO, ANT2_GPIO, 0, 0);
  if (!err) {
    Serial.println("Dual Antenna configuration failed (stub or unsupported hardware)!");
  } else {
    Serial.println("Dual Antenna configuration successfully done!");
  }

  WiFi.disconnect();
  delay(100);
  Serial.println("Setup done");
}

void loop() {
  Serial.println("scan start");

  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WAAM_OPEN) ? " " : "*");
      delay(10);
    }
  }
  Serial.println();
  delay(5000);
}
