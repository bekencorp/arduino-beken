#include <Wire.h>

static const uint8_t kFirstAddress = 0x08;
static const uint8_t kLastAddress = 0x77;

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("arduino-beken i2c scanner");

    Wire.begin();
}

void loop() {
    uint8_t found = 0;

    Serial.println("Scanning I2C bus...");
    for (uint8_t address = kFirstAddress; address <= kLastAddress; ++address) {
        Wire.beginTransmission(address);
        if (Wire.endTransmission() == 0) {
            Serial.print("Found device at 0x");
            if (address < 16) {
                Serial.print("0");
            }
            Serial.println(address, HEX);
            ++found;
        }
        delay(2);
    }

    if (found == 0) {
        Serial.println("No I2C devices found");
    }
    Serial.println();
    delay(3000);
}
