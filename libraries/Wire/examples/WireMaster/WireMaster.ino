#include <Wire.h>

void setup() {
  Wire.begin();        // Initialize I2C as master
  Serial.begin(115200);
}

void loop() {
  // Write to slave device
  Wire.beginTransmission(0x48);  // Slave address (7-bit)
  Wire.write(0x00);              // Register address to write
  Wire.write(0x55);              // Data to write
  Wire.endTransmission();        // End transmission
  
  // Read from slave device
  Wire.beginTransmission(0x48);  // Slave address
  Wire.endTransmission(false);   // Send restart condition
  
  Wire.requestFrom(0x48, 2);     // Request 2 bytes from slave
  if (Wire.available() >= 2) {
    int high = Wire.read();
    int low = Wire.read();
    int value = (high << 8) | low;
    Serial.print("Value: ");
    Serial.println(value);
  }
  
  delay(1000);
}