/**
 * StreamHttpClient.ino
 *
 *  Adapted for arduino-beken: uses WiFiMulti and waits for WSS_GOT_IP before HTTP.
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

    HTTPClient http;

    USE_SERIAL.print("[HTTP] begin...\n");

    // Configure server and URL
    if (!http.begin(client, httpUrl)) {
      USE_SERIAL.println("[HTTP] begin failed");
      delay(5000);
      return;
    }

    USE_SERIAL.print("[HTTP] GET...\n");
    // Start connection and send HTTP header
    int httpCode = http.GET();
    if (httpCode > 0) {
      // HTTP header has been sent and Server response header has been handled
      USE_SERIAL.print("[HTTP] GET... code: ");
      USE_SERIAL.println(httpCode);

      // File found at server
      if (httpCode == HTTP_CODE_OK) {
        // Get length of document (-1 when server sends no Content-Length header)
        int len = http.getSize();
        USE_SERIAL.print("[HTTP] Content-Length: ");
        USE_SERIAL.println(len);

        // Create buffer for read
        uint8_t buff[128] = {0};

        // Get TCP stream
        WiFiClient *stream = http.getStreamPtr();
        if (!stream) {
          USE_SERIAL.println("[HTTP] stream pointer is null");
          http.end();
          delay(5000);
          return;
        }

        int totalRead = 0;

        // Read all data from server
        while (http.connected() && (len > 0 || len == -1)) {
          // Get available data size
          size_t size = stream->available();

          if (size) {
            // Read up to 128 bytes
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

            // Write it to Serial
            USE_SERIAL.write(buff, c);
            totalRead += c;

            if (len > 0) {
              len -= c;
            }
          }
          delay(1);
        }

        USE_SERIAL.println();
        USE_SERIAL.print("[HTTP] stream bytes read: ");
        USE_SERIAL.println(totalRead);
        USE_SERIAL.print("[HTTP] connection closed or file end.\n");
      }
    } else {
      USE_SERIAL.print("[HTTP] GET... failed, error: ");
      USE_SERIAL.println(HTTPClient::errorToString(httpCode));
    }

    http.end();
  } else {
    USE_SERIAL.println("[HTTP] WiFi not connected");
  }

  delay(10000);
}
