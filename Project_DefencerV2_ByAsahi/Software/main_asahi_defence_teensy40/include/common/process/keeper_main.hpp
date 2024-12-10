#ifndef _KEEPER_MAIN_H
#define _KEEPER_MAIN_H

#include "Arduino.h"

#include "common/timer.hpp"
#include "common/motor.hpp"
#include "common/serial_receive.hpp"
#include "common/vector.hpp"

int move_type = 0;
int ingoal_movetype = 0;

void keeper_main()
{
    pid_motion.pid_gyro();

    if(move_type == 0)
    {
        if(Ball_Deg == -1)
        {
            move(0, 0);
        }
        else
        {
            if(Yellow_Goal_Deg >= 100)
            {
                if(Ball_Deg >= 70 && Ball_Deg <= 110)
                {
                    move(0, 0);
                }
                else if(Ball_Deg >= 180 && Ball_Deg <= 110)
                {
                    move(180, 40);
                }
                else
                {
                    move(0, 40);
                }
            }
            else if(Yellow_Goal_Deg <= 65 )
            {
                if(Ball_Deg >= 250 && Ball_Deg <= 290)
                {
                    move(0, 0);
                }
                else if(Ball_Deg >= 180 && Ball_Deg <= 250)
                {
                    move(180, 40);
                }
                else
                {
                    move(0, 40);
                }
            }
            else
            {
                if(Yellow_Goal_Deg <= 72)
                {
                    if(Ball_Deg >= 200 && Ball_Deg <= 260)
                    {
                        move(215, 40);
                    }
                    else if(Ball_Deg >= 20 && Ball_Deg <= 90)
                    {
                        move(45, 40);
                    }
                    else
                    {
                        move(0, 0);
                    }
                }
                else if(Yellow_Goal_Deg >= 94)
                {
                    if(Ball_Deg >= 110 && Ball_Deg <= 160)
                    {
                        move(135, 40);
                    }
                    else if(Ball_Deg >= 270 && Ball_Deg <= 340)
                    {
                        move(315, 40);
                    }
                    else
                    {
                        move(0, 0);
                    }
                }
                else
                {
                    if(Line_Deg == -1)
                    {
                        if(Yellow_Goal_Deg >= 90)
                        {
                            move(180, 30);
                        }
                        else
                        {
                            move(0, 30);
                        }
                    }
                    else
                    {
                        if(Ball_Deg <= 20 || Ball_Deg >= 340 || (Ball_Deg <= 160 && Ball_Deg >= 200))
                        {
                            move(0, 0);
                        }
                        else if(Ball_Deg <= 180)
                        {
                            move(90, 50);
                        }
                        else
                        {
                            move(270, 50);
                        }
                    }
                }
            }
        }

        if((Line_Deg == -1 && !((Yellow_Goal_Deg >= 55 && Yellow_Goal_Deg <= 120) )) || (Yellow_Goal_Distance <= 75 || Yellow_Goal_Distance >= 110))
        {
            move_type = 1;
        }
    }
    else if(move_type == 1)
    {
        if(Yellow_Goal_Deg >= 75 && Yellow_Goal_Deg <= 95)
        {
            if(Yellow_Goal_Distance <= 90)
            {
                move(0, 20 + (90 - Yellow_Goal_Distance) * 3);
            }
            else
            {
                move(Yellow_Goal_Deg * 2, Yellow_Goal_Distance <= 100 ? 40 : 50);
            }
        }
        else
        {
            //ゴール端の処理
            if(Yellow_Goal_Deg <= 90)
            {
                if(Yellow_Goal_Deg <= 47 || ((Line_Deg >= 270 || Line_Deg <= 90) && Line_Deg != -1))
                {
                    move(0, 40);
                }
                else if(Yellow_Goal_Deg >= 65)
                {
                    move(90, 40);
                }
                else
                {
                    move(45, 40);
                }
                //とりあえず左半分
            }
            else
            {
                if(Yellow_Goal_Deg >= 125 || ((Line_Deg >= 270 || Line_Deg <= 90) && Line_Deg != -1))
                {
                    move(0, 40);
                }
                else if(Yellow_Goal_Deg <= 115)
                {
                    move(270, 40);
                }
                else
                {
                    move(315, 40);
                }
            }
        }


        if(Line_Deg != -1 && (Yellow_Goal_Deg >= 70 && Yellow_Goal_Deg <= 105))
        {
            move_type = 0;
        }
    }
}

#endif