#include <Arduino.h>

// Simple board bring-up test:
// 1. Blinks the onboard LED (pin 13) to confirm the chip is running code
// 2. Echoes back anything typed into Serial Monitor to confirm USB-serial works

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  while (!Serial) { ; }
  Serial.println("FT232 Nano board test starting...");
  Serial.println("Type something and press Enter - it should echo back.");
}

void loop() {
  // Blink LED
  digitalWrite(LED_BUILTIN, HIGH);
  delay(300);
  digitalWrite(LED_BUILTIN, LOW);
  delay(300);

  // Echo any incoming serial data
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    Serial.print("You typed: ");
    Serial.println(input);
  }
}