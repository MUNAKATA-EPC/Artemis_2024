#include <Arduino.h>
#include <Servo.h>

Servo dribbler;

void setup() {
  dribbler.attach(1);

  delay(8000);
  dribbler.writeMicroseconds(2000);
  delay(5000);
  dribbler.writeMicroseconds(1000);
  delay(10000);
}

void loop() {
  dribbler.writeMicroseconds(1050);
}