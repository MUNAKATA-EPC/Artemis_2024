#include <Arduino.h>
#include "engelline.hpp"

#include "sensors_variables.hpp"

double line_sensors_deg[16] = {22.5 * 0,   22.5 * 1,   22.5 * 2,   22.5 * 3,
                            22.5 * 4,   22.5 * 5,   22.5 * 6,   22.5 * 7,
                            22.5 * 8,   22.5 * 9,   22.5 * 10,  22.5 * 11,
                            22.5 * 12,  22.5 * 13,  22.5 * 14,  22.5 * 15,
                            };

int line_deg;
int line_first_deg;
int previous_line_deg;

int line_raw_deg;

bool is_halfout;

bool is_detected_before;

extern bool is_able_to_run;

void reset_line_process()
{
    is_detected_before = false;
    is_halfout = false;
    line_raw_deg = -1;
    previous_line_deg = -1;
    line_first_deg = -1;
    line_deg = -1;
}

bool is_line_movement()
{
    return is_line_detected() || (is_halfout && !is_line_detected());
}

bool is_line_detected()
{
    bool ret = false;

    for(int i = 0; i < 19; i++)
    {
        if(line_data[i] == 1)
        {
            ret = true;
        }
    }

    return ret;
}

int get_count_of_detcted_sensors()
{
    int ret = 0;

    for(int i = 0; i < 16; i++)
    {
        if(line_data[i] == 1)
        {
            ret++;
        }
    }

    return ret;
}

bool is_engelline_detected()
{
    bool ret = false;

    for(int i = 0; i < 16; i++)
    {
        if(line_data[i] == 1)
        {
            ret = true;
        }
    }

    return ret;
}

int get_line_deg()
{
    return line_deg;
}

int get_raw_line_deg()
{
    return line_raw_deg;
}

int get_prev_line_deg()
{
    return line_first_deg;
}

bool is_line_halfout()
{
    return is_halfout;
}

void exchange_value(int check_value)
{
    if(line_first_deg <= check_value)
    {
        if(line_deg >= line_first_deg + check_value && line_deg <= line_first_deg - check_value + 360)
        {
        }
        else
        {
            line_first_deg = line_deg;
            is_halfout = false;
        }
    }
    else if(line_first_deg >= 360 - check_value)
    {
        if(line_deg >= line_first_deg + check_value - 360 && line_deg <= line_first_deg - check_value)
        {
        }
        else
        {
            line_first_deg = line_deg;
            is_halfout = false;
        }
    }
    else
    {
        if(line_deg >= line_first_deg + check_value || line_deg <= line_first_deg - check_value)
        {
        }
        else
        {
            line_first_deg = line_deg;
            is_halfout = false;
        }
    }
}

void line_process()
{
    for(int i = 0; i < 19; i++)
    {
        Serial.print(line_data[i]);
        Serial.print(",");
    }
    Serial.println();

    if(is_line_detected() && is_able_to_run)
    {
        tone(4, 4000, 100);
    }

    if(get_count_of_detcted_sensors() == 1)
        return;
    
    double line_data_x = 0;
    double line_data_y = 0;

    if(is_engelline_detected())
    {
        for(int i = 0; i < 16; i++)
        {
            if(line_data[i] == 1)
            {
                line_data_x += cos(radians(line_sensors_deg[i]));
                line_data_y += sin(radians(line_sensors_deg[i]));
            }
        }

        line_deg = 360 - degrees(atan2(line_data_y, line_data_x));

        if(line_deg <= 360)
        {
            line_deg = 360 - line_deg;
        }
        else
        {
            line_deg = 360 - (line_deg - 180) + 180;
        }
    }
    else
    {
        line_deg = -1;
    }

    line_raw_deg = line_deg;

    if(is_detected_before == false && is_engelline_detected())
    {
        if(!is_halfout)
        {
            line_first_deg = line_deg;
            previous_line_deg = line_deg;
        }
    }

    if(is_detected_before == true && !is_engelline_detected())
    {
        if(!is_halfout)
        {
            line_first_deg = -1;
        }
        else
        {
            if(is_line_detected())
            {
                line_deg = (line_first_deg + 180) % 360;
            }
            else
            {
                line_deg = (line_first_deg + 180) % 360;
            }
        }
    }

    if(is_halfout && !is_engelline_detected())
    {
        line_deg = line_first_deg;
    }

    int check_value = 90;
    int check_value_exchange = 1100;

    if(is_line_detected())
    {
        if(is_engelline_detected())
        {
            if(line_first_deg <= check_value)
            {
                if(line_deg >= line_first_deg + check_value && line_deg <= line_first_deg - check_value + 360)
                {
                    is_halfout = true;
                }
                else
                {
                    exchange_value(check_value_exchange);
                }
            }
            else if(line_first_deg >= 360 - check_value)
            {
                if(line_deg >= line_first_deg + check_value - 360 && line_deg <= line_first_deg - check_value)
                {
                    is_halfout = true;
                }
                else
                {
                    exchange_value(check_value_exchange);
                }
            }
            else
            {
                if(line_deg >= line_first_deg + check_value || line_deg <= line_first_deg - check_value)
                {
                    is_halfout = true;
                }
                else
                {
                    exchange_value(check_value_exchange);
                }
            }
    
            if(line_deg != -1)
                previous_line_deg = line_deg;
            else
                line_deg = line_first_deg;
        }
        else if(line_data[16] == 1)
        {
            if(line_data[17] == 1)
            {
                line_deg = 225;
            }
            else if(line_data[18] == 1)
            {
                line_deg = line_first_deg;
            }
            else
            {
                line_deg = 270;
            }
        }
        else if(line_data[17] == 1)
        {
            if(line_data[18] == 1)
            {
                line_deg = 135;
            }
            else
            {
                line_deg = 180;
            }
        }
        else if(line_data[18] == 1)
        {
            line_deg = 90;
        }
    }

    is_detected_before = is_engelline_detected();
}