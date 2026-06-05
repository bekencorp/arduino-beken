#include <Ticker.h>

Ticker ledTicker;
volatile bool led_state = false;

void toggleLed() {
    led_state = !led_state;
    digitalWrite(LED_BUILTIN, led_state ? HIGH : LOW);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(115200);
    Serial.println("arduino-beken ticker blink");

    ledTicker.attach(0.5f, toggleLed);
}

void loop() {
    static unsigned long last_report = 0;
    if (millis() - last_report >= 2000) {
        last_report = millis();
        Serial.print("Ticker active: ");
        Serial.println(ledTicker.active() ? "yes" : "no");
    }
}
