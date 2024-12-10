#include <Arduino.h>

#include "common/motor.hpp"
#include "common/serial_receive.hpp"

#include "common/ui/ui.hpp"

#include "common/process/keeper_main.hpp"

void setup() {
  ui_setup();
  motor_init();
  serials_init();

  Serial.begin(9600);
}

void loop() {
  serials_receive();

  if(is_running)
  {
    keeper_main();
  }
  else
  {
    motor_power(0, 0, 0, 0);
  }

  ui_main();
}