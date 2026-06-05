/*
  BLEWriteTest - verify App -> device GATT write

  Service UUID:  fff0
  Byte char:     fff1  (Read + Write, 1 byte)
  Text char:     fff2  (Read + Write, up to 20 bytes)

  Test with nRF Connect / LightBlue:
  1. Connect to "ble-write-test"
  2. Open service fff0
  3. Write to fff1 (e.g. 0x55) or fff2 (e.g. "hello")
  4. Tap Read on the same characteristic - value should match
  5. Serial Monitor (115200) prints when the device receives a write
*/

#include <ArduinoBLE.h>

BLEService g_service("fff0");

// App writes a single byte here (0x00 - 0xFF)
BLEByteCharacteristic g_cmd("fff1", BLERead | BLEWrite);

// App writes a short text here
BLEStringCharacteristic g_text("fff2", BLERead | BLEWrite, 20);

static uint8_t s_last_cmd = 0xFF;
static String s_last_text;

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("BLEWriteTest starting...");

  g_service.addCharacteristic(g_cmd);
  g_service.addCharacteristic(g_text);
  BLE.addService(g_service);

  BLE.setDeviceName("ble-write-test");
  BLE.setLocalName("ble-write-test");
  BLE.setAdvertisedService(g_service);

  g_cmd.writeValue(static_cast<uint8_t>(0));
  g_text.writeValue("");

  if (!BLE.begin()) {
    Serial.println("BLE.begin() failed");
    return;
  }

  if (!BLE.advertise()) {
    Serial.println("BLE.advertise() failed");
    return;
  }

  Serial.println("Advertising as \"ble-write-test\"");
  Serial.println("Service fff0:");
  Serial.println("  fff1 = byte (Write then Read to verify)");
  Serial.println("  fff2 = text (Write then Read to verify)");
  Serial.println();
}

void loop() {
  const uint8_t cmd = g_cmd.value();
  if (cmd != s_last_cmd) {
    s_last_cmd = cmd;
    Serial.print("[fff1] App wrote byte: 0x");
    if (cmd < 0x10) {
      Serial.print('0');
    }
    Serial.print(cmd, HEX);
    Serial.print(" (");
    Serial.print(cmd);
    Serial.println(')');
  }

  const String text = g_text.value();
  if (text != s_last_text) {
    s_last_text = text;
    Serial.print("[fff2] App wrote text: \"");
    Serial.print(text);
    Serial.println('"');
  }

  if (BLE.connected()) {
    static unsigned long last_hint_ms = 0;
    const unsigned long now = millis();
    if (now - last_hint_ms >= 10000UL) {
      last_hint_ms = now;
      Serial.println("(connected - use Write then Read on fff1/fff2)");
    }
  }

  delay(50);
}
