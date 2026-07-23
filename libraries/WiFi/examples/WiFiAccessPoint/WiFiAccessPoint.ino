/*
  WiFi Access Point - SoftAP HTTP LED control
  Adapted for arduino-beken: softAP + WiFiServer.
*/

// -------- AP configuration (edit before upload) --------
#ifndef AP_SSID
#define AP_SSID "yourAP"
#endif
#ifndef AP_PASS
#define AP_PASS "yourPassword"
#endif
// ----------------------------------------------------------

#include <WiFi.h>

WiFiServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("Configuring access point...");

  WiFi.softAPConfig(IPAddress(192, 168, 0, 1), IPAddress(192, 168, 0, 1),
                    IPAddress(255, 255, 255, 0));

  // Password must be empty (open) or at least 8 characters
  if (!WiFi.softAP(AP_SSID, AP_PASS)) {
    Serial.println("Soft AP creation failed.");
    while (1) {
      delay(1000);
    }
  }

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print("Click <a href=\"/H\">here</a> to turn ON the LED.<br>");
            client.print("Click <a href=\"/L\">here</a> to turn OFF the LED.<br>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /H")) {
          digitalWrite(LED_BUILTIN, HIGH);
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, LOW);
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
