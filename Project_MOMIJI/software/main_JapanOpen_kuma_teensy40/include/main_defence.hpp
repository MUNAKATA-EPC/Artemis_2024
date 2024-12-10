#ifndef _MAIN_DEFENCE_HPP
#define _MAIN_DEFENCE_HPP

#include "serial_receive.hpp"
#include "motor.hpp"
#include "kicker.hpp"
#include "lcd.hpp"
#include "engelline.hpp"
#include "../lib/vector/vector.hpp"
#define MOTOR_SPEED 50

#define goal_right 105
#define goal_left 75
#define goal_right_center 113//右端ラインの真ん中か壁側、動きで変える
#define goal_left_center 60//上とおんなじかんじ
#define goal_near 96//大事細かく
#define goal_very_near 74
#define court_center 115

int mode = 0;
int defence_goal ;
class Defence{
private:
int camera_data[4];
    int before_line;
    bool line_move;  
    int judge_value;
    Timer ball_front;
    int moter_speed =  0;
// 0 = ライントレース　1　= 攻撃
public:


void set_camera(int type)
{
    if(type == 0)
    {
        camera_data[0] = Yellow_Goal_Deg;
        camera_data[1] = Yellow_Goal_Distance;
        camera_data[2] = Blue_Goal_Deg;
        camera_data[3] = Blue_Goal_Distance;
    }
    else
    {
        camera_data[0] = Blue_Goal_Deg;
        camera_data[1] = Blue_Goal_Distance;
        camera_data[2] = Yellow_Goal_Deg;
        camera_data[3] = Yellow_Goal_Distance;
    }
}

void main(){

    int move_deg = 0;
    int move_speed = 0;
    
    set_camera(defence_goal);
    pid_motion.pid_gyro();

        if(camera_data[0] != 255 &&(camera_data[0] < goal_left || camera_data[0] > goal_right))
        {
            move_deg = camera_data[0] * 2;
            move_speed = 50;
        }
        else
        {
            if(goal_near < camera_data[1])//待機場所まで
            {
                move_deg = camera_data[0] * 2;
                move_speed = 50;
            }
            else if(is_line_detected())
                {
                    move_deg = 0;
                    move_speed = 0;
                }
            else if(camera_data[1] < goal_very_near)
            {
                move_deg = 0;
                move_speed = 40;

            }
            else
            {  
                    if(Ball_Deg != -1)
                    {
                        if((Ball_Deg >= 355 || Ball_Deg <= 5) || (Ball_Deg >= 175 && Ball_Deg <= 185))
                        {
                            move_deg = 0;
                            move_speed = 0;      
                        }
                        else if(Ball_Deg < 180)
                        {
                            
                            move_deg = 90;
                            move_speed = 70;  
                        }
                        else
                        {
                            move_deg = 270;
                            move_speed = 70; 
                            
                        }
                        
                    }
        }

        }
        
       move(move_deg, move_speed);
}
};


Defence defence;

#endif
