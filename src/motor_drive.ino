// Motor drive with deadband compensation
//
// This is the drive layer the balance loop will sit on top of. Two things
// it does that the raw analogWrite calls did not:
//
//   1. Slow decay (drive/brake) rather than fast decay (drive/coast).
//      Measured on this robot: breakaway dropped from 110 to 65 on the
//      left motor and from 125 to 95 on the right.
//
//   2. Deadband feedforward. Each motor has its own minimum PWM, so a
//      command of "1% forward" is mapped to just above that motor's
//      breakaway rather than to PWM 3, which does nothing at all. Without
//      this, a PID's small corrections vanish and the robot only reacts
//      once it is already falling hard.
//
// Commands are -1.0 to +1.0. Positive drives the robot forward.
//
// Wiring (Nano -> DRV8833):
//   D10 -> IN1   D9 -> IN2    (motor A = left,  OUT1/OUT2)
//   D6  -> IN3   D5 -> IN4    (motor B = right, OUT3/OUT4)
//
// ROBOT ON A STAND.

#include <Arduino.h>

const byte A_IN1 = 10;
const byte A_IN2 = 9;
const byte B_IN3 = 6;
const byte B_IN4 = 5;

// Measured breakaway PWM in slow decay, 2026-10-13.
// Re-measure after a few hours of running - these drop as the gearboxes
// wear in. Set them a little BELOW the measured value rather than above:
// too high and the robot twitches at the slightest tilt.
const int A_MIN_PWM = 60;
const int B_MIN_PWM = 90;

const int MAX_PWM = 255;

// Commands smaller than this are treated as zero, so the motors are not
// held energised while the robot is balanced and happy.
const float DEADZONE = 0.02;

void driveRaw(byte pinFwd, byte pinRev, int pwm, bool forward) {
  byte hi = forward ? pinFwd : pinRev;
  byte lo = forward ? pinRev : pinFwd;
  analogWrite(hi, 255);
  analogWrite(lo, 255 - pwm);
}

void coast(byte p1, byte p2) {
  analogWrite(p1, 0);
  analogWrite(p2, 0);
}

// cmd: -1.0 .. +1.0
void setMotor(byte pin1, byte pin2, float cmd, int minPwm) {
  if (cmd > 1.0) cmd = 1.0;
  if (cmd < -1.0) cmd = -1.0;

  if (cmd > -DEADZONE && cmd < DEADZONE) {
    coast(pin1, pin2);
    return;
  }

  bool forward = (cmd > 0);
  float mag = forward ? cmd : -cmd;

  // Map 0..1 onto minPwm..MAX_PWM instead of 0..MAX_PWM
  int pwm = minPwm + (int)((MAX_PWM - minPwm) * mag + 0.5);
  driveRaw(pin1, pin2, pwm, forward);
}

void setMotorA(float cmd) { setMotor(A_IN1, A_IN2, cmd, A_MIN_PWM); }
void setMotorB(float cmd) { setMotor(B_IN3, B_IN4, cmd, B_MIN_PWM); }

void setBoth(float cmd) {
  setMotorA(cmd);
  setMotorB(cmd);
}

void stopBoth() {
  coast(A_IN1, A_IN2);
  coast(B_IN3, B_IN4);
}

void hold(const char *label, float cmd, unsigned long ms) {
  Serial.print("--> ");
  Serial.print(label);
  Serial.print("   cmd = ");
  Serial.println(cmd, 2);
  setBoth(cmd);
  delay(ms);
  stopBoth();
  delay(600);
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
  Serial.println("=== Motor drive, deadband compensated ===");
  Serial.println("Both wheels should start moving at the SAME command,");
  Serial.println("even though their raw thresholds differ by 30 counts.");
  Serial.println();
  delay(3000);
}

void loop() {
  hold("creep forward",  0.03, 2000);
  hold("slow forward",   0.15, 2000);
  hold("medium forward", 0.40, 1500);
  hold("full forward",   1.00, 1500);

  Serial.println();

  hold("creep reverse",  -0.03, 2000);
  hold("slow reverse",   -0.15, 2000);
  hold("medium reverse", -0.40, 1500);
  hold("full reverse",   -1.00, 1500);

  Serial.println();
  Serial.println("=== cycle complete, repeating in 6 seconds ===");
  Serial.println();
  delay(6000);
}
