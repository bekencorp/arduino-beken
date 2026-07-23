/*
  Example of using a Stream object to store the message payload

  Original Ethernet/SRAM sketch adapted for WiFi on arduino-beken.
  Uses an in-RAM Stream buffer instead of the external SRAM library.

  - connects to an MQTT server
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic"
  - stores inbound payloads into a memory Stream and prints them

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

// Simple RAM-backed Stream used by PubSubClient to store payloads
class MemoryStream : public Stream {
 public:
  explicit MemoryStream(size_t capacity)
      : _capacity(capacity), _length(0), _readPos(0), _writePos(0), _buffer(nullptr) {
    _buffer = (uint8_t*)malloc(capacity);
  }

  ~MemoryStream() {
    free(_buffer);
  }

  bool begin() {
    return _buffer != nullptr;
  }

  void seek(size_t pos) {
    if (pos > _length) {
      pos = _length;
    }
    _readPos = pos;
    _writePos = pos;
  }

  int available() override {
    return (int)(_length > _readPos ? (_length - _readPos) : 0);
  }

  int read() override {
    if (_readPos >= _length || _buffer == nullptr) {
      return -1;
    }
    return _buffer[_readPos++];
  }

  int peek() override {
    if (_readPos >= _length || _buffer == nullptr) {
      return -1;
    }
    return _buffer[_readPos];
  }

  void flush() override {}

  size_t write(uint8_t b) override {
    if (_buffer == nullptr || _writePos >= _capacity) {
      return 0;
    }
    _buffer[_writePos++] = b;
    if (_writePos > _length) {
      _length = _writePos;
    }
    return 1;
  }

  size_t write(const uint8_t* buffer, size_t size) override {
    size_t written = 0;
    while (written < size) {
      if (write(buffer[written]) != 1) {
        break;
      }
      written++;
    }
    return written;
  }

 private:
  size_t _capacity;
  size_t _length;
  size_t _readPos;
  size_t _writePos;
  uint8_t* _buffer;
};

MemoryStream payloadStream(1024);

void callback(char* topic, byte* payload, unsigned int length) {
  (void)payload;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");

  // Payload bytes were also written into payloadStream by PubSubClient
  payloadStream.seek(0);
  while (payloadStream.available() > 0) {
    Serial.write((uint8_t)payloadStream.read());
  }
  Serial.println();

  // Reset position for the next message to be stored
  payloadStream.seek(0);
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

  if (!payloadStream.begin()) {
    Serial.println("Failed to allocate payload stream buffer");
    return;
  }
  payloadStream.seek(0);

  setup_wifi();

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.setStream(payloadStream);

  if (client.connect("arduinoClient")) {
    Serial.println("MQTT connected");
    client.publish("outTopic", "hello world");
    client.subscribe("inTopic");
  } else {
    Serial.print("MQTT connect failed, rc=");
    Serial.println(client.state());
  }
}

void loop() {
  client.loop();
}
