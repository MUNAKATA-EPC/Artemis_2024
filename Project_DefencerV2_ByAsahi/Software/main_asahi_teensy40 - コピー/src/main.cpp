#include <Arduino.h>
#include <Servo.h>
#include <DSR1202.h>

#include "common/serial_receive.hpp"
#include "common/motor.hpp"
#include "common/button.hpp"
#include "common/timer.hpp"
#include "common/vector.hpp"

#include "common/modules/kicker.hpp"
#include "common/modules/dribbler.hpp"

#include "common/ui/ui.hpp"

#include "common/processing/engelline.hpp"
#include "common/processing/ball_tracking.hpp"

#include "running/normal_running_goalpid.hpp"
#include "running/normal_running_gyropid.hpp"

void setup() {
  serials_init();
  motor_init();
  ui_setup();

  engelline.engelline_init();

  Dribbler.attach(4);
  Dribbler.writeMicroseconds(2000);
  delay(500);
  Dribbler.writeMicroseconds(1000);
  delay(500);
}

void loop() {
  serials_receive();

  if(!is_running)
  {
    motor_power(0, 0, 0, 0);
    Dribbler.writeMicroseconds(1000);
  }
  else
  {
    if(running_type == RUNNING_TYPE::NORMAL_YELLOW)
    {
      normal_running_goalpid.normal_running(0);
    }
    else if(running_type == RUNNING_TYPE::NORMAL_BLUE)
    {
      normal_running_goalpid.normal_running(1);
    }
    else if(running_type == RUNNING_TYPE::NORMAL_GYROONLY)
    {    
      normal_running_gyropid.normal_running();
    }
  }

  ui_main();
}