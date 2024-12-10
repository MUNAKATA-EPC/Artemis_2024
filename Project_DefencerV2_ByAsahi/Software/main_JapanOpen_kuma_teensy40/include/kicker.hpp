#ifndef _KICKER_HPP
#define _KICKER_HPP

#include <Arduino.h>
#include <timer.hpp>

class Kicker{
private:
    int _charge_pin;
    int _kick_pin;

    int start_time = 0;
    int delay_time = 0;
    bool iskicking;
public:
    Timer Kick_Timer;

    Kicker(int charge, int kick)
    {
        _charge_pin = charge;
        _kick_pin = kick;
    }

    void tick()
    {
        Kick_Timer.tick();

        if(!iskicking)
        {
            analogWrite(_kick_pin, 0);
            analogWrite(_charge_pin, 1023);
        }
        else
        {
            if(Kick_Timer.get_value() <= start_time)
            {
                analogWrite(_charge_pin, 1023);
                analogWrite(_kick_pin, 0);
            }
            else if(Kick_Timer.get_value() <= start_time + 20)
            {
                analogWrite(_charge_pin, 0);
                analogWrite(_kick_pin, 0);
            }
            else if(Kick_Timer.get_value() <= start_time + 50)
            {
                analogWrite(_charge_pin, 0);
                analogWrite(_kick_pin, 1023);
            }
            else if(Kick_Timer.get_value() <= start_time + 70)
            {
                analogWrite(_charge_pin, 0);
                analogWrite(_kick_pin, 0);
            }
            else if(Kick_Timer.get_value() <= start_time + 50 + delay_time)
            {
                analogWrite(_charge_pin, 1023);
                analogWrite(_kick_pin, 0);
            }
            else
            {
                Kick_Timer.reset();
                start_time = 0;
                iskicking = false;
            }
        }
    }

    void kick(int starttime = 0, int delaytime = 0)
    {
        if(iskicking == false)
        {
            Kick_Timer.reset();
            Kick_Timer.start();
            start_time = starttime;
            delay_time = delaytime + 100;
            iskicking = true;
        }
    }

    int get_timer()
    {
        return Kick_Timer.get_value();
    }

    bool get_kicking()
    {
        return iskicking;
    }
};

Kicker kicker = Kicker(2, 3);

#endif