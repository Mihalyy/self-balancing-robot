// DRV8833 motor test
//
// Steps both motors through a safe sequence: slow forward, stop, slow
// reverse, stop, then a ramp to find the PWM at which each motor starts
// turning at all. That threshold matters later - below it the gearbox
// friction wins and your balance loop has a dead zone around zero.
//
// Wiring (Nano -> DRV8833):
//   D10 -> IN1   D9 -> IN2    (motor A, OUT1/OUT2)
//   D6  -> IN3   D5 -> IN4    (motor B, OUT3/OUT4)
//
// DRV8833 drive logic, per channel:
//   IN1=PWM, IN2=0   forward, slow decay off-phase
//   IN1=0,   IN2=PWM reverse
//   IN1=0,   IN2=0   coast
//   IN1=1,   IN2=1   brake
//
// PUT THE ROBOT ON A STAND before running this. Both wheels must spin
// freely, or it will drive itself off the bench.

#include <Arduino.h>

const byte A_IN1 = 10;
const byte A_IN2 = 9;
const byte B_IN3 = 6;
const byte B_IN4 = 5;

const int TEST_PWM = 80;      // out of 255 - gentle
const unsigned long STEP_MS = 1500;

void motorA(int speed) {       // -255..255
  if (speed >= 0) {
    analogWrite(A_IN1, speed);
    analogWrite(A_IN2, 0);
  } else {
    analogWrite(A_IN1, 0);
    analogWrite(A_IN2, -speed);
  }
}

void motorB(int speed) {
  if (speed >= 0) {
    analogWrite(B_IN3, speed);
    analogWrite(B_IN4, 0);
  } else {
    analogWrite(B_IN3, 0);
    analogWrite(B_IN4, -speed);
  }
}

void stopBoth() {
  motorA(0);
  motorB(0);
}

void announce(const char *what) {
  Serial.print("--> ");
  Serial.println(what);
}

// Ramp one motor up from zero and report where it starts moving.
// You have to watch the wheel and note the number yourself - there is
// no encoder to tell us.
void rampTest(void (*motor)(int), const char *name) {
  Serial.print("Ramping ");
  Serial.print(name);
  Serial.println(" from 0. Watch the wheel and note when it starts turning.");

  for (int pwm = 0; pwm <= 160; pwm += 5) {
    motor(pwm);
    Serial.print("  PWM ");
    Serial.println(pwm);
    delay(400);
  }

  motor(0);
  Serial.println("  ramp done, stopped.");
  Serial.println();
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(A_IN1, OUTPUT);
  pinMode(A_IN2, OUTPUT);
  pinMode(B_IN3, OUTPUT);
  pinMode(B_IN4, OUTPUT);
  stopBoth();

  Serial.println();
  Serial.println("=== DRV8833 motor test ===");
  Serial.println("Robot on a stand, wheels free. Starting in 3 seconds.");
  Serial.println();
  delay(3000);
}

void loop() {
  announce("Motor A forward");
  motorA(TEST_PWM);
  delay(STEP_MS);
  stopBoth();
  delay(800);

  announce("Motor A reverse");
  motorA(-TEST_PWM);
  delay(STEP_MS);
  stopBoth();
  delay(800);

  announce("Motor B forward");
  motorB(TEST_PWM);
  delay(STEP_MS);
  stopBoth();
  delay(800);

  announce("Motor B reverse");
  motorB(-TEST_PWM);
  delay(STEP_MS);
  stopBoth();
  delay(800);

  announce("Both forward");
  motorA(TEST_PWM);
  motorB(TEST_PWM);
  delay(STEP_MS);
  stopBoth();
  delay(800);

  announce("Both reverse");
  motorA(-TEST_PWM);
  motorB(-TEST_PWM);
  delay(STEP_MS);
  stopBoth();
  delay(1500);

  Serial.println();
  announce("Finding the minimum PWM that turns each motor");
  Serial.println();
  rampTest(motorA, "motor A");
  rampTest(motorB, "motor B");

  Serial.println("=== cycle complete, repeating in 5 seconds ===");
  Serial.println();
  delay(5000);
}
