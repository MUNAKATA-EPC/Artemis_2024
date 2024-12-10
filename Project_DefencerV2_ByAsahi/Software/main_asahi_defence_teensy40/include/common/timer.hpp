#ifndef _TIMER_H
#define _TIMER_H

#include <Arduino.h>

class Timer{
private:
    int _start_val;
    int value;
    bool ticking;
public:
    void tick(){
        if(ticking)
            value = millis() - _start_val;
    }
    void start(){
        if(ticking == false)
        {
            _start_val = millis();
            ticking = true;
        }
    }
    void stop(){
        ticking = false;
    }
    
    void reset(){
        ticking = false;
        value = 0;
    }
    int get_value(){
        return value;
    }
    void set_value(int val)
    {
        this->value = val;
    }
    bool isticking(){
        return ticking;
    }
};

#endif