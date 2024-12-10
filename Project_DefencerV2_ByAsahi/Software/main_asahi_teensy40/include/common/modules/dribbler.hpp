#ifndef _DRIBBLER_H
#define _DRIBBLER_H

#include <Servo.h>

Servo Dribbler;

void dribbler_calibration()
{
    delay(5000);
    Dribbler.writeMicroseconds(2000);
    delay(5000);
    Dribbler.writeMicroseconds(1500);
    delay(5000);
    Dribbler.writeMicroseconds(1000);
    delay(5000);
}

void dribbler_setup()
{
  Dribbler.writeMicroseconds(2000);
  delay(500);
  Dribbler.writeMicroseconds(1000);
  delay(500);
}

class dribbler{
private:
    Servo esc;
    dribbler(int pin)
    {
        esc.attach(pin);
    }

    void init()
    {
        esc.writeMicroseconds(2000);
    }

    void roll(int speed)
    {
        esc.writeMicroseconds(speed);
    }
};

#endif