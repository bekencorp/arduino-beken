static const unsigned long kBaudRate = 115200;

void setup() {
    Serial.begin(kBaudRate);
    Serial.println();
    Serial.println("arduino-beken serial echo");
    Serial.println("Type any character to echo it back.");
}

void loop() {
    if (Serial.available() > 0) {
        int value = Serial.read();
        Serial.print("RX: 0x");
        if (value < 16) {
            Serial.print("0");
        }
        Serial.print(value, HEX);
        Serial.print(" '");
        if (value >= 32 && value <= 126) {
            Serial.write((uint8_t)value);
        } else {
            Serial.print(".");
        }
        Serial.println("'");
    }
}
