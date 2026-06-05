#include <WiFi.h>

static const char *kSsid = "YOUR_SSID";
static const char *kPassword = "YOUR_PASSWORD";

void setup() {
    Serial.begin(115200);
    Serial.println();
    Serial.println("arduino-beken wifi connect");

    int status = WiFi.begin(kSsid, kPassword);
    if (status != WL_CONNECTED) {
        Serial.print("WiFi.begin() failed, status=");
        Serial.println((int)status);
        return;
    }

    Serial.println("WiFi connected");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("RSSI: ");
    Serial.println(WiFi.RSSI());
}

void loop() {
    int status = WiFi.status();
    Serial.print("Link status: ");
    Serial.println((int)status);
    delay(5000);
}
