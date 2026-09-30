#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- OLED setup ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// ---------- MPU6050 setup ----------
const int MPU_ADDR = 0x68;
int16_t accX, accY, accZ;
int16_t gyroX, gyroY, gyroZ;
int16_t rawTemp;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Wake up MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Start OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED init failed!");
    while (true) { ; } // halt if OLED not found
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("MPU6050 + OLED ready");
  display.display();
  delay(1000);
}

void readMPU() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  accX = Wire.read() << 8 | Wire.read();
  accY = Wire.read() << 8 | Wire.read();
  accZ = Wire.read() << 8 | Wire.read();
  rawTemp = Wire.read() << 8 | Wire.read();
  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();
}

void loop() {
  readMPU();

  // Convert raw values to real-world units
  // Default full-scale ranges: Accel ±2g, Gyro ±250 deg/s
  float aX = accX / 16384.0;   // g
  float aY = accY / 16384.0;
  float aZ = accZ / 16384.0;
  float gX = gyroX / 131.0;    // deg/s
  float gY = gyroY / 131.0;
  float gZ = gyroZ / 131.0;
  float temperature = (rawTemp / 340.0) + 36.53;

  // Print to Serial for debugging
  Serial.print("Acc(g): "); Serial.print(aX); Serial.print(", ");
  Serial.print(aY); Serial.print(", "); Serial.print(aZ);
  Serial.print(" | Gyro(dps): "); Serial.print(gX); Serial.print(", ");
  Serial.print(gY); Serial.print(", "); Serial.print(gZ);
  Serial.print(" | Temp: "); Serial.println(temperature);

  // Display on OLED
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Ax:"); display.print(aX, 2);
  display.print(" Ay:"); display.println(aY, 2);

  display.print("Az:"); display.print(aZ, 2);
  display.print(" T:"); display.println(temperature, 1);

  display.print("Gx:"); display.print(gX, 0);
  display.print(" Gy:"); display.println(gY, 0);

  display.print("Gz:"); display.println(gZ, 0);

  display.display();

  delay(200);
}
