/*
  WiFi Client Events - register WiFi event callbacks
  Adapted for arduino-beken: simplified onEvent API (arduino_event_id_t).
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

void WiFiEvent(arduino_event_id_t event) {
  Serial.print("[WiFi-event] event: ");
  Serial.println(static_cast<int>(event));

  switch (event) {
    case ARDUINO_EVENT_WIFI_SCAN_DONE:
      Serial.println("Completed scan for access points");
      break;
    case ARDUINO_EVENT_WIFI_STA_START:
      Serial.println("WiFi client started");
      break;
    case ARDUINO_EVENT_WIFI_STA_STOP:
      Serial.println("WiFi client stopped");
      break;
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("Connected to access point");
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("Disconnected from WiFi access point");
      break;
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("Obtained IP address: ");
      Serial.println(WiFi.localIP());
      break;
    default:
      break;
  }
}

void WiFiGotIP(arduino_event_id_t event) {
  if (event == ARDUINO_EVENT_WIFI_STA_GOT_IP) {
    Serial.println("WiFi connected (GOT_IP handler)");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  WiFi.disconnect(true);
  delay(1000);

  WiFi.onEvent(WiFiEvent);
  WiFi.onEvent(WiFiGotIP, ARDUINO_EVENT_WIFI_STA_GOT_IP);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.println();
  Serial.println("Wait for WiFi...");
}

void loop() {
  delay(1000);
}
