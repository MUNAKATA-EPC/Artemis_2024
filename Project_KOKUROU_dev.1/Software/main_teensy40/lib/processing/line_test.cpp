#include <Arduino.h>
#include "motor.hpp"
#include "timer.hpp"
#include "line.hpp"
#include "sensors_variables.hpp"

void line_test_loop()
{
    pid_gyro();

    if(is_line_process())
    {
        motor_move(orange_ball_deg * 2, 90);
    }
    else
    {
        motor_move(0, 0);
    }
}