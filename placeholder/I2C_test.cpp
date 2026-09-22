// I2C Scanner
// Scans all I2C addresses and reports which devices respond.
// Expect to see:
//   0x68  -> MPU6050
//   0x3C  -> OLED (once wired/tested)
#include <Arduino.h>
#include <Wire.h>

void setup() {
  Wire.begin();
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("\nI2C Scanner starting...");
}

void loop() {
  byte error, address;
  int devicesFound = 0;

  Serial.println("Scanning...");

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println("  !");
      devicesFound++;
    } else if (error == 4) {
      Serial.print("Unknown error at address 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
    }
  }

  if (devicesFound == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.println("Scan complete\n");
  }

  delay(3000); // wait 3 seconds before next scan
}
