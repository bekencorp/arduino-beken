/**
 * Authorization.ino
 *
 *  Adapted for arduino-beken:
 *    - uses WiFiMulti + external WiFiClient for HTTPClient::begin()
 *    - waits for WSS_GOT_IP before starting HTTP
 *    - demonstrates setAuthorization(user, password)
 *
 */

// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"  // Change this to your WiFi SSID
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"  // Change this to your WiFi password
#endif
// ----------------------------------------------------------

// ----- Basic Authorization configuration (edit before upload) -----
#ifndef AUTH_HTTP_URL
// Online Basic Auth test endpoint (user/passwd must match URL path)
#define AUTH_HTTP_URL "http://httpbin.org/basic-auth/user/passwd"
#endif
#ifndef AUTH_USER
#define AUTH_USER "user"  // Change this to the Basic Auth username
#endif
#ifndef AUTH_PASS
#define AUTH_PASS "passwd"  // Change this to the Basic Auth password
#endif
// ------------------------------------------------------------------

#include <Arduino.h>

#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial

WiFiMulti wifiMulti;
WiFiClient client;  // External client required by http.begin(client, url)

/*
 * Send one HTTP GET. When useAuth is true, attach Basic Authorization.
 *
 * Recommended path on arduino-beken:
 *   http.begin(client, url);
 *   http.setAuthorization(user, password);
 *
 * Alternatives kept for reference (not used by this sketch):
 *   http.begin(client, "http://user:passwd@httpbin.org/basic-auth/user/passwd");
 *   http.setAuthorization("dXNlcjpwYXNzd2Q=");  // already base64-encoded user:passwd
 */
static void doAuthorizedGet(const char *label, bool useAuth, const char *user, const char *password) {
  HTTPClient http;

  USE_SERIAL.println();
  USE_SERIAL.print("[HTTP] begin ");
  USE_SERIAL.println(label);

  // Configure target server and URL
  if (!http.begin(client, AUTH_HTTP_URL)) {
    USE_SERIAL.println("[HTTP] begin failed");
    return;
  }

  if (useAuth) {
    // Attach Authorization: Basic <base64(user:password)>
    USE_SERIAL.print("[HTTP] setAuthorization(");
    USE_SERIAL.print(user);
    USE_SERIAL.println(", ...)");
    http.setAuthorization(user, password);
  } else {
    // Intentionally omit Authorization to show the rejected path
    USE_SERIAL.println("[HTTP] no Authorization header");
  }

  USE_SERIAL.println("[HTTP] GET...");
  // Start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been sent and Server response header has been handled
    USE_SERIAL.print("[HTTP] GET... code: ");
    USE_SERIAL.println(httpCode);

    // Print body for both success and non-OK status codes
    String payload = http.getString();
    if (httpCode == HTTP_CODE_OK) {
      USE_SERIAL.println(payload);
    } else {
      USE_SERIAL.print("[HTTP] body length: ");
      USE_SERIAL.println(payload.length());
      if (payload.length()) {
        USE_SERIAL.println(payload);
      }
    }
  } else {
    USE_SERIAL.print("[HTTP] GET... failed, error: ");
    USE_SERIAL.println(HTTPClient::errorToString(httpCode));
  }

  http.end();
}

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

  USE_SERIAL.print("[SETUP] URL: ");
  USE_SERIAL.println(AUTH_HTTP_URL);
  USE_SERIAL.print("[SETUP] user: ");
  USE_SERIAL.println(AUTH_USER);

  // We start by connecting to a WiFi network
  wifiMulti.addAP(WIFI_SSID, WIFI_PASS);
}

void loop() {
  // Wait for WiFi association
  if (wifiMulti.run() == WL_CONNECTED) {
    // Wait for WSS_GOT_IP  (arduino-beken) before starting TCP/HTTP
    if (WiFi.status() != WSS_GOT_IP) {
      USE_SERIAL.println("[HTTP] WiFi connected, waiting for IP...");
      delay(500);
      return;
    }

    // SUCCESS ---- correct Basic Authorization credentials
    doAuthorizedGet("with correct auth", true, AUTH_USER, AUTH_PASS);
    delay(1000);

    // FAIL ---- request without Authorization header
    doAuthorizedGet("without auth", false, NULL, NULL);
  } else {
    USE_SERIAL.println("[HTTP] WiFi not connected");
  }

  delay(10000);
}
