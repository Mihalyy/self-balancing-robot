// I2C bus diagnostic
//
// 1. Reads SDA (A4) and SCL (A5) as plain inputs BEFORE Wire touches them,
//    so you can see whether the bus is free at boot.
// 2. Then scans, printing each address BEFORE trying it, so a hang tells
//    you exactly which address it died on.
//
// Wire.endTransmission() has no timeout on AVR - if the bus is stuck it
// blocks forever. The Serial.flush() before each attempt guarantees the
// address reaches the terminal even if the next line never returns.

#include <Arduino.h>
#include <Wire.h>

void reportBus(const char *label) {
  Serial.print(label);
  Serial.print("  SDA(A4) = ");
  Serial.print(digitalRead(A4) ? "HIGH" : "LOW ");
  Serial.print("   SCL(A5) = ");
  Serial.println(digitalRead(A5) ? "HIGH" : "LOW");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("=== I2C bus diagnostic ===");
  Serial.println();

  // Read the lines with nothing driving them at all
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  delay(10);
  reportBus("No pull-up: ");

  // Now with the AVR's internal pull-ups (~30k) enabled
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  delay(10);
  reportBus("Pull-ups on:");

  Serial.println();
  Serial.println("Both HIGH on the second line = bus is free.");
  Serial.println("Either LOW = that line is being held down by something.");
  Serial.println();
  Serial.println("Starting scan. Watch which address it stops at.");
  Serial.println();
  Serial.flush();

  Wire.begin();
}

void loop() {
  byte found = 0;

  for (byte address = 1; address < 127; address++) {
    Serial.print("0x");
    if (address < 16) Serial.print("0");
    Serial.print(address, HEX);
    Serial.print(" ");
    Serial.flush();              // get it out before we risk blocking

    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.println("  <-- DEVICE FOUND");
      found++;
    } else if (address % 16 == 0) {
      Serial.println();          // wrap the line every 16 addresses
    }
  }

  Serial.println();
  Serial.print("Scan complete. Devices found: ");
  Serial.println(found);
  Serial.println();
  delay(3000);
}
