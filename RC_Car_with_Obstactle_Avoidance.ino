#include <IBusBM.h>

IBusBM IBus;

const int ENA = 9;
const int IN1 = 3;
const int IN2 = 4;

const int ENB = 10;
const int IN3 = 5;
const int IN4 = 6;

const int TRIG_PIN = 11;
const int ECHO_PIN = 2;

const int DEADZONE = 40;
const int MIN_PWM = 100;
const int OBSTACLE_DISTANCE = 25;

int channelToSpeed(int value) {
  value = constrain(value, 1000, 2000);

  if (abs(value - 1500) < DEADZONE) {
    return 0;
  }

  return map(value, 1000, 2000, -255, 255);
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);

  if (duration == 0) {
    return 999;
  }

  return duration * 0.0343 / 2;
}

void setMotor(int speedValue, int enablePin, int pinA, int pinB) {
  speedValue = constrain(speedValue, -255, 255);

  if (speedValue > 0) {
    digitalWrite(pinA, HIGH);
    digitalWrite(pinB, LOW);
  }
  else if (speedValue < 0) {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, HIGH);
  }
  else {
    digitalWrite(pinA, LOW);
    digitalWrite(pinB, LOW);
    analogWrite(enablePin, 0);
    return;
  }

  int pwm = map(abs(speedValue), 1, 255, MIN_PWM, 255);
  analogWrite(enablePin, pwm);
}

void stopMotors() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

void setup() {
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  stopMotors();

  IBus.begin(Serial);
}

void loop() {
  int steeringRaw = IBus.readChannel(0);
  int throttleRaw = IBus.readChannel(2);

  if (steeringRaw < 900 || steeringRaw > 2100 ||
      throttleRaw < 900 || throttleRaw > 2100) {
    stopMotors();
    delay(200);
    return;
  }

  int steering = -channelToSpeed(steeringRaw);
  int throttle = channelToSpeed(throttleRaw);

  float distance = getDistance();

  if (distance <= OBSTACLE_DISTANCE && throttle > 0) {
    throttle = 0;
  }

  int leftSpeed = throttle + steering;
  int rightSpeed = throttle - steering;

  int maxSpeed = max(abs(leftSpeed), abs(rightSpeed));

  if (maxSpeed > 255) {
    leftSpeed = (long)leftSpeed * 255 / maxSpeed;
    rightSpeed = (long)rightSpeed * 255 / maxSpeed;
  }

  leftSpeed = constrain(leftSpeed, -255, 255);
  rightSpeed = constrain(rightSpeed, -255, 255);

  setMotor(rightSpeed, ENA, IN1, IN2);
  setMotor(leftSpeed, ENB, IN3, IN4);

  delay(100);
}