#include <Arduino.h>

#include "serial_receive.hpp"

#include "motor.hpp"
#include "button.hpp"
#include "timer.hpp"
#include "vector.hpp"

#include "dribbler.hpp"
#include "kicker.hpp"

#include "attacker.hpp"

Button btEnter(30, BUTTON_PULLDOWN);
Button btLeft(31, BUTTON_PULLDOWN);
Button btRight(32, BUTTON_PULLDOWN);
Button btStart(33, BUTTON_PULLDOWN);

bool is_running;

void setup() {
    motor_init();
    serials_init();

    Serial.begin(9600);
}

void loop() {
  serials_receive();

  btEnter.loop();
  btLeft.loop();
  btRight.loop();
  btStart.loop();

  if(btStart.isPushed())
  {
    is_running = !is_running;
    attacker_init();
  }

  if(is_running)
  {
    attacker_main();
  }
  else
  {
    motor_power(0, 0, 0, 0);
  }
}