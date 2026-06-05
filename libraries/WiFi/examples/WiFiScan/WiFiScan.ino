#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("arduino-beken wifi scan");
}

void loop() {
    int networks = WiFi.scanNetworks();
    if (networks < 0) {
        Serial.println("WiFi scan failed");
    } else if (networks == 0) {
        Serial.println("No WiFi networks found");
    } else {
        Serial.print("Found ");
        Serial.print(networks);
        Serial.println(" network(s):");

        for (int i = 0; i < networks; ++i) {
            Serial.print("  ");
            Serial.print(i + 1);
            Serial.print(". ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" RSSI=");
            Serial.print(WiFi.RSSI(i));
            Serial.print(" ENC=");
            Serial.println(WiFi.encryptionType(i));
        }
    }

    Serial.println();
    delay(10000);
}
