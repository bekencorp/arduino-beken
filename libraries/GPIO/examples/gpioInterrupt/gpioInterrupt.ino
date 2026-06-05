#include <Arduino.h>

const int LED_PIN = LED_BUILTIN;
const int INTERRUPT_PIN = 9;

volatile int interruptCount = 0;
volatile bool ledState = false;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void handler() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime < debounceDelay) {
    return;
  }
  lastDebounceTime = currentTime;
  
  interruptCount++;
  ledState = !ledState;
  digitalWrite(LED_PIN, ledState);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  
  delay(100);
  attachInterrupt(INTERRUPT_PIN, handler, CHANGE);
  
  Serial.println("Interrupt test started (with debounce)");
}

void loop() {
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 1000) {
    lastPrint = millis();
    Serial.print("Interrupt count: ");
    Serial.print(interruptCount);
    Serial.print(", LED state: ");
    Serial.println(ledState ? "ON" : "OFF");
  }
}