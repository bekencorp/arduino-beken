/**
 * HTTPClientMinimal.ino
 *
 *  Adapted for arduino-beken:
 *    - waits for WSS_GOT_IP before HTTP
 *    - uses http.begin(WiFiClient&, url)
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

#include <WiFi.h>
#include <HTTPClient.h>

WiFiClient client;
HTTPClient http;

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println();
  Serial.print("Connecting to WiFi... ");

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WSS_GOT_IP) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  const char *url = "http://example.com/";

  Serial.print("HTTP GET ");
  Serial.println(url);

  if (!http.begin(client, url)) {
    Serial.println("HTTP begin failed");
    return;
  }

  int code = http.GET();
  Serial.print("HTTP code=");
  Serial.println(code);

  if (code > 0) {
    String body = http.getString();
    Serial.print("HTTP body length=");
    Serial.println(body.length());
    if (body.length() > 0) {
      Serial.print("HTTP body preview=");
      Serial.println(body.substring(0, body.length() > 120 ? 120 : body.length()));
    }
  } else {
    Serial.print("HTTP error: ");
    Serial.println(HTTPClient::errorToString(code));
  }

  http.end();
}

void loop() {
  delay(1000);
}
