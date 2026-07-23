/*
  Publishing in the callback

  - connects to an MQTT server
  - subscribes to the topic "inTopic"
  - when a message is received, republishes it to "outTopic"

  This example shows how to publish messages within the
  callback function. The callback function header needs to
  be declared before the PubSubClient constructor and the
  actual callback defined afterwards.
  This ensures the client reference in the callback function
  is valid.

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

// Callback function header
void callback(char* topic, byte* payload, unsigned int length);

WiFiClient wifiClient;
PubSubClient client(wifiClient);

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] len=");
  Serial.print(length);
  Serial.print(" payload=");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // In order to republish this payload, a copy must be made
  // as the original payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.

  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length);
  if (p == NULL) {
    Serial.println("malloc failed, skip republish");
    return;
  }
  // Copy the payload to the new buffer
  memcpy(p, payload, length);
  if (client.publish("outTopic", p, length)) {
    Serial.println("Republished payload to outTopic");
  } else {
    Serial.println("Republish to outTopic failed");
  }
  // Free the memory
  free(p);
}

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
  Serial.println("mqtt_publish_in_callback starting");
  Serial.println("Test: publish any payload to inTopic; board should echo it to outTopic");

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  if (client.connect("arduinoClient")) {
    Serial.println("MQTT connected");
    if (client.publish("outTopic", "hello world")) {
      Serial.println("Published announcement to outTopic");
    }
    if (client.subscribe("inTopic")) {
      Serial.println("Subscribed to inTopic");
    } else {
      Serial.println("Subscribe to inTopic failed");
    }
  } else {
    Serial.print("MQTT connect failed, rc=");
    Serial.println(client.state());
  }
}

void loop() {
  // Keep MQTT session alive and dispatch callbacks
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
