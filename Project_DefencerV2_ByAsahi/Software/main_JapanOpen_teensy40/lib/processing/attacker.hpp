#ifndef _ATTACKER_HPP
#define _ATTACKER_HPP

#include <Arduino.h>

#include "serial_receive.hpp"

#include "vector.hpp"
#include "timer.hpp"
#include "motor.hpp"

#include "dribbler.hpp"
#include "kicker.hpp"

void attacker_init()
{

}

void attacker_main()
{
    pid_motion.pid_gyro();

    move(0, 0);
}

#endif