#ifndef _NORMAL_RUNNING_GOALPID_H
#define _NORMAL_RUNNING_GOALPID_H

#include <Arduino.h>

#include "common/timer.hpp"
#include "common/motor.hpp"
#include "common/serial_receive.hpp"

int now_mode;

class Normal_Running_GoalPID
{
private:
    Timer LimitOfSideMove;
    Timer Ball_timer;

    int ROBO_SPEED = 58;
    int mode;


    float Line_SpeedPer = 0.8;
    float AfterLine_SpeedPer = 0.85;

    int Camera_Data[4];

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
            Camera_Data[0] = Yellow_Goal_Deg - 2;
            Camera_Data[1] = Yellow_Goal_Distance;
            Camera_Data[2] = Blue_Goal_Deg - 2;
            Camera_Data[3] = Blue_Goal_Distance;
            mode = 1;
        }
        else if(type == 1)
        {
            Camera_Data[0] = Blue_Goal_Deg - 2;
            Camera_Data[1] = Blue_Goal_Distance;
            Camera_Data[2] = Yellow_Goal_Deg - 2;
            Camera_Data[3] = Yellow_Goal_Distance;
            mode = 2;
        }

        pid_motion.pid_gyro();
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

        engelline.engelline_main(ROBO_SPEED - 10);
        ball_tracking_gyro.ball_tracking_main(ROBO_SPEED - 7);

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
            if(Ball_Distance >= 40)//ボールを追いかけたいボールの距離
            {
                if(ball_tracking_camera.IsAbleToTracking())
                {  
                    move_deg = ball_tracking_gyro.get_degrees();
                    move_speed = ball_tracking_gyro.get_speed();

                    if(engelline.AfterLine_Wait.get_value() <= 400 || !engelline.AfterLine_Wait.isticking())
                    {
                        move_speed = ROBO_SPEED * AfterLine_SpeedPer;
                    }

                    if(abs(sin(radians(move_deg))) >= abs(sin(radians(45))))
                    {
                        LimitOfSideMove.start();
                        if(LimitOfSideMove.get_value() >= 500)
                        {
                            if(LimitOfSideMove.get_value() >= 2500)
                            {
                                LimitOfSideMove.stop();
                                LimitOfSideMove.reset();
                            }
                            else
                            {
                                if(Ball_Distance <= 85)
                                {
                                    move_speed = move_speed * 0.85;
                                }
                            }
                        }
                    }
                    else
                    {
                        LimitOfSideMove.stop();
                        LimitOfSideMove.reset();
                    }
                }
            }
            else
            {
                if(mode == 1)//黄ゴール守りの時
                {
                    if(Yellow_Goal_Deg == 255)//ゴールが見えているか
                    {
                        move_deg = 180;
                        move_speed = 40;
                    }
                    else if(Ball_Distance > 43)//コートの半分
                    {
                        move_deg = move_deg;
                        move_speed = move_speed;
                    }
                    else
                    {
                        if(Ball_Deg == -1)
                        {
                            move_deg = Yellow_Goal_Deg * 2;
                            move_speed = min(Yellow_Goal_Distance / 2, 40);
                        }
                        else if(Yellow_Goal_Distance < 102)//止まってほしい位置
                        {
                            move_deg = 0;
                            move_speed = 0;
                            
                        }
                        else
                        {
                            move_deg = Yellow_Goal_Deg * 2;
                            move_speed = min(Yellow_Goal_Distance / 2, 40);
                        }
                    }
                }
                else if(mode == 2)//青ゴール守りの時
                {
                    if(Blue_Goal_Deg == 255)//ゴールが見えているか
                    {
                        move_deg = 180;
                        move_speed = 40;
                    }
                    else if(Ball_Distance > 43)//コートの半分
                    {
                        move_deg = move_deg;
                        move_speed = move_speed;
                    }
                    else
                    {
                        if(Ball_Deg == -1)
                        {
                            move_deg = Blue_Goal_Deg * 2;
                            move_speed = min(Blue_Goal_Distance / 2, 40);
                        }
                        else if(Blue_Goal_Deg < 102)//止まってほしい位置
                        {
                            move_deg = 0;
                            move_speed = 0; 
                        }
                        else
                        {
                            move_deg = Blue_Goal_Deg * 2;
                            move_speed = min(Blue_Goal_Distance / 2, 40);
                        }
                    }
                }
            }
        }
        move(move_deg, move_speed);
    }
};
Normal_Running_GoalPID normal_running_goalpid;



#endif