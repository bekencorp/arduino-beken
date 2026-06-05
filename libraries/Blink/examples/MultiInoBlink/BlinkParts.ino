void printStartupMessage() {
    Serial.println("arduino-beken multi-ino blink start");
}

void blinkLed(unsigned long high_ms, unsigned long low_ms) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(high_ms);
    digitalWrite(LED_BUILTIN, LOW);
    delay(low_ms);
}
