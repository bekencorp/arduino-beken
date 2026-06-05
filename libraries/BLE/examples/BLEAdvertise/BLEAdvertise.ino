#include <ArduinoBLE.h>

static const uint8_t kManufacturerData[] = {0x34, 0x12, 0xBE, 0x4B, 0x01};
BLEService g_service("180F");
BLEByteCharacteristic g_level("2A19", BLERead | BLENotify);

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("arduino-beken ble advertise");

    if (!BLE.begin()) {
        Serial.println("BLE.begin() failed");
        return;
    }

    BLE.setDeviceName("arduino-beken");
    BLE.setLocalName("arduino-beken");
    BLE.setManufacturerData(kManufacturerData, sizeof(kManufacturerData));
    g_service.addCharacteristic(g_level);
    BLE.addService(g_service);
    g_level.writeValue(static_cast<uint8_t>(42));

    if (BLE.advertise()) {
        Serial.println("BLE advertising started");
    } else {
        Serial.println("BLE advertising failed");
    }
}

void loop() {
    static uint8_t level = 42;
    g_level.writeValue(level++);
    Serial.print("Advertising, battery level=");
    Serial.println(level);
    delay(2000);
}
