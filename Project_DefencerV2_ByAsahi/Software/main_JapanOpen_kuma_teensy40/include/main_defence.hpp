#ifndef _MAIN_DEFENCE_HPP
#define _MAIN_DEFENCE_HPP

#include "serial_receive.hpp"
#include "motor.hpp"
#include "kicker.hpp"
#include "lcd.hpp"
#include "../lib/vector/vector.hpp"
#define MOTOR_SPEED 50

#define goal_right 130
#define goal_left 50
#define goal_right_center 113//右端ラインの真ん中か壁側、動きで変える
#define goal_left_center 60//上とおんなじかんじ
#define goal_near 96//大事細かく
#define court_center 115

int mode = 0;
int camera_data[4];
int defence_goal = 0;
class Defence{
private:
    int before_line;
    bool line_move;  
    int judge_value;
    Timer ball_front;
    int move_deg = 0;
    int move_speed = 0;
    int moter_speed =  0;
// 0 = ライントレース　1　= 攻撃
public:
int ball_exist(int min,int max)
{
    int  check_deg = 0;
    int temp = (min + max) / 2;
    int temp_ = (temp + 180) % 360;


    if(Ball_Deg != -1)
    {   
        if(Ball_Distance < 120)
        {
            if(Ball_Deg > 340 || Ball_Deg < 20)
            {
                check_deg = 2.5;
            }
            else
            {
                check_deg = 5;
            }
        }
        else if(Ball_Distance < 100)
        {
            if(Ball_Deg > 340 || Ball_Deg < 20)
            {
                check_deg = 2.5;
            }
            else
            {
                check_deg = 10;
            }
        }
        else if(Ball_Distance < 80)
        {
            
            if(Ball_Deg > 340 || Ball_Deg < 20)
            {
                check_deg = 5;
            }
            else
            {
                check_deg = 10;
            }
        }
        else if(Ball_Distance < 60)
        {
            if(camera_data[0] > 90 && camera_data[0] < goal_right_center)
            {
                if(Ball_Deg > 340 || Ball_Deg < 20)
                {
                    check_deg = 30;
                }
                else
                {
                    check_deg = 15;
                }
            }
            else if(camera_data[0] < 90 && camera_data[0] > goal_left_center)
            {
                
                if(Ball_Deg > 340 || Ball_Deg < 20)
                {
                    check_deg = 30;
                }
                else
                {
                    check_deg = 15;
                }
            }
        }
        else
        {
            if(camera_data[0] > 90 && camera_data[0] < goal_right_center)
            {
                if(Ball_Deg > 340 || Ball_Deg < 20)
                {
                    check_deg = 30;
                }
                else
                {
                    check_deg = 10;
                }
            }
            else if(camera_data[0] < 90 && camera_data[0] > goal_left_center)
            {
                
                if(Ball_Deg > 340 || Ball_Deg < 20)
                {
                    check_deg = 30;
                }
                else
                {
                    check_deg = 10;
                }
            }
        }
    }

    if(Ball_Deg != -1)
    {
        if (temp_ - check_deg < 0)
        {
            if(temp_ - check_deg + 360 <= Ball_Deg || temp_ + check_deg >= Ball_Deg)
            {
                return 0;
            }
            else
            {
                if(temp_ + check_deg <= Ball_Deg && Ball_Deg <= temp - check_deg)
                {
                    return 1;
                }
                else if(temp + check_deg <= Ball_Deg && Ball_Deg <= temp_ - check_deg + 360)
                {
                    return 2;
                }
                else
                {
                    return 0;
                }
            }
        }
        else if(temp + check_deg > 360)
        {
            if(temp - check_deg <= Ball_Deg || Ball_Deg <= temp + check_deg - 360)
            {
                return 0;
            }
            else
            {
                if(temp_ + check_deg <= Ball_Deg || Ball_Deg <= temp - check_deg)
                {
                    return 1;
                }
                else if(temp + check_deg - 360 <= Ball_Deg && Ball_Deg <= temp_ - check_deg)
                {
                    return 2;
                }
                else
                {
                    return 0;
                }
            }
        }
        else
        {
            if(temp > temp_)
            {
                if(temp_ + check_deg < Ball_Deg && Ball_Deg < temp - check_deg)
                {
                    return 1;
                }
                else if(temp + check_deg < Ball_Deg || Ball_Deg <  temp_ - check_deg)
                {
                    return 2;
                }
                else
                {
                    return 0;
                }
            }
            else if(temp_ > temp)
            {
                if(temp_ + check_deg < Ball_Deg || Ball_Deg <  temp - check_deg)
                {
                    return 1;
                }
                else if(temp + check_deg < Ball_Deg && Ball_Deg < temp_ - check_deg)
                {
                    return 2;
                }
                else
                {
                    return 0;
                }
            }
        }
    }
    return 0;
}

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

int movespeed(int balldeg, int movedeg)
{
    if(abs(balldeg - movedeg) < 30)
    {
        return  55;
    }
    else if(abs(balldeg - movedeg) < 70)
    {
        return  50;
    }
    else 
    {
        return  40;
    }
}

void main()
{
    set_camera(defence_goal);
    ball_front.tick();

    if(mode == 0)
    {
        pid_motion.pid_gyro();
    }
    else if(mode == 1)
    {
        if(camera_data[3] > 120)
        {
            pid_motion.pid_gyro();
        }
        else 
        {
            pid_motion.pid_gyro();
            //pid_motion.pid_camera(camera_data[2]);
        }
    }


    if(Ball_Distance > 40 && (Ball_Deg > 340 || Ball_Deg < 20))
    {
        Dribbler.writeMicroseconds(1100);
    }
    else
    {
        Dribbler.writeMicroseconds(1000);
    }

    if(camera_data[1] <= goal_near)
    {
        if((camera_data[0] < goal_left_center +  10) && Ball_Deg != -1)
        {
            if(Ball_Deg <= 180)
            {
                Vector move = Vector(radians(45),1);
                Vector move_ = Vector(radians(45),1);
                move.add(move_);
                move_deg = move.get_deg();
                move_speed = 45;
            }
        }
        else if((camera_data[0] > goal_right_center -  10) && Ball_Deg != -1)
        {
            if(Ball_Deg >= 180)
            {
                Vector move = Vector(radians(315),1);
                Vector move_ = Vector(radians(315),1);
                move.add(move_);
                move_deg = move.get_deg();
                move_speed = 45;
            }

        }
        else
        {
            Vector move = Vector(radians(0),1);
            Vector move_ = Vector(radians(0),1);
            move.add(move_);
            move_deg = move.get_deg();
            move_speed = 45;
        }
    }
    else
    {
        if(mode == 0)
        {
            if((line_min == -1 && line_max == -1))
            {
                if(Line_Deg != -1)
                {
                    move_deg = 0;
                    move_speed = 0;
                }
                else
                {
                    move_deg = camera_data[0] * 2;
                    move_speed = camera_data[1] / 2.2;
                }

            }    
            else if(Ball_Deg >= 155 && Ball_Deg <= 205)
            {
                    move_deg = 0;
                    move_speed = 0;
            }
            else
            {
                if(line_min != -1 && line_max != -1)
                {
                    if(Ball_Deg == -1 || (Ball_Deg < 3 && Ball_Distance < 25))
                    {
                        if(camera_data[0] <= goal_left_center)
                        {
                            move_deg = 45;
                            move_speed = 50;
                        }
                        else if((camera_data[0] >= goal_right_center))
                        {
                            move_deg = 315;
                            move_speed = 50;
                            
                        }
                        else
                        {
                            move_deg = 0;
                            move_speed = 0;

                        }
                    }
                    else
                    {
                        judge_value = ball_exist(line_min,line_max);


                        if(judge_value == 0)
                        {
                            if(camera_data[0] > 90 && camera_data[0] < goal_right_center)
                            {
                                if((Ball_Deg > 340 || Ball_Deg < 20) && Ball_Distance > 40)
                                {
                                    ball_front.start();
                                    motor_break();
                                    if(ball_front.get_value() > 1000)
                                    {
                                        mode = 1;
                                    }
                                    else
                                    {
                                        move_deg = 0;
                                        move_speed = 0;
                                    }       
                                }
                                else
                                {
                                    ball_front.reset();
                                    ball_front.stop();

                                    move_deg = 0;
                                    move_speed = 0;

                                }
                            }
                            else if(camera_data[0] < 90 && camera_data[0] > goal_left_center)
                            {
                                if((Ball_Deg > 350 || Ball_Deg < 10) && Ball_Distance > 40)
                                {
                                    ball_front.start();
                                    motor_break();
                                    if(ball_front.get_value() > 1000)
                                    {
                                        mode = 1;
                                    }
                                    else
                                    {
                                        move_deg = 0;
                                        move_speed = 0;
                                    }       
                                }
                                else
                                {
                                    ball_front.reset();
                                    ball_front.stop();

                                    move_deg = 0;
                                    move_speed = 0;

                                }

                            }
                            else if((Ball_Deg > 350 || Ball_Deg < 10) && Ball_Distance > 40)
                            {
                                ball_front.start();
                                motor_break();
                                if(ball_front.get_value() > 1000)
                                {
                                    mode = 1;
                                }
                                else
                                {
                                    move_deg = 0;
                                    move_speed = 0;
                                }      
                            }
                            else
                            {
                                ball_front.reset();
                                ball_front.stop();

                                move_deg = 0;
                                move_speed = 0;

                            }
                        }
                        else if(judge_value == 1)
                        {
                            if(camera_data[0] != 255 && camera_data[0]  > goal_right_center/*右端*/)
                            {
                                if(Ball_Deg != -1 && (Ball_Deg > 45 && Ball_Deg <= 180))
                                {
                                    if((camera_data[0] != 255 && camera_data[0] > goal_right))
                                    {
                                        move_deg = 315;
                                        move_speed = 60; 
                                    }  
                                    else
                                    {
                                        move_deg = 0;
                                        move_speed = 0;
                                    }         
                                }
                                else 
                                {
                                    move_deg = 315;
                                    move_speed = movespeed(Ball_Deg,move_deg);         
                                }               
                            }
                            else if(camera_data[0] < goal_left_center/*左端*/)
                            {
                                if(Ball_Deg != -1 && (Ball_Deg > 330 || Ball_Deg <= 180))
                                {
                                    move_deg = 45;
                                    move_speed = movespeed(Ball_Deg,move_deg);          
                                }
                                else 
                                {
                                    if((camera_data[0] < goal_left || (camera_data[0] != 255 && camera_data[0] > goal_right)))
                                    {
                                        move_deg = 45;
                                        move_speed = 60; 
                                    }  
                                    else
                                    {
                                        move_deg = 0;
                                        move_speed = 0;
                                    }   
                                }               
                            }
                            else
                            {
                                if((Ball_Deg > 350 || Ball_Deg < 10) && Ball_Distance > 40)
                                { 
                                    motor_break();
                                    ball_front.start();
                                    
                                    if(ball_front.get_value() > 1000)
                                    {
                                        mode = 1;
                                    }
                                    else
                                    {
                                        move_deg = 0;
                                        move_speed = 0;
                                    }      
                                }
                                else if(Ball_Deg <= 180)
                                {
                                    ball_front.reset();
                                    ball_front.stop();
                                    
                                    Vector move = Vector(radians(90),1);
                                    Vector move_ = Vector(radians(90),1);
                                    move.add(move_);
                                    move_deg = move.get_deg();
                                    move_speed =  movespeed(Ball_Deg,move_deg);   
                                }
                                else
                                {
                                    ball_front.reset();
                                    ball_front.stop();

                                    Vector move = Vector(radians(270),1);
                                    Vector move_ = Vector(radians(270),1);
                                    move.add(move_);
                                    move_deg = move.get_deg();
                                    move_speed =  movespeed(Ball_Deg,move_deg);;     
                                }
                            }
                        }
                        else if(judge_value == 2)
                        {
                            if(camera_data[0] != 255 && camera_data[0]  > goal_right_center/*右端*/)
                            {
                                if(Ball_Deg != -1 && (Ball_Deg > 20 && Ball_Deg <= 180))
                                {
                                    if((camera_data[0] < goal_left || (camera_data[0] != 255 && camera_data[0] > goal_right)))
                                    {
                                        move_deg = 315;
                                        move_speed = movespeed(Ball_Deg,move_deg); 
                                    }  
                                    else
                                    {
                                        move_deg = 0;
                                        move_speed = 0;
                                    }          
                                }
                                else 
                                {
                                    move_deg = 315;
                                    move_speed = movespeed(Ball_Deg,move_deg);         
                                }               
                            }
                            else if(camera_data[0] < goal_left_center/*左端*/)
                            {
                                if(Ball_Deg != -1 && (Ball_Deg > 330 || Ball_Deg <= 180))
                                {
                                    move_deg = 45;
                                    move_speed = movespeed(Ball_Deg,move_deg);         
                                }
                                else 
                                {
                                    if(camera_data[0] < goal_left || (camera_data[0] != 255 && camera_data[0] > goal_right))
                                    {
                                        move_deg = 45;
                                        move_speed =movespeed(Ball_Deg,move_deg); 
                                    }  
                                    else
                                    {
                                        move_deg = 0;
                                        move_speed = 0;
                                    }           
                                }                    
                            }
                            else
                            {
                                if((Ball_Deg > 350 || Ball_Deg < 10) && Ball_Distance > 40)
                                {
                                    motor_break();
                                    ball_front.start();
                                    
                                    if(ball_front.get_value() > 1000)
                                    {
                                        mode = 1;
                                    }
                                    else
                                    {
                                        move_deg = 0;
                                        move_speed = 0;
                                    }      
                                }
                                else if(Ball_Deg <= 180)
                                {
                                    ball_front.reset();
                                    ball_front.stop();
                                    
                                    Vector move = Vector(radians(90),1);
                                    Vector move_ = Vector(radians(90),1);
                                    move.add(move_);
                                    move_deg = move.get_deg();
                                    move_speed =  movespeed(Ball_Deg,move_deg);    
                                }
                                else
                                {
                                    ball_front.reset();
                                    ball_front.stop();
                                    
                                    Vector move = Vector(radians(270),1);
                                    Vector move_ = Vector(radians(270),1);
                                    move.add(move_);
                                    move_deg = move.get_deg();
                                    move_speed =  movespeed(Ball_Deg,move_deg);;
                                }
                            }
                        }
                        else
                        {
                            move_deg = 0;
                            move_speed = 0;
                        }
                    }
                }        
            }
        }
        else if(mode == 1)
        {
            if(Line_Deg != -1)
            {
                move_deg = 0;
                move_speed = 50;
            }
            else
            {
                if(ball_front.get_value() < 4000 )
                {
                    if(Line_Deg != -1 && !line_move)
                    {
                        line_move = true;
                        before_line = Line_Deg;
                    }

                    if(line_move == true)
                    {
                        if(Line_Deg != -1)
                        {
                            move_deg = Court_Deg * 2;
                            move_speed = 80;
                    
                        }
                        else
                        {
                            line_move = false;
                            move_deg = 0;
                            move_speed = 0;
                        }
                    }
                    else
                    {
                        if(Ball_Deg != -1)
                        {
                            if(Ball_Deg > 350 || Ball_Deg < 10)
                            {
                                
                                //Dribbler.writeMicroseconds(1080);
                                move_deg = 0;
                                move_speed = 60;

                                if(HoldSensor == 1)
                                {
                                    kicker.kick(100);
                                }
                            }
                            else 
                            {
                                if(Ball_Deg <= 40)
                                {
                                    Vector ball = Vector(radians(Ball_Deg), 1);
                                    Vector ball_ = Vector(radians(Ball_Deg + 30), 1);
                                    ball.add(ball_);
                                    move_deg = ball.get_deg();
                                    move_speed = movespeed(Ball_Deg,move_deg);

                                }
                                else if(Ball_Deg <= 180)
                                {
                                    Vector ball = Vector(radians(Ball_Deg), 1);
                                    Vector ball_ = Vector(radians(Ball_Deg + 100), 1);
                                    ball.add(ball_);
                                    move_deg = ball.get_deg();
                                    move_speed = movespeed(Ball_Deg,move_deg);

                                }
                                else if(Ball_Deg <= 320)
                                {
                                    Vector ball = Vector(radians(Ball_Deg), 1);
                                    Vector ball_ = Vector(radians(Ball_Deg - 100), 1);
                                    ball.add(ball_);
                                    move_deg = ball.get_deg();
                                    move_speed = movespeed(Ball_Deg,move_deg);

                                }
                                else
                                {
                                    Vector ball = Vector(radians(Ball_Deg), 1);
                                    Vector ball_ = Vector(radians(Ball_Deg - 30), 1);
                                    ball.add(ball_);
                                    move_deg = ball.get_deg();
                                    move_speed = movespeed(Ball_Deg,move_deg);

                                }      
                            }
                        }
                        else
                        {
                            move_deg = 0;
                            move_speed = 0;
                        }
                    }
                }
                else
                {
                    ball_front.reset();
                    ball_front.stop();
                    mode = 0;
                }

            }

        }
    }

    move(move_deg, move_speed);
}

};


Defence defence;

#endif
