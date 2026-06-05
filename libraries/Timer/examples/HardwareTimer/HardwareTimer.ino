#include <Arduino.h>
static HardwareTimer g_timer(4);
static constexpr uint32_t kTimerPeriodMs = 250;

volatile uint32_t g_tick_count = 0;

void onHardwareTimer() {
    ++g_tick_count;
    digitalWrite(LED_BUILTIN, (g_tick_count & 1U) ? HIGH : LOW);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    Serial.begin(115200);
    delay(200);

    Serial.println();
    Serial.println("arduino-beken hardware timer");
    Serial.print("Starting timer ");
    Serial.print(g_timer.id());
    Serial.print(" with period ");
    Serial.print(kTimerPeriodMs);
    Serial.println(" ms.");

    if (!g_timer.begin()) {
        Serial.println("HardwareTimer.begin() failed");
        return;
    }

    g_timer.attachInterrupt(onHardwareTimer);
    if (!g_timer.start(kTimerPeriodMs)) {
        Serial.println("HardwareTimer.start() failed");
        return;
    }
}

void loop() {
    static unsigned long last_report = 0;
    if (millis() - last_report < 1000) {
        delay(10);
        return;
    }

    last_report = millis();

    Serial.print("ticks=");
    Serial.print(g_tick_count);
    Serial.print(" counter=");
    Serial.print(g_timer.read());
    Serial.print(" running=");
    Serial.println(g_timer.running() ? "yes" : "no");
}
