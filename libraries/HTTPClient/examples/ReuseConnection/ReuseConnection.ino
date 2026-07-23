/**
 * ReuseConnection.ino
 *
 *  Adapted for arduino-beken: uses WiFiMulti, external WiFiClient, and WSS_GOT_IP.
 *  Demonstrates HTTP keep-alive via HTTPClient::setReuse(true).
 *
 */

// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
// ----------------------------------------------------------

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;
WiFiClient client;
HTTPClient http;

const char *httpUrl = "http://example.com/";  // Change this to your HTTP server URL

void setup() {
  USE_SERIAL.begin(115200);
  delay(500);

  USE_SERIAL.println();
  USE_SERIAL.println();
  USE_SERIAL.println();

  for (uint8_t t = 4; t > 0; t--) {
    USE_SERIAL.print("[SETUP] WAIT ");
    USE_SERIAL.print(t);
    USE_SERIAL.println("...");
    USE_SERIAL.flush();
    delay(1000);
  }

  // We start by connecting to a WiFi network
  wifiMulti.addAP(WIFI_SSID, WIFI_PASS);

  // Allow reuse (if server supports keep-alive)
  http.setReuse(true);
}

void loop() {
  // Wait for WiFi connection
  if (wifiMulti.run() == WL_CONNECTED) {
    // Wait for WSS_GOT_IP before starting TCP/HTTP on arduino-beken
    if (WiFi.status() != WSS_GOT_IP) {
      USE_SERIAL.println("[HTTP] WiFi connected, waiting for IP...");
      delay(500);
      return;
    }

    // Configure target server and URL (same client is reused when keep-alive works)
    if (!http.begin(client, httpUrl)) {
      USE_SERIAL.println("[HTTP] begin failed");
      delay(1000);
      return;
    }

    // Start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if (httpCode > 0) {
      USE_SERIAL.print("[HTTP] GET... code: ");
      USE_SERIAL.println(httpCode);

      // File found at server
      if (httpCode == HTTP_CODE_OK) {
        http.writeToStream(&USE_SERIAL);
        USE_SERIAL.println();
      }
    } else {
      USE_SERIAL.print("[HTTP] GET... failed, error: ");
      USE_SERIAL.println(HTTPClient::errorToString(httpCode));
    }

    http.end();
  } else {
    USE_SERIAL.println("[HTTP] WiFi not connected");
  }

  delay(1000);
}
