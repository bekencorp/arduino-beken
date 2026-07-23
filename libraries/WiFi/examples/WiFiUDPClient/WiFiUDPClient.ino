/*
  WiFi UDP Client - periodic UDP broadcast/unicast
  Adapted for arduino-beken: uses WSS_* status codes and onEvent for GOT_IP.
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
#include <WiFiUdp.h>

// IP address to send UDP data to (broadcast or unicast)
const char *udpAddress = "192.168.1.255";  // Change to match your LAN broadcast or receiver host IP
const int udpPort = 3333;

boolean connected = false;
WiFiUDP udp;

void onWiFiEvent(arduino_event_id_t event) {
  switch (event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.print("WiFi connected! IP address: ");
      Serial.println(WiFi.localIP());
      udp.begin(WiFi.localIP(), udpPort);
      connected = true;
      break;
    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      connected = false;
      break;
    default:
      break;
  }
}

void connectToWiFi(const char *ssid, const char *pwd) {
  Serial.println("Connecting to WiFi network: " + String(ssid));

  WiFi.disconnect(true);
  WiFi.onEvent(onWiFiEvent);

  WiFi.begin(ssid, pwd);
  Serial.println("Waiting for WiFi connection...");
}

void setup() {
  Serial.begin(115200);
  delay(500);
  connectToWiFi(WIFI_SSID, WIFI_PASS);
}

void loop() {
  if (connected) {
    udp.beginPacket(udpAddress, udpPort);
    udp.print("Seconds since boot: ");
    udp.print(millis() / 1000);
    udp.endPacket();
  }
  delay(1000);
}
