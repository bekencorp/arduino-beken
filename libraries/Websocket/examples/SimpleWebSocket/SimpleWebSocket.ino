/*
  Simple WebSocket client (WSS)

  Connects to a secure WebSocket echo server over TLS, sends a hello
  message every 5 seconds, and prints any text reply.

  Adapted for arduino-beken (BK7239N):
    - uses WSS_* status codes (not ESP WL_*)
    - uses WiFiClientSecure for wss:// (TLS under WebSocketClient)
*/

// -------- WiFi configuration (edit before upload) --------
#ifndef WIFI_SSID
#define WIFI_SSID "your-ssid"
#endif
#ifndef WIFI_PASS
#define WIFI_PASS "your-password"
#endif
// ----------------------------------------------------------

// -------- WebSocket server (edit before upload) -----------
// Public echo: wss://echo.websocket.org/ (TLS on 443)
#ifndef WS_HOST
#define WS_HOST "echo.websocket.org"
#endif
#ifndef WS_PORT
#define WS_PORT 443
#endif
#ifndef WS_PATH
#define WS_PATH "/"
#endif
// Set to 1 to skip certificate verification (INSECURE, debug only)
#ifndef WS_INSECURE
#define WS_INSECURE 1
#endif
// ----------------------------------------------------------

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Websocket.h>

WiFiClientSecure wifi;
WebSocketClient client = WebSocketClient(wifi, WS_HOST, WS_PORT);
int count = 0;

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait for WSS_GOT_IP (arduino-beken), not WL_CONNECTED
  while (WiFi.status() != WSS_GOT_IP) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

#if WS_INSECURE
  // Debug only: skip certificate verification (needed for quick public-echo tests)
  wifi.setInsecure();
  Serial.println("WARNING: TLS certificate verification disabled (WS_INSECURE=1)");
#else
  // Production: call wifi.setCACert(rootCaPem) for the server's issuer
  Serial.println("TLS: setCACert required when WS_INSECURE=0");
#endif

  // Allow time for TLS + WebSocket handshake
  wifi.setTimeout(30000);
}

void loop() {
  Serial.print("starting WebSocket client wss://");
  Serial.print(WS_HOST);
  Serial.print(":");
  Serial.print(WS_PORT);
  Serial.println(WS_PATH);

  // Start WebSocket handshake (HTTP Upgrade over the TLS socket)
  int status = client.begin(WS_PATH);
  if (status != 0) {
    Serial.print("WebSocket begin failed, status=");
    Serial.println(status);
    delay(5000);
    return;
  }

  Serial.println("WebSocket connected");

  while (client.connected()) {
    Serial.print("Sending hello ");
    Serial.println(count);

    // send a hello #
    client.beginMessage(TYPE_TEXT);
    client.print("hello ");
    client.print(count);
    client.endMessage();

    // increment count for next message
    count++;

    // check if a message is available to be received
    int messageSize = client.parseMessage();

    if (messageSize > 0) {
      Serial.println("Received a message:");
      Serial.println(client.readString());
    }

    // wait 5 seconds
    delay(5000);
  }

  Serial.println("disconnected");
  delay(2000);
}
