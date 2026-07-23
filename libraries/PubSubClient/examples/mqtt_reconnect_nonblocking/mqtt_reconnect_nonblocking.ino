/*
  Reconnecting MQTT example - non-blocking

  This sketch demonstrates how to keep the client connected
  using a non-blocking reconnect function. If the client loses
  its connection, it attempts to reconnect every 5 seconds
  without blocking the main loop.

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

// Change this to your MQTT broker hostname or IP
const char* mqtt_server = "broker.mqtt-dashboard.com";
const uint16_t mqtt_port = 1883;

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

unsigned long lastReconnectAttempt = 0;
bool wasConnected = false;

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

boolean reconnect() {
  Serial.print("MQTT reconnect attempt... broker=");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(mqtt_port);

  if (client.connect("arduinoClient")) {
    Serial.println("MQTT connected");
    // Once connected, publish an announcement...
    if (client.publish("outTopic", "hello world")) {
      Serial.println("Published announcement to outTopic");
    } else {
      Serial.println("Publish announcement failed");
    }
    // ... and resubscribe
    if (client.subscribe("inTopic")) {
      Serial.println("Subscribed to inTopic");
    } else {
      Serial.println("Subscribe to inTopic failed");
    }
  } else {
    Serial.print("MQTT connect failed, rc=");
    Serial.println(client.state());
  }
  return client.connected();
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("mqtt_reconnect_nonblocking starting");

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  lastReconnectAttempt = 0;
  wasConnected = false;
  Serial.println("Entering loop: non-blocking reconnect every 5s when disconnected");
}

void loop() {
  if (!client.connected()) {
    if (wasConnected) {
      Serial.print("MQTT disconnected, rc=");
      Serial.println(client.state());
      wasConnected = false;
    }

    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      // Attempt to reconnect
      if (reconnect()) {
        lastReconnectAttempt = 0;
        wasConnected = true;
      } else {
        Serial.println("Will retry MQTT reconnect in 5 seconds");
      }
    }
  } else {
    // Client connected
    wasConnected = true;
    client.loop();
  }
}
