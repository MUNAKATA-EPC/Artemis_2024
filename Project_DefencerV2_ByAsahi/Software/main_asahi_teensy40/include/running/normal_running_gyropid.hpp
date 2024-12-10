#ifndef _NORMAL_RUNNING_GYROPID_H
#define _NORMAL_RUNNING_GYROPID_H

#include <Arduino.h>

#include "common/config.hpp"
#include "common/timer.hpp"
#include "common/motor.hpp"
#include "common/vector.hpp"
#include "common/serial_receive.hpp"

#include "common/processing/engelline.hpp"
#include "common/processing/ball_tracking.hpp"

class Normal_Running_GyroPID
{
private:
    Timer LimitOfSideMove;
    Timer LimitOfBackMove;

    float Line_SpeedPer = 1.0;
    float AfterLine_SpeedPer = 1.00;
    
    const int ROBO_SPEED = 80;

    bool CheckDegToBeforeLine(int deg)
    {
        int check_deg = 45;
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

    int speed_process(int speed, int deg)
    {
        int move_speed = speed;
        //ライン脱出後一定時間は速度を落とす。
        if(engelline.AfterLine_Wait.get_value() <= 400 || !engelline.AfterLine_Wait.isticking())
        {
            move_speed = ROBO_SPEED * AfterLine_SpeedPer;
        }

        //ある角度に一定時間移動していた時は速度を落とす。
        if(abs(sin(radians(deg))) >= abs(sin(radians(50))))
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
                    move_speed = move_speed * 0.8;
                }
            }
        }
        else
        {
            LimitOfSideMove.stop();
            LimitOfSideMove.reset();
        }

        //ある角度に一定時間移動していた時は速度を落とす。
        if(cos(radians(deg)) <= cos(radians(140)))
        {
            LimitOfBackMove.start();
            if(LimitOfBackMove.get_value() >=500)
            {
                if(LimitOfBackMove.get_value() >= 1300)
                {
                    LimitOfBackMove.stop();
                    LimitOfBackMove.reset();
                }
                else
                {
                    if(Ball_Distance <= 90)
                    {
                        move_speed = move_speed * 0.8;
                    }
                }
            }
        }
        else
        {
            LimitOfBackMove.stop();
            LimitOfBackMove.reset();
        }

        return move_speed;
    }

public:
    int get_robo_speed()
    {
        return ROBO_SPEED;
    }

    void normal_running()
    {  
        int camera_dis;
        int camera_dir;

        int move_deg = 0;
        int move_speed = 0;

        kicker.tick();

        LimitOfSideMove.tick();
        LimitOfBackMove.tick();

        engelline.engelline_main(ROBO_SPEED * Line_SpeedPer);
        ball_tracking_gyro.ball_tracking_main(ROBO_SPEED, 255);

        pid_motion.pid_gyro();
        
        //TODO: ボールの回り込みの最適化
        if(engelline.IsLineMovement())
        {
            LimitOfSideMove.stop();
            LimitOfSideMove.reset();
            LimitOfBackMove.stop();
            LimitOfBackMove.reset();
            
            move_deg = engelline.get_degrees();
            move_speed = engelline.get_speed();
        }
        else
        {
            if(ball_tracking_gyro.IsAbleToTracking())
            {
                move_deg = ball_tracking_gyro.get_degrees(); 
                move_speed = speed_process(ball_tracking_gyro.get_speed(), move_deg);
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
Normal_Running_GyroPID normal_running_gyropid;

#endif