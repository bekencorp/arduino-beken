void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("arduino-beken blink start");
}

void loop() {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
}
