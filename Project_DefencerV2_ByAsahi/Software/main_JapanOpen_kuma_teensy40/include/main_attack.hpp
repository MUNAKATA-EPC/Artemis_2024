#ifndef _MAIN_ATTACK_HPP
#define _MAIN_ATTACK_HPP

#include "serial_receive.hpp"
#include "motor.hpp"
#include "kicker.hpp"


int attack_goal = 0;
class Attacker{
private:
    int before_line;
    bool line_move; 
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

void main()
{
    int move_deg = 0;
    int move_speed = 0;
    kicker.tick();
    
    set_camera(attack_goal);
    if(camera_data[1] >  110)
    {
        pid_motion.pid_gyro();
    }
    else
    {
        pid_motion.pid_gyro();
       // pid_motion.pid_camera(camera_data[0]);
    }

    if(Line_Deg != -1 && HoldSensor == 1)  
    {
        kicker.kick(100);
    }

    if(Line_Deg != -1 && !line_move)
    {
        line_move = true;
        before_line = Line_Deg;
    }

    
    if(line_move == true)
    {
        if(Line_right != -1)
        {
            move_deg = 90;
            move_speed = 60;
        }
        else if(Line_left != -1)
        {
            move_deg = 270;
            move_speed = 60;

        }
        else if(Line_back != -1)
        {
            move_deg = 0;
            move_speed = 60;

        }
        else if(Line_Deg != -1)
        {
            move_deg = 360 - ((before_line + 180) % 360);
            move_speed = 60;
     
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
            if(Ball_Deg >= 350|| Ball_Deg <=10)
            {
                //Dribbler.writeMicroseconds(1080);
                move_deg = 0;
                move_speed = 50;

                if(HoldSensor == 1)
                {
                    kicker.kick(100);
                }
            }
            else if (Ball_Distance < 35) 
            {
                int theta = 70 + (70 - Ball_Distance) * 60 / 110;
                move_deg = Ball_Deg + (Ball_Deg < 180 ? theta : -theta);
                move_speed = 60;
            }
            else
            {
                    if(Ball_Deg <= 40)
                    {
                        Vector ball = Vector(radians(Ball_Deg), 1);
                        Vector ball_ = Vector(radians(Ball_Deg + 30), 1);
                        ball.add(ball_);
                        move_deg = ball.get_deg();
                        move_speed = Ball_Distance > 90 ? 35 : 40;

                    }
                    else if(Ball_Deg <= 180)
                    {
                        Vector ball = Vector(radians(Ball_Deg), 1);
                        Vector ball_ = Vector(radians(Ball_Deg), 1);
                        ball.add(ball_);
                        move_deg = ball.get_deg();
                        move_speed = Ball_Distance > 90 ? 35 : 40;

                    }
                    else if(Ball_Deg <= 320)
                    {
                        Vector ball = Vector(radians(Ball_Deg), 1);
                        Vector ball_ = Vector(radians(Ball_Deg - 100), 1);
                        ball.add(ball_);
                        move_deg = ball.get_deg();
                        move_speed = Ball_Distance > 90 ? 35 : 40;

                    }
                    else
                    {
                        Vector ball = Vector(radians(Ball_Deg), 1);
                        Vector ball_ = Vector(radians(Ball_Deg - 30), 1);
                        ball.add(ball_);
                        move_deg = ball.get_deg();
                        move_speed = Ball_Distance > 90 ? 35 : 40;

                    } 
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


Attacker attack;

#endif