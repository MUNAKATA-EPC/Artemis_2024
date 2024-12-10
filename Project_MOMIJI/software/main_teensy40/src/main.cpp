#include <Arduino.h>

#include "bno055.hpp"

void setup() {
  Serial.begin(9600);

  bno_init();
}

void loop() {
  bno_process();

  Serial.println(bno_getdeg());
}