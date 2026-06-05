void printStartupMessage();
void blinkLed(unsigned long high_ms, unsigned long low_ms);

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    printStartupMessage();
}

void loop() {
    blinkLed(200, 800);
}
