#include <Arduino.h>
#include <Wire.h>

int16_t accX, accY, accZ;
int16_t gyroX, gyroY, gyroZ;
int16_t rawTemp;

void setup() {

  Wire.begin();
  Serial.begin(115200);

  // Wake up MPU6050
  Wire.beginTransmission(0x68);  // MPU6050 I2C address
  Wire.write(0x6B);              // PWR_MGMT_1 register
  Wire.write(0);                 // Wake up
  Wire.endTransmission(true);
}

void loop() {

  // Start reading from ACCEL_XOUT_H (0x3B)
  Wire.beginTransmission(0x68);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  Wire.requestFrom(0x68, 14, true);

  accX = Wire.read() << 8 | Wire.read();
  accY = Wire.read() << 8 | Wire.read();
  accZ = Wire.read() << 8 | Wire.read();

  rawTemp = Wire.read() << 8 | Wire.read();

  gyroX = Wire.read() << 8 | Wire.read();
  gyroY = Wire.read() << 8 | Wire.read();
  gyroZ = Wire.read() << 8 | Wire.read();

  float temperature = (rawTemp / 340.0) + 36.53;

  Serial.print("Acc: ");
  Serial.print(accX);
  Serial.print(", ");
  Serial.print(accY);
  Serial.print(", ");
  Serial.print(accZ);

  Serial.print(" | Gyro: ");
  Serial.print(gyroX);
  Serial.print(", ");
  Serial.print(gyroY);
  Serial.print(", ");
  Serial.print(gyroZ);

  Serial.print(" | Temp: ");
  Serial.print(temperature);
  Serial.println(" C");

  delay(200);
}