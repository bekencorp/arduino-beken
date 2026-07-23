/*
  CaptivePortal - SoftAP DNS hijack + HTTP portal page
  Adapted for arduino-beken (BK7239N).

  Uses DNSServer with domain "*" so every DNS query resolves to the SoftAP IP.
  A simple WiFiServer on port 80 serves the captive portal HTML.
*/

// -------- AP configuration (edit before upload) --------
#ifndef AP_SSID
#define AP_SSID "DNSServer"
#endif
#ifndef AP_PASS
#define AP_PASS ""
#endif
// ----------------------------------------------------------

#include <WiFi.h>
#include <DNSServer.h>

const byte DNS_PORT = 53;
// SoftAP / captive-portal IP (must match softAPConfig below)
IPAddress apIP(192, 168, 4, 1);

DNSServer dnsServer;
WiFiServer server(80);

String responseHTML = ""
                      "<!DOCTYPE html><html><head><title>CaptivePortal</title></head><body>"
                      "<h1>Hello World!</h1><p>This is a captive portal example. All requests will "
                      "be redirected here.</p></body></html>";

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Starting Captive Portal...");

  // AP-only mode on arduino-beken
  WiFi.mode(WIFI_AP);

  // Fixed SoftAP IP so DNS replies and HTTP share the same address
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));

  // Password must be empty (open) or at least 8 characters
  if (!WiFi.softAP(AP_SSID, AP_PASS)) {
    Serial.println("Soft AP creation failed.");
    while (1) {
      delay(1000);
    }
  }

  // Let SoftAP / DHCP settle before binding DNS
  delay(200);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  // "*" replies with apIP for every DNS query (captive-portal hijack)
  if (!dnsServer.start(DNS_PORT, "*", apIP)) {
    Serial.println("DNS server start failed.");
  } else {
    Serial.println("DNS server started on port 53");
  }

  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Connect to AP, then open any http URL (or http://192.168.4.1).");
}

void loop() {
  dnsServer.processNextRequest();

  // Listen for incoming HTTP clients
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New HTTP client connected");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          // Blank line ends the HTTP request headers
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(responseHTML);
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      } else {
        yield(); // feed watchdog while waiting for HTTP data
      }
    }
    client.stop();
    Serial.println("HTTP client disconnected");
  }
}
