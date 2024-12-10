#ifndef _DRIBBLER_H
#define _DRIBBLER_H

#include <Servo.h>

Servo Dribbler;

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