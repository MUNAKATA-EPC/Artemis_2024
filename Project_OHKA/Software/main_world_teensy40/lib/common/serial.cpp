#include <Arduino.h>
#include "serial.hpp"
#include "sensors_variables.hpp"

// ball sensor
int ball_deg;
int ball_distance;

// line sensor
int line_value;

// [0] ~ [15]   : engel line
// [16]         : left
// [17]         : right
// [18]         : back
int line_data[19];

// camera
int cam_yellow_deg;
int cam_yellow_dis;
int cam_blue_deg;
int cam_blue_dis;

int gyro_deg;

int hold_value;

// 0 = ボールが近い
// 1 = ボールが遠い
// 2 = ボールを保持している
// 3 = 電源が切れている

int ally_mode = 1;

String twelite_data;

Timer ally_lost_timer;

void serials_init()
{
    //Ball Sensors
    Serial2.begin(115200);

    //Communication Module
    Serial3.begin(115200);

    //Twelite
    Serial4.begin(115200);
    Serial4.setTimeout(10);

    //OpenMV
    Serial5.begin(115200);

    //Line Sensors
    Serial6.begin(115200);
    Serial6.setTimeout(10);

    //Null
    Serial7.begin(115200);
}

void serials_process()
{
    ally_lost_timer.tick();

    if(Serial2.available() > 0)
    {
        ball_deg        = Serial2.readStringUntil('a').toInt();
        ball_distance   = Serial2.readStringUntil('b').toInt();
    }

    if(Serial4.available() > 0)
    {
        String data = Serial4.readStringUntil('\n');

        if(data == "a")
        {
            ally_lost_timer.reset();
            ally_lost_timer.stop();

            ally_mode = 0;
        }
        else if(data == "b")
        {
            ally_lost_timer.reset();
            ally_lost_timer.stop();

            ally_mode = 1;
        }
        else if(data == "c")
        {
            ally_lost_timer.reset();
            ally_lost_timer.stop();

            ally_mode = 2;
        }
        else
        {
            ally_lost_timer.start();

            if(ally_lost_timer.get_value() >= 1000)
            {
                //ally_mode = 3;
            }
        }
    }
    else
    {
        ally_lost_timer.start();

        if(ally_lost_timer.get_value() >= 1000)
        {
            //ally_mode = 3;
        }
    }

    if(Serial5.available() > 0)
    {
        cam_yellow_deg   = Serial5.readStringUntil('a').toInt();
        cam_yellow_dis   = Serial5.readStringUntil('b').toInt();
        cam_blue_deg     = Serial5.readStringUntil('c').toInt();
        cam_blue_dis     = Serial5.readStringUntil('d').toInt();

        cam_yellow_deg = cam_yellow_deg == 255 ? -1 : 360 - cam_yellow_deg * 2;
        cam_blue_deg = cam_blue_deg == 255 ? -1 : 360 - cam_blue_deg * 2;
    }

    while(Serial6.available() > 0)
    {
        int line_value = Serial6.readStringUntil('\n').toInt();
    
        for(int i = 18; i >= 0; i--)
        {
            if(line_value - pow(2, i) >= 0)
            {
                line_value -= pow(2, i);
                line_data[i] = 1;
            }
            else
            {
                line_data[i] = 0;
            }
        }
    }

    hold_value = digitalRead(27);
}