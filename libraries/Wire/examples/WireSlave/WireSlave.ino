#include <Wire.h>

#define SLAVE_ADDRESS 0x48

void receiveEvent(int howMany) {
  Serial.print("Received ");
  Serial.print(howMany);
  Serial.println(" bytes:");
  
  while (Wire.available()) {
    char c = Wire.read();
    Serial.print("  0x");
    Serial.println(c, HEX);
  }
}

void requestEvent() {
  // Send data back to master when requested
  static uint8_t counter = 0;
  const char* msg = "Hello from Slave! ";
  for (int i = 0; msg[i] != '\0'; i++) {
    Wire.write((uint8_t)msg[i]);
  }
  Wire.write(counter++);
}

void setup() {
  Wire.begin(SLAVE_ADDRESS);     // Join I2C bus with specified address
  Wire.onReceive(receiveEvent);  // Register callback for received data
  Wire.onRequest(requestEvent);  // Register callback for data requests
  Serial.begin(115200);
  Serial.println("I2C Slave ready");
}

void loop() {
  delay(100);  // Main loop does nothing, callbacks handle communication
}
