#ifndef _NORMAL_RUNNING_GOALPID_H
#define _NORMAL_RUNNING_GOALPID_H

#include <Arduino.h>

#include "common/timer.hpp"
#include "common/motor.hpp"
#include "common/serial_receive.hpp"

class Normal_Running_GoalPID
{
private:
    Timer LimitOfSideMove;

    int ROBO_SPEED = 60;

    float Line_SpeedPer = 0.8;
    float AfterLine_SpeedPer = 0.80;

    int Camera_Data[2];

    bool CheckDegToBeforeLine(int deg)
    {
        int check_deg = 60;
        if(engelline.Line_Deg_First + 180 + check_deg >= 360)
        {
            if(deg >= engelline.Line_Deg_First + 180 - check_deg || deg <= engelline.Line_Deg_First + 180 + check_deg - 360)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if(engelline.Line_Deg_First + 180 - check_deg >= 360)
            {
                if(deg >= engelline.Line_Deg_First + 180 - check_deg - 360 && deg <= engelline.Line_Deg_First + 180 + check_deg - 360)
                {
                    return true;
                }
                else
                {
                    if(deg >= engelline.Line_Deg_First + 180 - check_deg && deg <= engelline.Line_Deg_First + 180 + check_deg)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }

        return false;
    }

    void set_camera_data(int type)
    {
        if(type == 0)
        {
            Camera_Data[0] = Yellow_Goal_Deg;
            Camera_Data[1] = Yellow_Goal_Distance;

            pid_motion.pid_gyro();
        }
        else
        {
            Camera_Data[0] = Blue_Goal_Deg;
            Camera_Data[1] = Blue_Goal_Distance;

            pid_motion.pid_gyro();
        }
    }

public:
    int get_robo_speed()
    {
        return ROBO_SPEED;
    }

    void normal_running(int deg_type)
    {  
        int move_deg = 0;
        int move_speed = 0;

        set_camera_data(deg_type);

        engelline.engelline_main(ROBO_SPEED);
        ball_tracking_camera.ball_tracking_main(ROBO_SPEED, Camera_Data);

        //TODO: ボールの回り込みの最適化
        if(engelline.IsLineMovement())
        {
            LimitOfSideMove.stop();
            LimitOfSideMove.reset();

            move_deg = engelline.get_degrees();
            move_speed = engelline.get_speed();
        }
        else
        {
            if(ball_tracking_camera.IsAbleToTracking())
            {  
                move_deg = ball_tracking_camera.get_degrees();
                move_speed = ball_tracking_camera.get_speed();

                if(engelline.AfterLine_Wait.get_value() <= 400 || !engelline.AfterLine_Wait.isticking())
                {
                    move_speed = ROBO_SPEED * AfterLine_SpeedPer;
                }

                //ある角度に一定時間移動していた時は速度を落とす。
                if(abs(sin(radians(move_deg))) <= abs(sin(radians(30))))
                {
                    LimitOfSideMove.start();
                    if(LimitOfSideMove.get_value() >= 300)
                    {
                        if(LimitOfSideMove.get_value() >= 1300)
                        {
                            LimitOfSideMove.stop();
                            LimitOfSideMove.reset();
                        }
                        else
                        {
                            move_speed = move_speed * 0.70;
                        }
                    }
                }
                else
                {
                    LimitOfSideMove.stop();
                    LimitOfSideMove.reset();
                }
            }
            else
            {  
                move_deg = 0;
                move_speed = 0;
            }         
        }
                
        move(move_deg, move_speed);
    }
};
Normal_Running_GoalPID normal_running_goalpid;


#endif