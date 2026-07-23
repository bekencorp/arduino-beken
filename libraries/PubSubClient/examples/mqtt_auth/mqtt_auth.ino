/*
  Basic MQTT example with Authentication

  - connects to WiFi, then an MQTT broker with username/password
  - publishes "hello world" to "outTopic"
  - subscribes to "inTopic"

  Default broker is test.mosquitto.org authenticated listener (port 1884).
  Public test accounts (https://test.mosquitto.org/):
    rw / readwrite, ro / readonly, wo / writeonly

  Adapted for arduino-beken: waits for WSS_GOT_IP.
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
#include <PubSubClient.h>

// Authenticated MQTT test broker (no signup required)
const char* mqtt_server = "test.mosquitto.org";
const uint16_t mqtt_port = 1884;            // authenticated, unencrypted
const char* mqtt_user = "rw";               // Change if using another broker
const char* mqtt_pass = "readwrite";        // Change if using another broker

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

WiFiClient wifiClient;
PubSubClient client(wifiClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // Wait for WSS_GOT_IP (bk_idk), not WL_CONNECTED
  while (WiFi.status() != WSS_GOT_IP) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("mqtt_auth starting");
  Serial.print("Broker ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.print(mqtt_port);
  Serial.print(" user=");
  Serial.println(mqtt_user);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  // Note - the default maximum packet size is 256 bytes. If the
  // combined length of clientId, username and password exceed this use the
  // following to increase the buffer size:
  // client.setBufferSize(255);

  if (client.connect("arduinoClient", mqtt_user, mqtt_pass)) {
    Serial.println("MQTT connected (auth OK)");
    if (client.publish("outTopic", "hello world")) {
      Serial.println("Published announcement to outTopic");
    }
    if (client.subscribe("inTopic")) {
      Serial.println("Subscribed to inTopic");
    }
  } else {
    Serial.print("MQTT connect failed, rc=");
    Serial.println(client.state());
    Serial.println("Hint: use test.mosquitto.org:1884 with rw/readwrite");
  }
}

void loop() {
  if (!client.connected()) {
    static unsigned long lastPrint = 0;
    unsigned long now = millis();
    if (now - lastPrint > 5000) {
      lastPrint = now;
      Serial.print("MQTT not connected, rc=");
      Serial.println(client.state());
    }
  }
  client.loop();
}
