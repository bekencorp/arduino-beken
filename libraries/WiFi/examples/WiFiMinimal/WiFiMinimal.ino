/*
  WiFi Minimal - single AP STA connect
  Adapted for arduino-beken: waits for WSS_GOT_IP.
*/

// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
// ----------------------------------------------------------

#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WSS_GOT_IP) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  Serial.print("status=");
  Serial.print(static_cast<int>(WiFi.status()));
  Serial.print(" ip=");
  Serial.println(WiFi.localIP());
  delay(5000);
}
