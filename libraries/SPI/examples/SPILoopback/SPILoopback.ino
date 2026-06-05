#include <SPI.h>

static const uint8_t kPattern[] = {0x55, 0xAA, 0x0F, 0xF0};

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("arduino-beken spi loopback");
    Serial.println("Connect MOSI to MISO to observe mirrored bytes.");

    SPI.begin();
}

void loop() {
    SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    Serial.print("TX/RX:");
    for (size_t i = 0; i < sizeof(kPattern); ++i) {
        uint8_t rx = SPI.transfer(kPattern[i]);
        Serial.print(" ");
        if (kPattern[i] < 16) {
            Serial.print("0");
        }
        Serial.print(kPattern[i], HEX);
        Serial.print("/");
        if (rx < 16) {
            Serial.print("0");
        }
        Serial.print(rx, HEX);
    }
    SPI.endTransaction();

    Serial.println();
    delay(1000);
}
