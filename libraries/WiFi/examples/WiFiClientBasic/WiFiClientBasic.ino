/*
  WiFi Client Basic - connect with WiFiMulti
  Adapted for arduino-beken: uses WSS_* / WL_CONNECTED via WiFiMulti.run().
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
#include <WiFiMulti.h>

WiFiMulti wifiMulti;

void setup() {
  Serial.begin(115200);
  delay(500);

  // Register AP credentials (add more with addAP if needed)
  wifiMulti.addAP(WIFI_SSID, WIFI_PASS);

  Serial.println();
  Serial.print("Waiting for WiFi... ");

  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Waiting for IP (WSS_GOT_IP)... ");

  while (WiFi.status() != WSS_GOT_IP) {
    Serial.print(".");
    delay(100);
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
}

void loop() {
  // Public HTTP endpoint used to verify DNS, TCP, and HTTP connectivity.
  // WiFiClient is a plain TCP client, so use HTTP port 80 rather than HTTPS.
  const uint16_t port = 80;
  const char *host = "example.com";
  const char *url = "/";

  if (WiFi.status() != WSS_GOT_IP) {
    Serial.println("WiFi not ready, reconnecting...");
    wifiMulti.run();
    delay(1000);
    return;
  }

  Serial.print("Connecting to ");
  Serial.print(host);
  Serial.print(":");
  Serial.println(port);

  WiFiClient client;
  if (!client.connect(host, port)) {
    Serial.println("Connection failed.");
    Serial.println("Waiting 5 seconds before retrying...");
    delay(5000);
    return;
  }

  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: arduino-beken/1.0\r\n" +
               "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Client timeout.");
      client.stop();
      delay(5000);
      return;
    }
  }

  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("Closing connection.");
  client.stop();
  Serial.println("Waiting 5 seconds before restarting...");
  delay(5000);
}
