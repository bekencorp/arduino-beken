/*
  WiFi Telnet to Serial bridge (port 23)
  Adapted for arduino-beken: WiFiMulti + WiFiServer.
*/

// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
#ifndef WIFI_SSID2
#define WIFI_SSID2 "your-backup-ssid"
#endif
#ifndef WIFI_PASS2
#define WIFI_PASS2 "your-backup-password"
#endif
// ----------------------------------------------------------

#include <WiFi.h>
#include <WiFiMulti.h>

#define MAX_SRV_CLIENTS 1

WiFiMulti wifiMulti;
WiFiServer server(23);
WiFiClient *serverClients[MAX_SRV_CLIENTS] = {nullptr};

// Use Serial as bridge for demo; replace with Serial1 when UART1 is available on your board
#define UART_BRIDGE Serial

// WiFiClient has no safe copy/assign; construct on heap from server.available() return value directly.
static void freeClient(uint8_t idx) {
  if (serverClients[idx] != nullptr) {
    delete serverClients[idx];
    serverClients[idx] = nullptr;
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("\nConnecting");

  wifiMulti.addAP(WIFI_SSID, WIFI_PASS);
  wifiMulti.addAP(WIFI_SSID2, WIFI_PASS2);

  Serial.println("Connecting WiFi");
  for (int loops = 10; loops > 0; loops--) {
    if (wifiMulti.run() == WL_CONNECTED) {
      WiFi.waitForConnectResult(15000);
      Serial.println();
      Serial.print("WiFi connected IP address: ");
      Serial.println(WiFi.localIP());
      break;
    }
    Serial.println(loops);
    delay(1000);
  }

  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi connect failed");
    delay(1000);
  }

  UART_BRIDGE.begin(115200);
  server.begin();
  server.setNoDelay(true);

  Serial.print("Ready! Use 'telnet ");
  Serial.print(WiFi.localIP());
  Serial.println(" 23' to connect");
}

void loop() {
  uint8_t i;
  if (wifiMulti.run() == WL_CONNECTED) {
    if (server.hasClient()) {
      for (i = 0; i < MAX_SRV_CLIENTS; i++) {
        if (serverClients[i] == nullptr || !serverClients[i]->connected()) {
          freeClient(i);
          serverClients[i] = new WiFiClient(server.available());
          if (serverClients[i] == nullptr) {
            Serial.println("available broken");
          } else {
            Serial.print("New client: ");
            Serial.print(i);
            Serial.print(' ');
            Serial.println(serverClients[i]->remoteIP());
          }
          break;
        }
      }
      if (i >= MAX_SRV_CLIENTS) {
        server.available().stop();
      }
    }

    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (serverClients[i] != nullptr && serverClients[i]->connected()) {
        if (serverClients[i]->available()) {
          while (serverClients[i]->available()) {
            UART_BRIDGE.write(serverClients[i]->read());
          }
        }
      } else {
        freeClient(i);
      }
    }

    if (UART_BRIDGE.available()) {
      size_t len = UART_BRIDGE.available();
      uint8_t *sbuf = (uint8_t *)malloc(len);
      if (sbuf != nullptr) {
        UART_BRIDGE.readBytes(sbuf, len);
        for (i = 0; i < MAX_SRV_CLIENTS; i++) {
          if (serverClients[i] != nullptr && serverClients[i]->connected()) {
            serverClients[i]->write(sbuf, len);
            delay(1);
          }
        }
        free(sbuf);
      }
    }
  } else {
    Serial.println("WiFi not connected!");
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      freeClient(i);
    }
    delay(1000);
  }
}
