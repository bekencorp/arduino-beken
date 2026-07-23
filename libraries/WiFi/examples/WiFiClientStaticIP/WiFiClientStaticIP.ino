/*
  Example of connection using Static IP
  Adapted for arduino-beken: uses WSS_* status codes (not ESP WL_*).
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

const char* host = "example.com";     // Public HTTP test server
const char* url  = "/";               // Path to request

// Static IP settings — adjust to match your router subnet
IPAddress local_IP(192, 168, 1, 100);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);     // optional
IPAddress secondaryDNS(8, 8, 4, 4);   // optional

void setup()
{
  Serial.begin(115200);
  delay(500);

  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait for WSS_GOT_IP (arduino-beken), not WL_CONNECTED
  while (WiFi.status() != WSS_GOT_IP) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS: ");
  Serial.println(WiFi.dnsIP());
}

void loop()
{
  delay(5000);

  Serial.print("connecting to ");
  Serial.println(host);

  IPAddress serverIP;
  Serial.print("DNS in use: ");
  Serial.println(WiFi.dnsIP());
  Serial.print("Resolving ");
  Serial.print(host);
  Serial.print("... ");
  if (!WiFi.hostByName(host, serverIP)) {
    Serial.println("failed");
    Serial.println("Check the static DNS and gateway settings.");
    return;
  }
  Serial.println(serverIP);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  Serial.print("Connecting to ");
  Serial.print(serverIP);
  Serial.print(":");
  Serial.println(httpPort);
  if (!client.connect(serverIP, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: arduino-beken/1.0\r\n" +
               "Connection: close\r\n\r\n");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  // Read all the lines of the reply from server and print them to Serial
  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");
}
