// DRV8833 motor test, version 2
//
// Changes from v1:
//   - Direction check now runs at PWM 160. The first version used 80,
//     which was below both motors' breakaway thresholds, so nothing moved.
//   - Each motor is ramped twice: once in fast decay (drive/coast, what
//     v1 used) and once in slow decay (drive/brake). Compare the two
//     breakaway numbers - slow decay usually wins by a wide margin.
//
// Decay modes on the DRV8833, per channel:
//   fast decay   IN1 = PWM(d),   IN2 = 0        drive <-> coast
//   slow decay   IN1 = 255,      IN2 = PWM(255-d)   drive <-> brake
//
// Both produce duty d in the forward direction. They differ in what the
// winding does between pulses, and that is what sets the dead zone.
//
// Wiring (Nano -> DRV8833):
//   D10 -> IN1   D9 -> IN2    (motor A = left,  OUT1/OUT2)
//   D6  -> IN3   D5 -> IN4    (motor B = right, OUT3/OUT4)
//
// PUT THE ROBOT ON A STAND. Both wheels must spin freely.

#include <Arduino.h>

const byte A_IN1 = 10;
const byte A_IN2 = 9;
const byte B_IN3 = 6;
const byte B_IN4 = 5;

const int TEST_PWM = 160;
const unsigned long STEP_MS = 1500;

bool slowDecay = true;

// speed is -255..255. Sign is direction, magnitude is duty.
void drive(byte pinA, byte pinB, int speed) {
  byte fwd = (speed >= 0) ? pinA : pinB;
  byte rev = (speed >= 0) ? pinB : pinA;
  int duty = (speed >= 0) ? speed : -speed;

  if (slowDecay) {
    analogWrite(fwd, 255);
    analogWrite(rev, 255 - duty);
  } else {
    analogWrite(fwd, duty);
    analogWrite(rev, 0);
  }
}

void motorA(int speed) { drive(A_IN1, A_IN2, speed); }
void motorB(int speed) { drive(B_IN3, B_IN4, speed); }

void coastBoth() {
  analogWrite(A_IN1, 0);
  analogWrite(A_IN2, 0);
  analogWrite(B_IN3, 0);
  analogWrite(B_IN4, 0);
}

void announce(const char *what) {
  Serial.print("--> ");
  Serial.println(what);
}

void rampTest(void (*motor)(int), const char *name, bool useSlow) {
  slowDecay = useSlow;

  Serial.print("Ramping ");
  Serial.print(name);
  Serial.print("  [");
  Serial.print(useSlow ? "SLOW decay" : "fast decay");
  Serial.println("]  - note the PWM where the wheel first turns.");

  for (int pwm = 0; pwm <= 200; pwm += 5) {
    motor(pwm);
    Serial.print("  PWM ");
    Serial.println(pwm);
    delay(400);
  }

  coastBoth();
  Serial.println("  ramp done.");
  Serial.println();
  delay(1500);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(A_IN1, OUTPUT);
  pinMode(A_IN2, OUTPUT);
  pinMode(B_IN3, OUTPUT);
  pinMode(B_IN4, OUTPUT);
  coastBoth();

  Serial.println();
  Serial.println("=== DRV8833 motor test v2 ===");
  Serial.println("Robot on a stand, wheels free. Starting in 3 seconds.");
  Serial.println();
  delay(3000);
}

void loop() {
  slowDecay = true;   // direction check uses the stronger mode

  announce("Motor A (left) forward");
  motorA(TEST_PWM);  delay(STEP_MS);  coastBoth();  delay(800);

  announce("Motor A (left) reverse");
  motorA(-TEST_PWM); delay(STEP_MS);  coastBoth();  delay(800);

  announce("Motor B (right) forward");
  motorB(TEST_PWM);  delay(STEP_MS);  coastBoth();  delay(800);

  announce("Motor B (right) reverse");
  motorB(-TEST_PWM); delay(STEP_MS);  coastBoth();  delay(800);

  announce("BOTH forward - does the robot try to drive, or spin in place?");
  motorA(TEST_PWM);  motorB(TEST_PWM);
  delay(STEP_MS);    coastBoth();     delay(800);

  announce("BOTH reverse");
  motorA(-TEST_PWM); motorB(-TEST_PWM);
  delay(STEP_MS);    coastBoth();     delay(2000);

  Serial.println();
  announce("Breakaway thresholds - fast decay vs slow decay");
  Serial.println();

  rampTest(motorA, "motor A (left)",  false);
  rampTest(motorA, "motor A (left)",  true);
  rampTest(motorB, "motor B (right)", false);
  rampTest(motorB, "motor B (right)", true);

  Serial.println("=== cycle complete, repeating in 8 seconds ===");
  Serial.println();
  delay(8000);
}
