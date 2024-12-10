#include <Arduino.h>
#include "line.hpp"

#include "sensors_variables.hpp"
#include "serial_receive.hpp"
#include "vector.hpp"
#include "timer.hpp"

Timer not_line_time;

void line_init()
{
}

void line_loop()
{
    not_line_time.tick();
    
    if(line_value >= 1000)
    {
        tone(6, 4000, 100);
    }
}

bool is_line_process()
{
    return line_value >= 1000;
}

int get_line_timer()
{
    return 0;
}