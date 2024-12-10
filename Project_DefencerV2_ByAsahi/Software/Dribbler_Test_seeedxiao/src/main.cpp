#include <Arduino.h>
#include <Servo.h>

Servo esc;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  esc.attach(0);
  esc.writeMicroseconds(2000);
  delay(4000);
  esc.writeMicroseconds(1000);
  delay(4000);
}

void loop() {
  esc.writeMicroseconds(1100);
  delay(10);
}