/*
  Long message MQTT example

  This sketch demonstrates sending arbitrarily large messages with PubSubClient.

  It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "greenBottles/#", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary
  - If the sub-topic is a number, it publishes a "greenBottles/lyrics" message
    with a payload consisting of the lyrics to "10 green bottles", replacing
    10 with the number given in the sub-topic.

  Single-board self-test: after MQTT connect, the sketch itself publishes lyrics
  for DEMO_BOTTLE_COUNT bottles (no external MQTT client required).

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

// Self-test bottle count after connect (keep small, e.g. 2~5)
#ifndef DEMO_BOTTLE_COUNT
#define DEMO_BOTTLE_COUNT 3
#endif

WiFiClient wifiClient;
PubSubClient client(wifiClient);
bool selfTestDone = false;

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

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void publishLyrics(int bottleCount) {
  if (bottleCount <= 0) {
    return;
  }

  // Work out how big our resulting message will be
  int msgLen = 0;
  for (int i = bottleCount; i > 0; i--) {
    String numBottles(i);
    msgLen += 2 * numBottles.length();
    if (i == 1) {
      msgLen += 2 * String(" green bottle, standing on the wall\n").length();
    } else {
      msgLen += 2 * String(" green bottles, standing on the wall\n").length();
    }
    msgLen += String("And if one green bottle should accidentally fall\nThere'll be ").length();
    switch (i) {
      case 1:
        msgLen += String("no green bottles, standing on the wall\n\n").length();
        break;
      case 2:
        msgLen += String("1 green bottle, standing on the wall\n\n").length();
        break;
      default:
        numBottles = String(i - 1);
        msgLen += numBottles.length();
        msgLen += String(" green bottles, standing on the wall\n\n").length();
        break;
    }
  }

  Serial.print("beginPublish greenBottles/lyrics, expectedLen=");
  Serial.println(msgLen);

  // Now we can start to publish the message
  if (!client.beginPublish("greenBottles/lyrics", msgLen, false)) {
    Serial.println("beginPublish failed");
    return;
  }

  for (int i = bottleCount; i > 0; i--) {
    for (int j = 0; j < 2; j++) {
      client.print(i);
      if (i == 1) {
        client.print(" green bottle, standing on the wall\n");
      } else {
        client.print(" green bottles, standing on the wall\n");
      }
    }
    client.print("And if one green bottle should accidentally fall\nThere'll be ");
    switch (i) {
      case 1:
        client.print("no green bottles, standing on the wall\n\n");
        break;
      case 2:
        client.print("1 green bottle, standing on the wall\n\n");
        break;
      default:
        client.print(i - 1);
        client.print(" green bottles, standing on the wall\n\n");
        break;
    }
  }

  // Now we're done!
  if (client.endPublish()) {
    Serial.println("endPublish OK (large message sent)");
  } else {
    Serial.println("endPublish failed");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] len=");
  Serial.print(length);
  Serial.print(" ");
  // Print a short preview so large lyrics do not flood the serial forever
  const unsigned int preview = length < 80 ? length : 80;
  for (unsigned int i = 0; i < preview; i++) {
    Serial.print((char)payload[i]);
  }
  if (length > preview) {
    Serial.print("...");
  }
  Serial.println();

  // Find out how many bottles we should generate lyrics for
  String topicStr(topic);
  int bottleCount = 0;  // assume no bottles unless we correctly parse a value from the topic
  if (topicStr.indexOf('/') >= 0) {
    // The topic includes a '/', we'll try to read the number of bottles from just after that
    topicStr.remove(0, topicStr.indexOf('/') + 1);
    // Now see if there's a number of bottles after the '/'
    bottleCount = topicStr.toInt();
  }

  if (bottleCount > 0) {
    publishLyrics(bottleCount);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    char clientId[24];
    snprintf(clientId, sizeof(clientId), "BK7239Client-%04lx", (unsigned long)random(0xffff));
    // Attempt to connect
    if (client.connect(clientId)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
      // ... and resubscribe
      if (client.subscribe("greenBottles/#")) {
        Serial.println("Subscribed to greenBottles/#");
      }
      selfTestDone = false;  // allow one self-test after each (re)connect
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println();
  Serial.println("mqtt_large_message starting");
  Serial.println("Single-board mode: will self-publish lyrics after MQTT connect");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // No external MQTT client needed: trigger large publish once after connect
  if (client.connected() && !selfTestDone) {
    selfTestDone = true;
    Serial.print("Self-test: publishing lyrics for ");
    Serial.print(DEMO_BOTTLE_COUNT);
    Serial.println(" bottles");
    publishLyrics(DEMO_BOTTLE_COUNT);
  }
}
